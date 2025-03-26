//
//  MovePhaseController.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/18/25.
//

#include "MovePhaseController.h"
#include "Constants.h"
#include "Platform.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "SSBDudeModel.h"
#include "WindObstacle.h"
#include "LevelModel.h"
#include "ObjectController.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::physics2;
using namespace cugl::audio;
using namespace Constants;

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

#pragma mark -
#pragma mark Constructors
/**
 * Creates the move phase controller.
 */
MovePhaseController::MovePhaseController() {
    _complete = false;
    _debug = false;
}

/**
 * Initializes the controller contents.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MovePhaseController::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::distrib::NetWorld>& world, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound) {
    if (assets == nullptr)
    {
        return false;
    }

    _assets = assets;
    _world = world;
    _input = input;
    _gridManager = gridManager;

    _networkController = networkController;
    _network = networkController->getNetwork();

    // Init network
    // TODO: Maybe move to network controller
    //Make a std::function reference of the linkSceneToObs function in game scene for network controller
    std::function<void(const std::shared_ptr<physics2::Obstacle>&,const std::shared_ptr<scene2::SceneNode>&)> linkSceneToObsFunc = [=,this](const std::shared_ptr<physics2::Obstacle>& obs, const std::shared_ptr<scene2::SceneNode>& node) {
        this->_movePhaseScene.linkSceneToObs(obs,node);
    };
    _network->enablePhysics(_world, linkSceneToObsFunc);

    _networkController->setObjects(&_objects);
    _networkController->setWorld(_world);
    

    // Initialize move phase scene
    _movePhaseScene.init(assets, world, gridManager, networkController, &_objects);
    _camera = _movePhaseScene.getCamera();
    _objectController = _movePhaseScene.getObjectController();
    _sound = sound;

    // Initalize UI Scene
    _uiScene.setTotalRounds(TOTAL_ROUNDS);
    _uiScene.init(assets);

    _playerStart = _movePhaseScene.getLocalPlayer()->getPosition().x;
    _levelWidth = _movePhaseScene.getGoalDoor()->getPosition().x - _movePhaseScene.getLocalPlayer()->getPosition().x;

    setComplete(false);
    setFailure(false);
    _complete = false;
    setDebug(false);

    return true;
};

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MovePhaseController::dispose() {
    _complete = false;
    _debug = false;

    _movePhaseScene.dispose();
    _uiScene.dispose();
}

void MovePhaseController::reset() {
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void MovePhaseController::resetRound() {
//    setFailure(false);
//    setComplete(false);
    _movePhaseScene.resetPlayerProperties();
    _movePhaseScene.resetCameraPos();
    
//    _movePhaseScene.reset();
//    _uiScene.reset();
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void MovePhaseController::preUpdate(float dt) {
    
    
    // Process the toggled key commands
    if (_input->didDebug())
    {
        setDebug(!isDebug());
    }
    if (_input->didReset())
    {
        reset();
    }
    if (_input->didExit())
    {
        CULog("Shutting down");
        Application::get()->quit();
    }

    // Process the movement
    if (_input->withJoystick())
    {
        if (_input->getHorizontal() < 0)
        {
            _uiScene.setLeftVisible();
        }
        else if (_input->getHorizontal() > 0)
        {
            _uiScene.setRightVisible();
        }
        else
        {
            _uiScene.setJoystickHidden();
        }
        _uiScene.setJoystickPosition(_input->getJoystick());
    }
    else
    {
        _uiScene.setJoystickHidden();
    }

    //THE GLIDE BULLSHIT SECTION
    if (_input->getRightTapped()) {
        _input->setRightTapped(false);
        if (!_movePhaseScene.getLocalPlayer()->isGrounded())
        {
            _movePhaseScene.getLocalPlayer()->setGlide(true);
        }
    }
    else if (!_input->isRightDown()) {
        _movePhaseScene.getLocalPlayer()->setGlide(false);
    }

//        if (_input->getRightTapped()) {
//            _input->setRightTapped(false);
//            if (!_avatar->isGrounded())
//            {
//                _avatar->setGlide(true);
//            }
//        }
//        else if (!_input->isRightDown()) {
//            _avatar->setGlide(false);
//
//        }
    
    _movePhaseScene.getLocalPlayer()->setGlide(_uiScene.getDidGlide());
    _movePhaseScene.getLocalPlayer()->setMovement(_input->getHorizontal() * _movePhaseScene.getLocalPlayer()->getForce());
    _movePhaseScene.getLocalPlayer()->setJumping(_uiScene.getDidJump());
    _movePhaseScene.getLocalPlayer()->applyForce();

    if (_movePhaseScene.getLocalPlayer()->isJumping() && _movePhaseScene.getLocalPlayer()->isGrounded())
    {
        _sound->playSound("jump");
    }

    if (_movePhaseScene.getLocalPlayer()->isGrounded() && !_uiScene.isGlideDown()){
        _uiScene.setJumpButtonActive();
        _uiScene.setDidGlide(false);
    }
    else if (!_movePhaseScene.getLocalPlayer()->isGrounded() && !_uiScene.isJumpDown()){
        _uiScene.setGlideButtonActive();
        _uiScene.setDidJump(false);
    }

    float player_pos = _movePhaseScene.getLocalPlayer()->getPosition().x;
    if (player_pos < _playerStart){
        _uiScene.setRedIcon(0, _levelWidth);
    }
    else if (player_pos > _playerStart){
        _uiScene.setRedIcon(_levelWidth, _levelWidth);
    }
    else{
        _uiScene.setRedIcon(player_pos - _playerStart, _levelWidth);
    }
    

    getCamera()->setPosition(Vec3(getCamera()->getPosition().x + (7 * dt) * (_movePhaseScene.getLocalPlayer()->getPosition().x * 56 + SCENE_WIDTH / 3.0f - getCamera()->getPosition().x), getCamera()->getPosition().y, 0));


    _movePhaseScene.preUpdate(dt);
    if (_mushroomCooldown > 0) {
        _mushroomCooldown--;
    }
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void MovePhaseController::postUpdate(float remain) {
    // Check for player death by falling into void
    if (!_failed && _movePhaseScene.getLocalPlayer()->getY() < 0)
    {
        killPlayer();
    }

    if (!_failed && _died){
        setFailure(true);
    }

//    if(_reachedGoal){
//        nextRound(true);
//    }
}

void MovePhaseController::setSpriteBatch(const shared_ptr<SpriteBatch> &batch) {
    _movePhaseScene.setSpriteBatch(batch);
    _uiScene.setSpriteBatch(batch);
}

void MovePhaseController::render() {
    _movePhaseScene.render();
    _uiScene.render();
}



/**
 * Triggers a change in building mode.
 */


