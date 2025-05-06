//
//  PlayerModel.h
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
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#ifndef __PF_PLAYER_MODEL_H__
#define __PF_PLAYER_MODEL_H__
#include <cugl/cugl.h>
#include "Treasure.h"
#include "Constants.h"
#include "Message.h"
#include "AnimationEvent.h"

using namespace cugl;
using namespace Constants;

#pragma mark -
#pragma mark Drawing Constants

/** Identifier to allow us to track the sensor in ContactListener */
#define SENSOR_NAME     "dudesensor"


#pragma mark -
#pragma mark Physics Constants
/** The factor to multiply by the input */
#define PLAYER_FORCE      20.0f
/** The amount to slow the character down in the air */
#define PLAYER_DAMPING    20.0f
/** The maximum character speed */
#define PLAYER_MAXSPEED   5.5f
#define PLAYER_MAX_Y_SPEED 12.5f
/*Amount of forgiveness for missing a jump input.*/
#define COYOTE_TIME_DURATION 0.1f;
/**How much the player speed should be dampened during gliding*/
#define GLIDE_DAMPING 1.5f
#define GLIDE_FORCE_FACTOR 1.0f
/**Initial burst of speed when the player begins gliding or changes direction*/
#define GLIDE_BOOST_FACTOR 1.2f
/** Multipliers for wind speed when player is gliding and not gliding*/
#define WIND_FACTOR 0.05f
#define WIND_FACTOR_GLIDING 0.4f
#define WIND_FACTOR_AIR 0.08f
//At what distance should wind be functional for gliding vs non gliding players
#define WIND_DIST_THRESHOLD 0.6f
//Determines for how long we can 'halt' a jump middair, allowing the player to control how high they jump
#define JUMP_DURATION 0.6f
#define JUMP_STOP_DAMPING 0.2f
#define JUMP_BUFFER_DURATION 0.2f
#define GLIDE_FALL_SPEED -2.5f
#define GLIDE_UPWARD_THRUST 15.0f
//How much we should slow down the player when they turn middair
#define MIDDAIR_DAMPING 0.6f
#define GROUND_DAMPING 0.3f
#define STARTING_VELOCITY 1.75f
/*Air friction*/
#define AIR_DAMPING = 2.5f




