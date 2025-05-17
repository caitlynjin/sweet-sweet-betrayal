//
//  PFPlayerModel.cpp
//  PlatformDemo
//
//  This encapsulates all of the information for the character avatar.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  Pay close attention to how this class designed.  Subclasses of Cocos2d classes
//  (which are all subclasses of the class Ref) should never have normal public
//  constructors.  Instead, you should organize their constructors into three parts,
//  just like we have done in this class.
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#include "PlayerModel.h"
#include <cugl/scene2/CUPolygonNode.h>
#include <cugl/scene2/CUTexturedNode.h>
#include <cugl/core/assets/CUAssetManager.h>

#define SIGNUM(x) ((x > 0) - (x < 0))

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN 2
/** Cooldown (in animation frames) for shooting */
#define SHOOT_COOLDOWN 20
/** The amount to shrink the body fixture (vertically) relative to the image */
#define PLAYER_VSHRINK 0.7f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define PLAYER_HSHRINK 0.60f
/** The amount to shrink the sensor fixture (horizontally) relative to the image */
#define PLAYER_SSHRINK 0.8f
/** Height of the sensor attached to the player's feet */
#define SENSOR_HEIGHT 0.1f
/** The density of the character */
#define PLAYER_DENSITY 3.25f
/** The impulse for the character jump */

#define PLAYER_JUMP 23.5f
/** Debug color for the sensor */
#define DEBUG_COLOR Color4::RED
/** Multipliers for wind speed when player is gliding and not gliding*/
#define AIR_DAMPING 1.75f
#define SPRITE_ANCHOR Vec2(0.55f,0.20f)
#define SPRITE_POSITION Vec2(-13.0f,0.0f)

#pragma mark -
#pragma mark Animation Constants
/** Define the time settings for animation */
#define DURATION 1.0f
/** Action key for idle */
#define IDLE_ACTION_KEY     "idle"
/** Action key for walk */
#define WALK_ACTION_KEY     "walk"
/** Action key for glide */
#define GLIDE_ACTION_KEY    "glide"
/** Action key for jump */
#define JUMP_ACTION_KEY     "jump"
/** Action key for death */
#define DEATH_ACTION_KEY    "death"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool PlayerModel::init(const Vec2 &pos, const Size &size, float scale, ColorType color)
{
    Size nsize = size;
    nsize.width *= PLAYER_HSHRINK;
    nsize.height *= PLAYER_VSHRINK;
    _height = (nsize.height * PLAYER_VSHRINK) * 0.5;
    _drawScale = scale;

    MovingPlat = nullptr;

    if (CapsuleObstacle::init(pos, nsize*0.5, cugl::poly2::Capsule::FULL))
    {
        setDensity(PLAYER_DENSITY);
        setFriction(0.0f);      // HE WILL STICK TO WALLS IF YOU FORGET
        setFixedRotation(true); // OTHERWISE, HE IS A WEEBLE WOBBLE
        
        // Set tag of player based on color
        if (color == ColorType::RED){
            setName("playerRed");
        }
        else if (color == ColorType::BLUE){
            setName("playerBlue");
        }
        else if (color == ColorType::GREEN){
            setName("playerGreen");
        }
        else if (color == ColorType::YELLOW){
            setName("playerYellow");
        }
        
        setDebugColor(Color4::YELLOW);
        
        _node = scene2::SpriteNode::alloc();

        _node->setColor(Color4::CLEAR);

        _node->setPriority(3);


        // Gameplay attributes
        _isGrounded = false;
        _isJumping = false;
        _faceRight = true;
        _isGliding = false;
        _jumpCooldown = 0;
        _glideDelay = 0.2;
        _glideTimer = 0;
        _windVel = Vec2();

        setVisible(false);

        return true;
    }

    return false;
}

#pragma mark -
#pragma mark Animation

/** Sets the idle animation and adds the idle sprite node to the scene node (_node) */
void PlayerModel::setIdleAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _idleSpriteNode = sprite;
    
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }
    
    _idleSpriteNode->setAnchor(SPRITE_ANCHOR.x, SPRITE_ANCHOR.y);
    _idleSpriteNode->setPosition(SPRITE_POSITION);
    _node->addChild(_idleSpriteNode);
    _idleSpriteNode->setVisible(true);
    _idleSpriteNode->setRelativeColor(false);
    
    _timeline = ActionTimeline::alloc();
    
    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _idleAnimateSprite = AnimateSprite::alloc(forward);
    _idleAction = _idleAnimateSprite->attach<scene2::SpriteNode>(_idleSpriteNode);
}