#pragma mark -
#pragma mark State Access
/**
 * Sets whether the level is completed.
 *
 * If true, the level will advance after a countdown
 *
 * @param value whether the level is completed.
 */
void MovePhaseController::setComplete(bool value)
{
    bool change = _complete != value;
    _complete = value;
    if (value && change)
    {
        _sound->playMusic("win");
        _uiScene.setWinVisible(true);
        _countdown = EXIT_COUNT;
    }
    else if (!value)
    {
        _uiScene.setWinVisible(false);
        _countdown = -1;
    }
}

/**
 * Sets whether the level is failed.
 *
 * If true, the level will reset after a countdown
 *
 * @param value whether the level is failed.
 */
void MovePhaseController::setFailure(bool value) {
    if (value) {
        // If next round available, do not fail
        if (_currRound < TOTAL_ROUNDS){
            if (_movePhaseScene.getLocalPlayer()->_hasTreasure){
                _movePhaseScene.setNextTreasure(_currGems);
            }

//            nextRound();
            return;
        }

        _sound->playMusic("lose");
        _uiScene.setLoseVisible(true);
        _countdown = EXIT_COUNT;
    }
    else
    {
        _uiScene.setLoseVisible(false);
        _countdown = -1;
    }
    _failed = value;
}

/**
* Sets the level up for the next round.
*
* When called, the level will reset after a countdown.
*
*/
void MovePhaseController::nextRound(bool reachedGoal) {
    // Check if player won before going to next round
    if (reachedGoal){
        if(_movePhaseScene.getLocalPlayer()->_hasTreasure){
            _movePhaseScene.getLocalPlayer()->removeTreasure();
            // Increment total treasure collected
            _currGems += 1;
            // Update score image
            _uiScene.setScoreImageFull(_currGems - 1);

            // Check if player won
            if (_currGems == TOTAL_GEMS){
                setComplete(true);
                return;
            }
            else{
                // Set up next treasure if collected in prev round
                _movePhaseScene.setNextTreasure(_currGems);
            }

        }
    }

    // Check if player lost
    if (_currRound == TOTAL_ROUNDS && _currGems != TOTAL_GEMS){
        setFailure(true);
        return;
    }

    // Increment round
    _currRound += 1;
    // Update text
    _uiScene.updateRound(_currRound, TOTAL_ROUNDS);

    setFailure(false);

    // Reset player properties
    _movePhaseScene.resetPlayerProperties();
    _died = false;
    _reachedGoal = false;

    // Reset growing wall
//    _growingWallWidth = 0.1f;
//    _growingWallNode->setVisible(false);
}


void MovePhaseController::killPlayer(){
    std::shared_ptr<DudeModel> player = _movePhaseScene.getLocalPlayer();
    // Send message to network that the player has ended their movement phase
    if (!player->isDead()){
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::MOVEMENT_END));
        
        player->setDead();
    }
    
}