#pragma mark -
#pragma mark Player Model
/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class PlayerModel : public physics2::CapsuleObstacle {
private:
	/** This macro disables the copy constructor (not allowed on physics objects) */
	CU_DISALLOW_COPY_AND_ASSIGN(PlayerModel);
    
    std::shared_ptr<Treasure> _treasure;

protected:
    /*Stores our current state*/
    enum class State {
        GLIDING, GROUNDED, MIDDAIR
    };
    State _state;
    //Temp variable
    bool _getGrounded;
	/** The current horizontal movement of the character */
	float _movement;
	/** Which direction is the character facing */
	bool _faceRight;
	/** How long until we can jump again */
	int  _jumpCooldown;
	/** Whether we are actively jumping */
	bool _isJumping;
    /*Keeps track of coyote time. */
    float _coyoteTimer = 0.0f;
    /*Manages the jump buffer. If we press the jump input in the air and release, put in a jump buffer*/
    bool _bufferEnabled;
    float _bufferTimer = JUMP_BUFFER_DURATION;
    /*Whether or not we are holding down the jump button while jumping-True while we are holding jump from a jump.*/
    bool _isHeld;
    /*How long a jump should last. If we let go of jump before this is over, set the linear velocity to zero.**/
    float _jumpDuration;
    /*how tall the player is*/
    float _height;
	/** Whether our feet are on the ground */
	bool _isGrounded;
    /** if standing on moving platform */
    bool _onMovingPlat;
    /** points to moving platform standing on*/
    physics2::Obstacle* MovingPlat;
    
    /** Whether is dead  */
    bool _isDead = false;
    /** Whether the player hasn't died (hasn't initiated the death animation) */
    bool _canDie = true;

    /** Whether is immobile */
    bool _immobile = true;

    /** Whether is ready */
    bool _ready = false;

    /** Whether we are gliding, and how long we need to fall for to intiate 'glide mode'*/
    float _glideDelay;
    float _glideTimer;
    float _autoGlideTimer;
    bool _isGliding;
    bool _justGlided = false;
    bool _justExitedGlide = false;
    /*Variables in charge of the vertical glide boost generated by 'air resistance'*/
    float _glideBoostTimer;
    float _glideBoostDelay = 0.9f;
    float _glideBoostImpulse = 0.5f;
    //Returns whether or not we have just flipped our character from left to right this update frame.
    bool _justFlipped = false;
    /**Wind gust variables. Controls multipliers for how much it should affect the player in and out of gliding, 
    as well as how much motion is being applied at any given time*/
    Vec2 _windVel;
    //Stores how far the player is away from wind at moment of gust blowing the player.
    float _windDist;
    
    //Handles jump damping. Jumptimer starts counting down upon jumping. During this time, release jump to dampen your vertical velocity.
    float _jumpTimer = 0.0f;

    bool _holdingJump;
    //False if we are on PCS
    bool _isDampEnabled = true;
    //Stores the player's previous position. Used for platform logic
    Vec2 _prevPos;

	/** Ground sensor to represent our feet */
	b2Fixture*  _sensorFixture;
    
	/** Reference to the sensor name (since a constant cannot have a pointer) */
	std::string _sensorName;
	/** The node for debugging the sensor */
	std::shared_ptr<scene2::WireNode> _sensorNode;

	/** The scene graph node for the Dude. This holds the animation sprite  */
    std::shared_ptr<scene2::SceneNode> _node;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;

#pragma mark Animation Variables
    /** Manager to process the animation actions */
    std::shared_ptr<cugl::ActionTimeline> _timeline;
    
    /** Idle animation variables */
    std::shared_ptr<AnimateSprite> _idleAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _idleSpriteNode;
    cugl::ActionFunction _idleAction;
    
    /** Walk animation variables */
    std::shared_ptr<AnimateSprite> _walkAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _walkSpriteNode;
    cugl::ActionFunction _walkAction;
    
    /** Glide animation variables */
    std::shared_ptr<AnimateSprite> _glideAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _glideSpriteNode;
    cugl::ActionFunction _glideAction;
    
    /** Jump animation variables */
    std::shared_ptr<AnimateSprite> _jumpAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _jumpSpriteNode;
    cugl::ActionFunction _jumpAction;

    /** Death animation variables */
    std::shared_ptr<AnimateSprite> _deathAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _deathSpriteNode;
    cugl::ActionFunction _deathAction;

	/**
	* Redraws the outline of the physics fixtures to the debug node
	*
	* The debug node is use to outline the fixtures attached to this object.
    *
    * Also adds scene node children to it (used for animations). Removes all children and adds the node if it already exists.
    *
	* This is very useful when the fixtures have a very different shape than
	* the texture (e.g. a circular shape attached to a square texture).
	*/
	virtual void resetDebug() override;

public:
    
    bool hasTreasure = false;
    
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a PlayerModel, you must call init().
     */
    PlayerModel() : CapsuleObstacle(), _sensorName(SENSOR_NAME) { }
    
    /**
     * Destroys this PlayerModel, releasing all resources.
     */
    virtual ~PlayerModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this PlayerModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a PlayerModel may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Initializes a new dude at the origin.
     *
     * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO, Size(1,1), 1.0f, ColorType::RED); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude is unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2 pos) override { return init(pos, Size(1,1), 1.0f, ColorType::RED); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2 pos, const Size size) override {
        return init(pos, size, 1.0f, ColorType::RED);
    }
    
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
    virtual bool init(const Vec2& pos, const Size& size, float scale, ColorType color);
    
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Creates a new dude at the origin.
	 *
	 * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @return  A newly allocated PlayerModel at the origin
	 */
	static std::shared_ptr<PlayerModel> alloc() {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init() ? result : nullptr);
	}

	/**
	 * Creates a new dude at the given position.
	 *
	 * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
     * @param pos   Initial position in world coordinates
	 *
	 * @return  A newly allocated PlayerModel at the given position
	 */
	static std::shared_ptr<PlayerModel> alloc(const Vec2& pos) {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init(pos) ? result : nullptr);
	}

    /**
	 * Creates a new dude at the given position.
	 *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
	 *
 	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
	 *
	 * @return  A newly allocated PlayerModel at the given position with the given scale
	 */
	static std::shared_ptr<PlayerModel> alloc(const Vec2& pos, const Size& size) {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

	/**
	 * Creates a new dude at the given position.
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
	 * @return  A newly allocated PlayerModel at the given position with the given scale
	 */
	static std::shared_ptr<PlayerModel> alloc(const Vec2& pos, const Size& size, float scale, ColorType color) {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init(pos, size, scale, color) ? result : nullptr);
	}
    