/** Sets the walk animation and adds the walk sprite node to the scene node (_node) */
void PlayerModel::setWalkAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _walkSpriteNode = sprite;
    
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }
    _walkSpriteNode->setAnchor(SPRITE_ANCHOR.x, SPRITE_ANCHOR.y);
    _walkSpriteNode->setPosition(SPRITE_POSITION);
    _node->addChild(_walkSpriteNode);
    _walkSpriteNode->setVisible(false);
    _walkSpriteNode->setRelativeColor(false);
    
    _timeline = ActionTimeline::alloc();
    
    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _walkAnimateSprite = AnimateSprite::alloc(forward);
    _walkAction = _walkAnimateSprite->attach<scene2::SpriteNode>(_walkSpriteNode);
}

/** Sets the glide animation and adds the glide sprite node to the scene node (_node) */
void PlayerModel::setGlideAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _glideSpriteNode = sprite;
    
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }

    _glideSpriteNode->setAnchor(SPRITE_ANCHOR.x, SPRITE_ANCHOR.y);
    _glideSpriteNode->setPosition(SPRITE_POSITION);
    _node->addChild(_glideSpriteNode);
    _glideSpriteNode->setVisible(false);
    _glideSpriteNode->setRelativeColor(false);
    
    _timeline = ActionTimeline::alloc();
    
    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _glideAnimateSprite = AnimateSprite::alloc(forward);
    _glideAction = _glideAnimateSprite->attach<scene2::SpriteNode>(_glideSpriteNode);
}

/** Sets the jump animation and adds the jump sprite node to the scene node (_node) */
void PlayerModel::setJumpAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _jumpSpriteNode = sprite;
    
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }
    _jumpSpriteNode->setAnchor(SPRITE_ANCHOR.x, SPRITE_ANCHOR.y);
    _jumpSpriteNode->setPosition(SPRITE_POSITION);
    _node->addChild(_jumpSpriteNode);
    _jumpSpriteNode->setVisible(false);
    _jumpSpriteNode->setRelativeColor(false);
    
    _timeline = ActionTimeline::alloc();
    
    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _jumpAnimateSprite = AnimateSprite::alloc(forward);
    _jumpAction = _jumpAnimateSprite->attach<scene2::SpriteNode>(_jumpSpriteNode);
}

/** Sets the death animation and adds the death sprite node to the scene node (_node) */
void PlayerModel::setDeathAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _deathSpriteNode = sprite;

    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }

    _deathSpriteNode->setAnchor(SPRITE_ANCHOR.x, SPRITE_ANCHOR.y);
    _deathSpriteNode->setPosition(SPRITE_POSITION);
    _node->addChild(_deathSpriteNode);
    _deathSpriteNode->setVisible(true);
    _deathSpriteNode->setRelativeColor(false);

    _timeline = ActionTimeline::alloc();

    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }

    // Create animations
    _deathAnimateSprite = AnimateSprite::alloc(forward);
    _deathAction = _deathAnimateSprite->attach<scene2::SpriteNode>(_deathSpriteNode);
}

/**
 * Performs a film strip action
 *
 * @param key   The action key
 * @param action The film strip action
 * @param slide  The associated movement slide
 */
void PlayerModel::doStrip(std::string key, cugl::ActionFunction action, float duration = DURATION) {
    if (_timeline->isActive(key)) {
        // NO OP
    } else {
        _timeline->add(key, action, duration);
    }
}

//void PlayerModel::setAnimationColors(ColorType color){
//    _walkSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_WALK_TEXTURE), 1, 3, 3);
//}

void PlayerModel::processNetworkAnimation(AnimationType animation, bool activate) {
    if (!activate) return;

    if (animation == AnimationType::DEATH) {
        CULog("playing dead animation");
        _timeline->add(DEATH_ACTION_KEY, _deathAction, 0.3f);   
        setDead(true);
        setGhost(_node, true);
    }
    else if (animation == AnimationType::GLIDE) {
        if (!_glideSpriteNode->isVisible()) {
            _idleSpriteNode ->setVisible(false);
            _walkSpriteNode ->setVisible(false);
            _glideSpriteNode->setVisible(true);
            _jumpSpriteNode->setVisible(false);
            _deathSpriteNode->setVisible(false);
        }
        doStrip(GLIDE_ACTION_KEY, _glideAction);
        _isGliding = true;
    }
}