#pragma mark -
#pragma mark Collision Handling
/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */
void MovePhaseController::beginContact(b2Contact *contact)
{
    b2Fixture *fix1 = contact->GetFixtureA();
    b2Fixture *fix2 = contact->GetFixtureB();

    contact->GetChildIndexA();

    b2Body *body1 = fix1->GetBody();
    b2Body *body2 = fix2->GetBody();

    std::string *fd1 = reinterpret_cast<std::string *>(fix1->GetUserData().pointer);
    std::string *fd2 = reinterpret_cast<std::string *>(fix2->GetUserData().pointer);

    physics2::Obstacle *bd1 = reinterpret_cast<physics2::Obstacle *>(body1->GetUserData().pointer);
    physics2::Obstacle *bd2 = reinterpret_cast<physics2::Obstacle *>(body2->GetUserData().pointer);


    // Check if both are players and disable contact
        if ((bd1->getName() == "player" && bd2->getName() == "player") ||
            (bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "player") ||
            (bd2 == _movePhaseScene.getLocalPlayer().get() && bd1->getName() == "player")) {
            contact->SetEnabled(false);
        }
    // See if we have landed on the ground.

    if (((_movePhaseScene.getLocalPlayer()->getSensorName() == fd2 && _movePhaseScene.getLocalPlayer().get() != bd1) ||
        (_movePhaseScene.getLocalPlayer()->getSensorName() == fd1 && _movePhaseScene.getLocalPlayer().get() != bd2)) && (bd2->getName() != "gust" && bd1->getName() != "gust"))
    {

        _movePhaseScene.getLocalPlayer()->setGrounded(true);
    }

//    if ((_movePhaseScene.getLocalPlayer()->getSensorName() == fd2 && _movePhaseScene.getLocalPlayer().get() != bd1) ||
//        (_movePhaseScene.getLocalPlayer()->getSensorName() == fd1 && _movePhaseScene.getLocalPlayer().get() != bd2))
//    {
//        _movePhaseScene.getLocalPlayer()->setGrounded(true);
//
//        // Could have more than one ground
//        _sensorFixtures.emplace(_movePhaseScene.getLocalPlayer().get() == bd1 ? fix2 : fix1);
//    }

    if ((_movePhaseScene.getLocalPlayer()->getSensorName() == fd2 && _movePhaseScene.getLocalPlayer().get() != bd1 && bd1->getName() != "player") ||
        (_movePhaseScene.getLocalPlayer()->getSensorName() == fd1 && _movePhaseScene.getLocalPlayer().get() != bd2 && bd2->getName() != "player")) {
        _movePhaseScene.getLocalPlayer()->setGrounded(true);

        // Could have more than one ground
        _sensorFixtures.emplace(_movePhaseScene.getLocalPlayer().get() == bd1 ? fix2 : fix1);
    }

    // If we hit the "win" door, we are done
    if((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2 == _movePhaseScene.getGoalDoor().get()) ||
        (bd1 == _movePhaseScene.getGoalDoor().get() && bd2 == _movePhaseScene.getLocalPlayer().get())) {
        _reachedGoal = true;

    }
    // If we hit a spike, we are DEAD
    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "spike") ||
        (bd1->getName() == "spike" && bd2 == _movePhaseScene.getLocalPlayer().get())) {
        //        setFailure(true);
//        _died = true;
        killPlayer();
    }
    //ounce if we hit a mushroom
    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "mushroom") ||
    (bd1->getName() == "mushroom" && bd2 == _movePhaseScene.getLocalPlayer().get())) {

        if (_mushroomCooldown == 0) {
            b2Body* playerBody = _movePhaseScene.getLocalPlayer()->getBody();
            b2Vec2 impulse(0.0f, 20.0f);
            playerBody->ApplyLinearImpulseToCenter(impulse, true);

            // Clip velocity AFTER impulse is applied
            b2Vec2 newVelocity = playerBody->GetLinearVelocity();
            if (newVelocity.y > 15.0f) {
                newVelocity.y = 15.0f;
                playerBody->SetLinearVelocity(newVelocity);
                CULog("Player vertical velocity clipped to 10.0f after bounce.");
            }

            _mushroomCooldown = 10;
            CULog("Mushroom bounce triggered; cooldown set to 10 frames.");
        }
    }



    // If the player collides with the growing wall, game over