#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this PlayerModel.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlayerModel.
     */
	const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _node; }

    /**
     * Sets the scene graph node representing this PlayerModel.
     *
     * Note that this method also handles creating the nodes for the body parts
     * of this PlayerModel. Since the obstacles are decoupled from the scene graph,
     * initialization (which creates the obstacles) occurs prior to the call to
     * this method. Therefore, to be drawn to the screen, the nodes of the attached
     * bodies must be added here.
     *
     * The bubbler also uses the world node when adding bubbles to the scene, so
     * the input node must be added to the world BEFORE this method is called.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this PlayerModel, which has been added to the world node already.
     */
	void setSceneNode(const std::shared_ptr<scene2::SpriteNode>& node) {

        if (!_node){
            _node = scene2::SceneNode::alloc();
        } else{
            _node->removeAllChildren();
        }
        _node->addChild(node);
        _node->setPosition(getPosition() * _drawScale);
    }
    
    /** Sets the idle animation and adds the idle sprite node to the scene node (_node) */
    void setIdleAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);

    /** Sets the walk animation and adds the walk sprite node to the scene node (_node) */
    void setWalkAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);

    /** Sets the glide animation and adds the glide sprite node to the scene node (_node) */
    void setGlideAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);

    /** Sets the jump animation and adds the jump sprite node to the scene node (_node) */
    void setJumpAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);

    /** Sets the death animation and adds the death sprite node to the scene node (_node) */
    void setDeathAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);

    /**
     * Performs a film strip action
     *
     * @param key   The action key
     * @param action The film strip action
     * @param slide  The associated movement slide
     */
    void doStrip(std::string key, cugl::ActionFunction action, float duration);

    /** Sets which animation color strip to use for the player */
    void setAnimationColors(ColorType color);
    
    void processNetworkAnimation(AnimationType animation, bool activate);
    
    /**
     * Called when the player obtains a treasure.
     *
     * Puts the treasure in this player's posession such that they now have ownership over it.
     */
    void gainTreasure(const std::shared_ptr<Treasure>& treasure){
        hasTreasure = true;
        _treasure = treasure;
    };
    
    /**
     * Called when the player loses a treasure.
     *
     * Removes the treasure from this player's posession.
     */
    void removeTreasure(){
        hasTreasure = false;
        _treasure = nullptr;        
    };
    
    /**
     * Sets the player as dead.
     */
    void setDead(bool value){
        _isDead = value;
    }
    
    /**
     * Returns whether the player is dead
     */
    bool isDead(){
        return _isDead;
    }
    
    /**
     * Sets the player as immobile.
     */
    void setImmobile(bool value){
        _immobile = value; 
    }
    
    /**
     * Returns whether the player is immobile
     */
    bool getImmobile(){
        return _immobile;
    }

    /**
     * Returns whether the player is ready
     */
    void setReady(bool ready){
        _ready = ready;
    }

    /**
     * Returns whether the player is ready
     */
    bool getReady(){
        return _ready;
    }
    

    