#pragma mark -
#pragma mark Attribute Properties

/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void PlayerModel::createFixtures()
{
    if (_body == nullptr)
    {
        return;
    }

    CapsuleObstacle::createFixtures();
    
    // Add collision filtering to the main body fixture first
       if (getBody()) {
           b2Fixture* mainFixture = getBody()->GetFixtureList();
           if (mainFixture) {
               b2Filter filter;
               filter.categoryBits = CATEGORY_PLAYER;
               filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;
               mainFixture->SetFilterData(filter);
           }
       }
    
    b2FixtureDef sensorDef;
    sensorDef.density = PLAYER_DENSITY;
    sensorDef.isSensor = true;
    
    // Apply the same filter data to the sensor
    sensorDef.filter.categoryBits = CATEGORY_PLAYER;
    sensorDef.filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;

    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -PLAYER_SSHRINK * getWidth() / 2.0f;
    corners[0].y = (-getHeight() + SENSOR_HEIGHT) / 2.0f;
    corners[1].x = -PLAYER_SSHRINK * getWidth() / 2.0f;
    corners[1].y = (-getHeight() - SENSOR_HEIGHT) / 2.0f;
    corners[2].x = PLAYER_SSHRINK * getWidth() / 2.0f;
    corners[2].y = (-getHeight() - SENSOR_HEIGHT) / 2.0f;
    corners[3].x = PLAYER_SSHRINK * getWidth() / 2.0f;
    corners[3].y = (-getHeight() + SENSOR_HEIGHT) / 2.0f;

    b2PolygonShape sensorShape;
    sensorShape.Set(corners, 4);

    sensorDef.shape = &sensorShape;
    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());
    _sensorFixture = _body->CreateFixture(&sensorDef);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void PlayerModel::releaseFixtures()
{
    if (_body != nullptr)
    {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr)
    {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }
}
/**
 * Disposes all resources and assets of this PlayerModel
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a PlayerModel may not be used until it is initialized again.
 */
void PlayerModel::dispose()
{
    _core = nullptr;
    _node = nullptr;
    _sensorNode = nullptr;
}


#pragma mark -
#pragma mark Physics Methods
/**
 * Sets left/right movement of this character.
 *
 * This is the result of input times dude force.
 *
 * @param value left/right movement of this character.
 */
void PlayerModel::setMovement(float value)
{
    _movement = value;
    bool face = _movement > 0;
    if (_movement == 0 || _faceRight == face)
    {
        return;
    }

    // Change facing
    _faceRight = face;
    
    updateFacing();
}

/**
 * Update the visual direction the dude is facing
 */