//    if ((bd1 == _avatar.get() && bd2 == _growingWall.get()) ||
//        (bd1 == _growingWall.get() && bd2 == _avatar.get()))
//    {
//        _died = true;
//
//    }

    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _movePhaseScene.getLocalPlayer().get()))
    {
        //determine which of bd1 or bd2 is the wind object
        Vec2 windPos = Vec2();
        if (bd2->getName() == "gust") {
            windPos = bd2->getPosition();
        }
        else {
            windPos = bd1->getPosition();
        }
        //Find the appropriate object

        auto p = std::make_pair(floor(windPos.x), floor(windPos.y));
        if (_gridManager->posToObjMap.count(p) > 0) {
            CULog("WIND FOUND!");
            std::shared_ptr<Object> thing = _gridManager->posToObjMap[p];
            _movePhaseScene.getLocalPlayer()->addWind(thing->getTrajectory());
        }
    }

//    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2 == _growingWall.get()) ||
//        (bd1 == _growingWall.get() && bd2 == _movePhaseScene.getLocalPlayer().get()))
//    {
//        _died = true;
//
//    }

    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _movePhaseScene.getLocalPlayer().get()))
    {
        // CULog("WIND");
        _movePhaseScene.getLocalPlayer()->addWind(Vec2(0, 6));
    }

    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "movingPlatform" && _movePhaseScene.getLocalPlayer()->isGrounded()) ||
        (bd2 == _movePhaseScene.getLocalPlayer().get() && bd1->getName() == "movingPlatform" && _movePhaseScene.getLocalPlayer()->isGrounded()))
    {
//        CULog("moving platform");
        _movePhaseScene.getLocalPlayer()->setOnMovingPlat(true);
        _movePhaseScene.getLocalPlayer()->setMovingPlat(bd1 == _movePhaseScene.getLocalPlayer().get() ? bd2 : bd1);

        // If we hit a spike, we are DEAD
        if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "spike") ||
            (bd1->getName() == "spike" && bd2 == _movePhaseScene.getLocalPlayer().get()))
        {
            killPlayer();
        }
    }

    // If we collide with a treasure, we pick it up
    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "treasure") ||
        (bd1->getName() == "treasure" && bd2 == _movePhaseScene.getLocalPlayer().get()))
    {
        if (!_movePhaseScene.getLocalPlayer()->_hasTreasure && !_movePhaseScene.getTreasure()->isTaken())
        {
            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::TREASURE_TAKEN));
//            _movePhaseScene.getTreasure()->setTaken(true);
            _movePhaseScene.getLocalPlayer()->gainTreasure(_movePhaseScene.getTreasure());
        }
    }
}

/**
 * Callback method for the start of a collision
 *
 * This method is called when two objects cease to touch.  The main use of this method
 * is to determine when the characer is NOT on the ground.  This is how we prevent
 * double jumping.
 */
void MovePhaseController::endContact(b2Contact *contact)
{
    b2Fixture *fix1 = contact->GetFixtureA();
    b2Fixture *fix2 = contact->GetFixtureB();

    b2Body *body1 = fix1->GetBody();
    b2Body *body2 = fix2->GetBody();

    std::string *fd1 = reinterpret_cast<std::string *>(fix1->GetUserData().pointer);
    std::string *fd2 = reinterpret_cast<std::string *>(fix2->GetUserData().pointer);

    physics2::Obstacle *bd1 = reinterpret_cast<physics2::Obstacle *>(body1->GetUserData().pointer);
    physics2::Obstacle *bd2 = reinterpret_cast<physics2::Obstacle *>(body2->GetUserData().pointer);

    if ((_movePhaseScene.getLocalPlayer()->getSensorName() == fd2 && _movePhaseScene.getLocalPlayer().get() != bd1) ||
        (_movePhaseScene.getLocalPlayer()->getSensorName() == fd1 && _movePhaseScene.getLocalPlayer().get() != bd2))
    {
        _sensorFixtures.erase(_movePhaseScene.getLocalPlayer().get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty())
        {
            _movePhaseScene.getLocalPlayer()->setGrounded(false);
        }
    }

    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "movingPlatform") ||
        (bd2 == _movePhaseScene.getLocalPlayer().get() && bd1->getName() == "movingPlatform"))
    {
//        CULog("disable movement platform");
        _movePhaseScene.getLocalPlayer()->setOnMovingPlat(false);
        _movePhaseScene.getLocalPlayer()->setMovingPlat(nullptr);
    }

    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _movePhaseScene.getLocalPlayer().get()))
    {
        //determine which of bd1 or bd2 is the wind object
        Vec2 windPos = Vec2();
        if (bd2->getName() == "gust") {
            windPos = bd2->getPosition();
        }
        else {
            windPos = bd1->getPosition();
        }
        //Find the appropriate object

        auto p = std::make_pair(floor(windPos.x), floor(windPos.y));
        if (_gridManager->posToObjMap.count(p) > 0) {
            CULog("WIND FOUND!");
            std::shared_ptr<Object> thing = _gridManager->posToObjMap[p];
            _movePhaseScene.getLocalPlayer()->addWind(-(thing->getTrajectory()));
        }
    }
}