#pragma mark -
#pragma mark Attribute Properties
    /*Gets player state. All player state actions should be resolve in update*/
    State getState() const { return _state; }
    /*THE ONLY FUNCTION THAT SHOULD BE ABLE TO CHANGE PLAYER STATES*/
    void handlePlayerState();

    /**
     * Returns left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @return left/right movement of this character.
     */
    float getMovement() const { return _movement; }
    
    /**
     * Sets left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @param value left/right movement of this character.
     */
    void setMovement(float value);
    
    /**
     * Update the visual direction the dude is facing
     */
    void updateFacing();

    /**
    Applies a certain amount of wind velocity to the player
    */
    void addWind(Vec2 wind, float dist) { _windVel.operator+=(wind); _windDist = dist; };
    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    bool isJumping() const { return _isJumping && _jumpCooldown <= 0; }
    
    /**
     * Sets whether the dude is actively jumping.
     *
     * @param value whether the dude is actively jumping.
     */
    void setJumping(bool value) { _isJumping = value; }

    /*Buffers a jump input. If we become grounded before the buffer duration is over, intiatie a jump**/

    void bufferJump() { _bufferTimer = 0.0f; }

    /**
     * Returns true if the dude is on the ground.
     *
     * @return true if the dude is on the ground.
     */
    bool isGrounded() const { return _isGrounded; }
    
    /**
     * Sets whether the dude is on the ground.
     *
     * @param value whether the dude is on the ground.
     */
    void setGrounded(bool value) { _isGrounded = value; }
    
    /**
     * Returns how much force to apply to get the dude moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the dude moving
     */
    float getForce() const { return PLAYER_FORCE; }
    
    /**
     * Returns ow hard the brakes are applied to get a dude to stop moving
     *
     * @return ow hard the brakes are applied to get a dude to stop moving
     */
    float getDamping() const { return PLAYER_DAMPING; }
    
    /**
     * Returns the upper limit on dude left-right movement.
     *
     * This does NOT apply to vertical movement.
     *
     * @return the upper limit on dude left-right movement.
     */
    float getMaxSpeed() const { return PLAYER_MAXSPEED; }
    
    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string* getSensorName() { return &_sensorName; }

    float getFeetHeight() { return getPosition().y - (_height * 0.5); }
    float getPrevFeetHeight() { return _prevPos.y - (_height * 0.5); }
    //_sensorFixture->GetBody()->GetTransform().p.y;
    
    
    /**
     * Returns true if this character is facing right
     *
     * @return true if this character is facing right
     */
    bool isFacingRight() const { return _faceRight; }
    
    /** Returns whether the character is standing on a moving platform */
    bool isOnMovingPlatform() const { return _onMovingPlat; }

    /** Returns a pointer to the moving platform the character is standing on */
    physics2::Obstacle* getMovingPlatform() const { return MovingPlat; }


    /** 
    * set if on movingplatform
    * @param on set is on movingplatform
    */
    void setOnMovingPlat(bool on) {_onMovingPlat = on;}

    /** 
    * set moving platform moving on
    * @param plot pltform standing on
    */
    void setMovingPlat(physics2::Obstacle* plat) {MovingPlat = plat;}

    /**Sets whether we are trying to glide or not.*/
    void setGlide(bool value) { if (!_isGliding && _isGliding != value) { _justGlided = true; }
    else if (_isGliding && !value) { _justExitedGlide = true; };_isGliding = value;
    }
    
    /*If we have are currently holding the jump button**/

    void setJumpHold(bool value) { _holdingJump = value; }

    bool getJumpHold() { return _holdingJump; }

    /**Enable/disable jump damping*/
    void setJumpDamping(bool value) { _isDampEnabled = value; }

    ColorType getColor();

#pragma mark -
#pragma mark Physics Methods
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;
    
    /**
     Sets the filtering data for collisions.
     */
    void setFilterData();
    
    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt) override;
    
    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();
    /**
    Checks whether or not we should be in glide mode, and updates accordingly.
    */
    void glideUpdate(float dt);
    /**
    Processes the wind motion applied to the player.
    */
    void windUpdate(float dt);

    /** Reset the player's movements in between rounds by setting it all to zero and to face the right */
    void resetMovement();
    
    /** Reset the player */
    void reset();

#pragma mark -
#pragma mark Helpers
    /**
     * Checks whether the player is visible or not.
     */
    bool isVisible() {
        return _node->isVisible();
    }

    /**
     * Sets whether the player is visible or not.
     */
    void setVisible(bool value) {
        _node->setVisible(value);
    }

};

#endif /* __PF_PLAYER_MODEL_H__ */