void PlayerModel::updateFacing(){
    float flipValue = _faceRight ? 1.0f : -1.0f;
    float offsetValue = _faceRight ? -13.0f : 13.0f;
    if (_idleSpriteNode) {
        _idleSpriteNode->setScale(flipValue, 1.0f);
        _idleSpriteNode->setPosition(Vec2(offsetValue, 0.0f));
    }
    if (_walkSpriteNode) {
        _walkSpriteNode->setScale(flipValue, 1.0f);
        _walkSpriteNode->setPosition(Vec2(offsetValue, 0.0f));
    }
    if (_glideSpriteNode) {
        _glideSpriteNode->setScale(flipValue, 1.0f);
        _glideSpriteNode->setPosition(Vec2(offsetValue, 0.0f));
    }
    if (_jumpSpriteNode) {
        _jumpSpriteNode->setScale(flipValue, 1.0f);
        _jumpSpriteNode->setPosition(Vec2(offsetValue, 0.0f));
    }

    _justFlipped = true;
}
/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void PlayerModel::applyForce()
{

    if (!isEnabled())
    {
        return;
    }
    if (_isGrounded) {
        _isGliding = false;
    }
    //Manipulate the x velocity in this section

    b2Vec2 vel = _body->GetLinearVelocity();

    // Don't want to be moving. Damp out player motion when on the ground or not gliding
    if (getMovement() == 0.0f || _justFlipped)
    {
        
        if (isGrounded())
        {// Instant friction on the ground or when we flip on the ground
            vel.x = vel.x * GROUND_DAMPING;
        }
        //Friction middair, but less
        else if (!_isGliding) {
            vel.x = vel.x* MIDDAIR_DAMPING;
        }
        
    }
    //Apply a small linear velocity burst when we turn around on the ground, for gamefeel
    if (_justFlipped && isGrounded()) {
        if (_faceRight) {
//            CULog("FLIPBOOST");
            vel.x += STARTING_VELOCITY;
        }
        else if (!_faceRight) {
//            CULog("FLIPBOOST");
            vel.x -= STARTING_VELOCITY;
        }
    }
    _body->SetLinearVelocity(vel);

    // Velocity too high, clamp it. If we are gliding, remove clamps on maxspeed
    if (fabs(getVX()) >= getMaxSpeed() && !_isGliding)
    {
        setVX(SIGNUM(getVX()) * getMaxSpeed());
    }
    else if (_isGliding) {
        //More powerful horizontal movement while gliding to counteract damping
        b2Vec2 force(getMovement()*GLIDE_BOOST_FACTOR, 0);
        _body->ApplyForce(force, _body->GetPosition(), true);
    }
    else
    {
        b2Vec2 force(getMovement(), 0);
        _body->ApplyForce(force, _body->GetPosition(), true);
        b2Vec2 vel = _body->GetLinearVelocity();

        if (abs(vel.x)< STARTING_VELOCITY) {
            b2Vec2 vel = _body->GetLinearVelocity();
            if (getMovement()>0) {
                vel.x = STARTING_VELOCITY;
            }
            else if (getMovement() < 0) {
                vel.x = STARTING_VELOCITY * -1;
            }
            _body->SetLinearVelocity(vel);
        }
    }
    //Vertical velocity speed limit
    if (fabs(getVY()) >= PLAYER_MAX_Y_SPEED)
    {
        setVY(SIGNUM(getVY()) * PLAYER_MAX_Y_SPEED);
    }
    //Reduce our y velocity if we are gliding. Try to apply this before wind physics happns?
    if (getVY() <= GLIDE_FALL_SPEED && _isGliding) {
        setVY(GLIDE_FALL_SPEED);
    }
    else if (_justExitedGlide) {
        setVY(0);

    }
    
    // Jump!
    if ((isJumping() or _bufferTimer < JUMP_BUFFER_DURATION) && isGrounded())
    {
        b2Vec2 force(0, PLAYER_JUMP);
        _body->ApplyLinearImpulse(force, _body->GetPosition(), true);
        _jumpTimer = JUMP_DURATION;
        _holdingJump = true;
        if (isJumping()) {
            _holdingJump = true;
        }
        if (_bufferTimer < JUMP_BUFFER_DURATION) {
            CULog("BUFFERED");
        }
        
    }

    if (isGrounded()){
        _bufferTimer = JUMP_BUFFER_DURATION;
    }
    
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void PlayerModel::update(float dt)
{
    _justContactMovingPlat = false;

    _prevPos = getPosition();
    // ANIMATION
    // TODO: Move to method updateAnimation
    _timeline->update(dt);
    
    // Change player facing
    //TODO-FIX THIS SHIT TO RESPECT CONTROLS
    if (getVX() > 0) {
        _faceRight = true;
    } else if(getVX() < 0){
        _faceRight = false;
    }
    updateFacing();

    if (_isDead && _deathAction) {
        if (!_deathSpriteNode->isVisible()) {
            _idleSpriteNode->setVisible(false);
            _walkSpriteNode->setVisible(false);
            _glideSpriteNode->setVisible(false);
            _jumpSpriteNode->setVisible(false);
            _deathSpriteNode->setVisible(true);
        }
        // Only play the animation once
        if (_canDie && !_timeline->isActive(DEATH_ACTION_KEY)) {
            _timeline->add(DEATH_ACTION_KEY, _deathAction, 0.3f);
            _canDie = false;
        }
    } else if (!isGrounded() && _isGliding && _glideAction){
        if (!_glideSpriteNode->isVisible()) {
            _idleSpriteNode->setVisible(false);
            _walkSpriteNode->setVisible(false);
            _glideSpriteNode->setVisible(true);
            _jumpSpriteNode->setVisible(false);
            _deathSpriteNode->setVisible(false);
        }
        doStrip(GLIDE_ACTION_KEY, _glideAction);
    } else if (!isGrounded() && _jumpAction){
        if (!_jumpSpriteNode->isVisible()) {
            _idleSpriteNode->setVisible(false);
            _walkSpriteNode->setVisible(false);
            _glideSpriteNode->setVisible(false);
            _jumpSpriteNode->setVisible(true);
            _deathSpriteNode->setVisible(false);
        }
        doStrip(JUMP_ACTION_KEY, _jumpAction);
    } else if (abs(getVX()) < 0.1f  && _idleAction) {
        if (!_idleSpriteNode->isVisible()) {
            _idleSpriteNode->setVisible(true);
            _walkSpriteNode->setVisible(false);
            _glideSpriteNode->setVisible(false);
            _jumpSpriteNode->setVisible(false);
            _deathSpriteNode->setVisible(false);
        }
        doStrip(IDLE_ACTION_KEY, _idleAction);
    } else if (_walkAction) {
        if (!_walkSpriteNode->isVisible()) {
            _walkSpriteNode->setVisible(true);
            _idleSpriteNode->setVisible(false);
            _glideSpriteNode->setVisible(false);
            _jumpSpriteNode->setVisible(false);
            _deathSpriteNode->setVisible(false);
        }
        doStrip(WALK_ACTION_KEY, _walkAction, 0.3f);
    }

//     Should not move when immobile
    if (_immobile){
        setLinearVelocity(Vec2(0,0));
    }
    
    if (!_isDead && !_immobile){
        _canDie = true;


        windUpdate(dt);
        //Set Justflipped and justglided to instantly deactivate
        if (_justFlipped == true) {
            _justFlipped = false;
        }
        
        // Apply cooldowns
        if (isJumping())
        {
            _jumpCooldown = JUMP_COOLDOWN;
        }
        else
        {
            // Only cooldown while grounded
            _jumpCooldown = (_jumpCooldown > 0 ? _jumpCooldown - 1 : 0);
        }
        //Tracks how long we are in the air but not gliding
        if (!_isGrounded && !_isGliding) {
            _glideBoostTimer += dt;
        }
        else if (_isGrounded){
            _glideBoostTimer = 0.0f;
        }
        //Increment jump buffer
        if (!_isGrounded) {
            _bufferTimer += dt;
        }
        

        _jumpTimer -= dt;
        //Tracks how long we have been holding jump
        if (_holdingJump and !_isGrounded and !_isGliding and _isDampEnabled) {
            b2Vec2 vel = _body->GetLinearVelocity();
            if (vel.y <= 0) {
                _isGliding = true;
                _justGlided = true;
                CULog("autogliding");
            }
        }
        glideUpdate(dt);
        
        CapsuleObstacle::update(dt);
        
        if (_node != nullptr)
        {
            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
        
        // If the player has a treasure, update the position of the treasure such that
        // it follows the player
        if (_treasure != nullptr)
        {
            _treasure->setPosition(getPosition() + Vec2(0.0f, 1.2f));
        }
    }
        
    // Allows the player to still move on a moving platform even if dead

    if (_onMovingPlat&& MovingPlat != nullptr)
    {
        CULog("on moving platform");
        Vec2 platformVel = MovingPlat->getLinearVelocity();
        setPosition(getPosition() + platformVel * dt);
    }
}

void PlayerModel::handlePlayerState() {
    switch (_state) {
    case State::GROUNDED:
        break;
    case State::GLIDING:
        break;
    case State::MIDDAIR:
        break;
    default:
        std::cout << "Unknown player state.\n";
        break;
    }
}

// Based on the player motion, check if we are falling.
// If the player is falling for more than the glidetimer, set player into glide mode
// once player is grounded, turn off glidemode.

void PlayerModel::glideUpdate(float dt)
{
    b2Vec2 motion = _body->GetLinearVelocity();

    if (isGrounded()) {
        _isGliding = false;
    }

    if (_isGliding)
    {
        _body->SetLinearDamping(GLIDE_DAMPING);
        //If we just flipped while gliding, or just entered gliding, apply a small linear impulse.
        if (_justFlipped || _justGlided) {
            int face = SIGNUM(_movement);
            CULog("Boost player");
            b2Vec2 force(face * GLIDE_BOOST_FACTOR, 0);
            _body->ApplyLinearImpulse(force, _body->GetPosition(), true);
        }
        //Apply a small upwards boost middair to simulate air resistance experienced by gliding while falling-
        //Scales with how long the player has been middair, and how long since the player has last glided.
        if (_justGlided) {
            float thrust_mult = min(_glideBoostTimer, _glideBoostDelay);
            CULog("something seomthing %f", thrust_mult);
            b2Vec2 force(0, GLIDE_UPWARD_THRUST * (thrust_mult/ _glideBoostDelay));
            _body->ApplyLinearImpulse(force, _body->GetPosition(), true);
            //Also slightly slow down the player-
            b2Vec2 vel = _body->GetLinearVelocity();
            vel.x = vel.x * 0.75;
            _body->SetLinearVelocity(vel);

            _glideBoostTimer = 0.0f;
        }
    }
    else
    {
        _body->SetLinearDamping(0);
    }
    _justGlided = false;
    _justExitedGlide = false;
}
/**
Inflicts an appropriate force to the player based on _windspeed
*/
void PlayerModel::windUpdate(float dt)
{
    float mult = WIND_FACTOR_GLIDING;
    if (!_isGliding) {
        mult = WIND_FACTOR_AIR;
    }
    else if (_isGrounded) {
        mult = WIND_FACTOR;
    }

    if (_windDist < WIND_DIST_THRESHOLD || _isGliding) {
        b2Vec2 vel = _body->GetLinearVelocity();
        vel.x += _windVel.x * mult;
        vel.y += _windVel.y * mult;
        if (vel.y <= 0 && _windVel.y>0 && !_isGliding) {
            vel.y += 3*_windVel.y * mult;
        }
        
        _body->SetLinearVelocity(vel);
    }
    
    _windVel = Vec2(0, 0);
}

#pragma mark -
#pragma mark Scene Graph Methods
/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void PlayerModel::resetDebug()
{
    CapsuleObstacle::resetDebug();
    float w = PLAYER_SSHRINK * _dimension.width;
    float h = SENSOR_HEIGHT;
    Poly2 poly(Rect(-w / 2.0f, -h / 2.0f, w, h));
    _sensorNode = scene2::WireNode::allocWithTraversal(poly, poly2::Traversal::INTERIOR);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width / 2.0f, 0.0f));
   
    _debug->addChild(_sensorNode);
}

void PlayerModel::setFilterData() {
    // Get the body fixtures
    b2Fixture* fixture = getBody()->GetFixtureList();
    
    // Create a filter
    b2Filter filter;
    filter.categoryBits = CATEGORY_PLAYER;
    
    // Set what this object collides with (everything EXCEPT other players)
//    filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER & ~CATEGORY_DEFAULT;
    filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;
    
    // Apply the filter to all fixtures
    while (fixture != nullptr) {
        fixture->SetFilterData(filter);
        fixture = fixture->GetNext();
    }
}

void PlayerModel::reset(){
    _isDead = false;
    removeTreasure();
    resetMovement();
}

/** Resets the player's movements in between rounds by setting it all to zero and to face the right */
void PlayerModel::resetMovement(){
    setVX(0);
    setVY(0);
    setMovement(0);
    _faceRight = true;
    updateFacing();
}

ColorType PlayerModel::getColor() {
    if (getName() == "playerRed"){
        return ColorType::RED;
    }
    if (getName() == "playerBlue"){
        return ColorType::BLUE;
    }
    if (getName() == "playerGreen"){
        return ColorType::GREEN;
    }
    if (getName() == "playerYellow"){
        return ColorType::YELLOW;
    }
    // DEFAULT
    return ColorType::RED;
}

#pragma mark -
#pragma mark Helpers

/**
 * Sets whether the player is transparent.
 *
 * @param node      the player scene node
 * @param value     whether to set the player to transparent or not
 */
void PlayerModel::setGhost(const std::shared_ptr<cugl::scene2::SceneNode>& node, bool value) {
    if (value) {
        // Set transparent
        Color4 color = node->getColor();
        color.a = 150;
        node->setColor(color);
    } else {
        // Restore color
        Color4 color = node->getColor();
        color.a = 255;
        node->setColor(color);
    }

    for (const auto& child : node->getChildren()) {
        setGhost(child, value);
    }
}
