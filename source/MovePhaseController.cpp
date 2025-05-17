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
#include "PlayerModel.h"
#include "WindObstacle.h"
#include "LevelModel.h"
#include "ObjectController.h"
#include "ScoreEvent.h"
#include "GoalDoor.h"

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
    _sound = sound;

    _networkController = networkController;
    _network = networkController->getNetwork();

    // TODO: Maybe move to network controller
    //Make a std::function reference of the linkSceneToObs function in game scene for network controller
    std::function<void(const std::shared_ptr<physics2::Obstacle>&,const std::shared_ptr<scene2::SceneNode>&)> linkSceneToObsFunc = [=,this](const std::shared_ptr<physics2::Obstacle>& obs, const std::shared_ptr<scene2::SceneNode>& node) {
        this->_movePhaseScene.linkSceneToObs(obs,node);
    };
    _network->enablePhysics(_world, linkSceneToObsFunc);
    _networkController->setObjects(&_objects);
    _networkController->setWorld(_world);
    
    
    // SEPARATE INTO PART 2 FOR WHEN LEVEL NUMBER IS LOADED IN
    // OR DON'T CALL UPDATE UNTIL LEVEL NUMBER IS LOADED IN
    
    return true;
};

bool MovePhaseController::finishInit(){
    // Initialize move phase scene
    _movePhaseScene.init(_assets, _world, _gridManager, _networkController, &_objects);
    _camera = _movePhaseScene.getCamera();
    _objectController = _movePhaseScene.getObjectController();

    // Initalize UI Scene
//    _uiScene.setTotalRounds(TOTAL_ROUNDS);

    _uiScene.init(_assets, _networkController->getScoreController(),_networkController, _movePhaseScene.getLocalPlayer()->getName());
    _playerStart = _movePhaseScene.getLocalPlayer()->getPosition().x;
    _levelWidth = _movePhaseScene.getGoalDoor()->getPosition().x - _movePhaseScene.getLocalPlayer()->getPosition().x;

    setComplete(false);
    setFailure(false);
    _complete = false;
    setDebug(false);

    // Set the players inivisible at the start
    _networkController->getLocalPlayer()->setVisible(false);

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MovePhaseController::dispose() {
    _complete = false;
    _debug = false;

    _movePhaseScene.dispose();
    _uiScene.dispose();
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void MovePhaseController::resetRound() {

    _movePhaseScene.resetPlayerProperties();
    _movePhaseScene.resetCameraPos();

//    _movePhaseScene.reset();
//    _uiScene.reset();
}

/**
 * Resets the status of the game so that we can play again.
 */
void MovePhaseController::reset() {
    // TODO: Need to properly reset
    _currRound = 1;
    _mushroomCooldown = 0;
    
    
    setFailure(false);
    setComplete(false);

    _movePhaseScene.reset();
    _uiScene.reset();
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void MovePhaseController::preUpdate(float dt) {

    // Process the toggled key commands
    // TODO: segment into updateInput method
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

    _uiScene.preUpdate(dt);

    // Process the movement
    // TODO: Segment into updateMovement method
    if (_input->withJoystick())
    {
        if (_input->getHorizontal() > 0)
        {
            _uiScene.setLeftVisible();
        }
        else if (_input->getHorizontal() < 0)
        {
            _uiScene.setRightVisible();
        }
        else
        {
            _uiScene.setJoystickHidden();
        }
        _uiScene.setJoystickPosition(Vec2 (_uiScene.screenToWorldCoords(_input->getJoystick()).x, SCENE_HEIGHT - (_uiScene.screenToWorldCoords(_input->getJoystick()).y)));
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
            _sound->playSound("glide");
            _movePhaseScene.getLocalPlayer()->bufferJump();
            _network->pushOutEvent(
                AnimationEvent::allocAnimationEvent(
                    _network->getShortUID(),           
                    AnimationType::GLIDE,              
                    true                               
                )
            );
        }
    }
    else if (!_input->isRightDown()) {
        if (_movePhaseScene.getLocalPlayer()->getJumpHold()) {
            _movePhaseScene.getLocalPlayer()->setJumpHold(false);
        }
        _movePhaseScene.getLocalPlayer()->setGlide(false);
    }
    //_movePhaseScene.getLocalPlayer()->setGlide(_uiScene.getDidGlide());
    _movePhaseScene.getLocalPlayer()->setMovement(_input->getHorizontal() * _movePhaseScene.getLocalPlayer()->getForce());
    _movePhaseScene.getLocalPlayer()->setJumping(_uiScene.getDidJump());
    _movePhaseScene.getLocalPlayer()->applyForce();
    if (_uiScene.getDidGiveUp()) {
        killPlayer();
    }


    if (_movePhaseScene.getLocalPlayer()->isJumping() && _movePhaseScene.getLocalPlayer()->isGrounded())
    {
        _sound->playSound("jump");
    }
    for (auto it = _world->getObstacles().begin(); it != _world->getObstacles().end(); ++it) {
        if (auto wind_cast = std::dynamic_pointer_cast<WindObstacle>(*it)) {
            windUpdate(wind_cast, dt);
        }
    }

    // TODO: Segment into uiUpdate method
    if (_movePhaseScene.getLocalPlayer()->isGrounded() && !_uiScene.isGlideDown()){
        _uiScene.setJumpButtonActive();
        _uiScene.setDidGlide(false);
    }
    else if (!_movePhaseScene.getLocalPlayer()->isGrounded() && !_uiScene.isJumpDown()){
        _uiScene.setGlideButtonActive();
        _uiScene.setDidJump(false);
    }
    if (_uiScene.getGiveUpCountdown() == 0 && !_movePhaseScene.getLocalPlayer()->isDead()) {
        _uiScene.setGiveUpButtonActive(true);
    } else {
        _uiScene.setGiveUpCountdown(_uiScene.getGiveUpCountdown()-1);
    }

    

    updateProgressBar(_movePhaseScene.getLocalPlayer());

    // TODO: Segment into progressBarUpdate method
    std::vector<std::shared_ptr<PlayerModel>> playerList = _networkController->getPlayerList();
    for (auto& player : playerList){
        if (player->getName() != _movePhaseScene.getLocalPlayer()->getName()){
            updateProgressBar(player);
        }
    }
    

    // TODO: Segment into updateCamera method
    if (_movePhaseScene.getLocalPlayer()->getPosition().x >= 0 && _movePhaseScene.getLocalPlayer()->getPosition().x <= _movePhaseScene.getGoalDoor()->getPosition().x){ getCamera()->setPosition(Vec3(getCamera()->getPosition().x + (7 * dt) *
                                                                   (_movePhaseScene.getLocalPlayer()->getPosition().x *
                                                                    56 + SCENE_WIDTH / 3.0f -
                                                                    getCamera()->getPosition().x),
        getCamera()->getPosition().y + (4 * dt) *
        (_movePhaseScene.getLocalPlayer()->getPosition().y *
            40 + SCENE_HEIGHT / 4.0 -
            getCamera()->getPosition().y), 0));
    }
    _movePhaseScene.preUpdate(dt);
    
    // TODO: This code should be handled in Mushroom class, why is it here?
    if (_mushroomCooldown > 0) {
        _mushroomCooldown--;
    }
}

void MovePhaseController::windUpdate(std::shared_ptr<WindObstacle> wind, float dt) {
    if (wind->getPlayerHits() > 0) {
        _movePhaseScene.getLocalPlayer()->addWind(wind->getWindForce(), wind->getPlayerToWindDist());
    }

    int i = 0;
    std::vector<cugl::Vec2> lst = wind->getRayOrigins();

    for (auto it = lst.begin(); it != lst.end(); ++it) {
        Vec2 rayEnd = *it + (wind->getWindDirection());

        /**Generates the appropriate callback function for this wind object*/
        
        auto callback = [this, wind, i](b2Fixture* f, Vec2 point, Vec2 normal, float fraction) {
            b2Body* body = f->GetBody();
            physics2::Obstacle* bd = reinterpret_cast<physics2::Obstacle*>(body->GetUserData().pointer);

            // Set grounded for all non-local players
            string fixtureName = wind->ReportFixture(f, point, normal, fraction);
            if ( bd == _movePhaseScene.getLocalPlayer().get()) {
                wind->setPlayerDist(i, fraction);
                return wind->getRayDist(i);
            }
            if (bd->getName() != "fan") {
                wind->setRayDist(i, fraction);
            }
            
            return fraction;
            };
        /**Generates the appropriate raycasts to handle collision for this wind object*/

        _world->rayCast(callback, *it, rayEnd);
        ++i;

    }
    //wind->update(dt);
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void MovePhaseController::postUpdate(float remain) {
    // Record failure if necessary.
    if (_movePhaseScene.getLocalPlayer()->getY() < 0)
    {
        // Hide player
        _networkController->getLocalPlayer()->setVisible(false);

        killPlayer();
    }

    // TODO: Set up overall win and lose logic
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
 Kills player for the round.
 */
void MovePhaseController::killPlayer(){
    std::shared_ptr<PlayerModel> player = _movePhaseScene.getLocalPlayer();
    // Send message to network that the player has ended their movement phase
    if (!player->isDead()){
        _sound->playSound("ow");
        // If player had treasure, remove from their possession
        if (player->hasTreasure){
            player->removeTreasure();
            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::TREASURE_LOST));
        }
        // Signal that the round is over for the player
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::MOVEMENT_END));
        _network->pushOutEvent(
            AnimationEvent::allocAnimationEvent(
                _network->getShortUID(),           
                AnimationType::DEATH,              
                true                               
            )
        );
        _networkController->getScoreController()->sendScoreEvent(
            _networkController->getNetwork(),
            _networkController->getNetwork()->getShortUID(),
            ScoreEvent::ScoreType::DEAD,
            _currRound
        );
        
        player->setDead(true);
        player->setGhost(player->getSceneNode(), true);
    }
    
}

/**
 Logic for when player reaches the goal
 */
void MovePhaseController::reachedGoal(){
    _reachedGoal = true;
    if (_reachedGoal && !_animateGoal) {
        (dynamic_pointer_cast<GoalDoor>(_movePhaseScene.getGoalDoor()))->setAnimating(true);
    }
    std::shared_ptr<PlayerModel> player = _movePhaseScene.getLocalPlayer();
    if (!player->getImmobile()){
        player->setImmobile(true);
        // Send message to network that the player has ended their movement phase
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::MOVEMENT_END));
        if (player->hasTreasure){
            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::MAKE_UNSTEALABLE));
            _networkController->getScoreController()->sendScoreEvent(
                _networkController->getNetwork(),
                _networkController->getNetwork()->getShortUID(),
                ScoreEvent::ScoreType::END_TREASURE,
                _currRound
            );
        } else {
            _networkController->getScoreController()->sendScoreEvent(
                _networkController->getNetwork(),
                _networkController->getNetwork()->getShortUID(),
                ScoreEvent::ScoreType::END,
                _currRound
            );
        }
        
    }
    
}

/**
 * Processes the change between modes (movement and building mode).
 *
 * @param value whether the level is in building mode.
 */
void MovePhaseController::processModeChange(bool value) {
    _reachedGoal = false;
    _animateGoal = false;
    if (value) {
        (dynamic_pointer_cast<GoalDoor>(_movePhaseScene.getGoalDoor()))->setResetting(true);
        _networkController->getLocalPlayer()->setVisible(false);
    }
    
    _movePhaseScene.resetCameraPos();
    _uiScene.disableUI(value);


}

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
//    if (reachedGoal){
//        if(_movePhaseScene.getLocalPlayer()->hasTreasure){
//            _movePhaseScene.getLocalPlayer()->removeTreasure();
//            // Increment total treasure collected
//            _currGems += 1;
//            // Update score image
//            _uiScene.setScoreImageFull(_currGems - 1);
//
//            // Check if player won
//            if (_currGems == TOTAL_GEMS){
//                setComplete(true);
//                return;
//            }
//            else{
//                // Set up next treasure if collected in prev round
//                _movePhaseScene.setNextTreasure(_currGems);
//            }
//
//        }
//    }
//
//    // Check if player lost
//    if (_currRound == TOTAL_ROUNDS && _currGems != TOTAL_GEMS){
//        setFailure(true);
//        return;
//    }
//
//    // Increment round
//    _currRound += 1;
//    // Update text
//    _uiScene.updateRound(_currRound, TOTAL_ROUNDS);
//
//    setFailure(false);
//
//    // Reset player properties
//    _movePhaseScene.resetPlayerProperties();
//    _died = false;
//    _reachedGoal = false;
//
//    // Reset growing wall
////    _growingWallWidth = 0.1f;
////    _growingWallNode->setVisible(false);

}

#pragma mark -
#pragma mark Collision Handling

//Collision filtering method-Right exists for pass thorugh platforms exclusively
void MovePhaseController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {

    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    contact->GetChildIndexA();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    Platform* plat = nullptr;

    Object* obj1 = reinterpret_cast<Object*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Object* obj2 = reinterpret_cast<Object*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    bool isWind1 = dynamic_cast<WindObstacle*>(obj1) != nullptr;
    bool isWind2 = dynamic_cast<WindObstacle*>(obj2) != nullptr;
    // If one of the objects is wind, prevent collision (stops player from colliding with fan WITHOUT wind being a sensor)
    if (isWind1 || isWind2) {
        contact->SetEnabled(false);
    }

    bool isMushroom1 = dynamic_cast<Mushroom*>(obj1) != nullptr;
    bool isMushroom2 = dynamic_cast<Mushroom*>(obj2) != nullptr;
    // Disables player - mushroom collision (makes it passthrough)
    if (isMushroom1 || isMushroom2) {
        contact->SetEnabled(false);
    }

    if (tagContainsPlayer(bd1->getName()) && bd1 == _movePhaseScene.getLocalPlayer().get()) {        
        if (bd2->getName() == "platform" || bd2->getName() == "movingPlatform") {
            plat = dynamic_cast<Platform*>(bd2);
        }
    }

    else if (tagContainsPlayer(bd2->getName()) && bd2 == _movePhaseScene.getLocalPlayer().get()) {
        if (bd1->getName() == "platform" || bd1->getName() == "movingPlatform") {
            plat = dynamic_cast<Platform*>(bd1);
        }
    }
    if (plat != nullptr) {
        contact->SetEnabled(false);
        _movePhaseScene.getLocalPlayer()->setGrounded(false);
        if (_movePhaseScene.getLocalPlayer()->getLinearVelocity().y <= 0.4f) {
            if (_movePhaseScene.getLocalPlayer()->getPrevFeetHeight() >= plat->getPlatformTop()) {
                contact->SetEnabled(true);
                _movePhaseScene.getLocalPlayer()->setGrounded(true);
            }
            else {
                _movePhaseScene.getLocalPlayer()->setGrounded(true);
            }
        }
        else {
            CULog("FAIL11");
        }
    }
}

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

    Object* obj1 = reinterpret_cast<Object*>(body1->GetUserData().pointer);
    Object* obj2 = reinterpret_cast<Object*>(body2->GetUserData().pointer);


    // Handle bomb object explosion
    if (obj1->getName() == "bomb" || obj2->getName() == "bomb") {
        Bomb* bomb = nullptr;
        Object* other = nullptr;

        if (obj1->getName() == "bomb") {
            bomb = dynamic_cast<Bomb*>(obj1);
            other = obj2;
        } else {
            bomb = dynamic_cast<Bomb*>(obj2);
            other = obj1;
        }


        if (bomb && other && !other->isRemoved() && other->getName() != "goalDoor" && other->getName() != "treasure") {
            CULog("Trigger bomb explosion");
            _sound->playSound("bomb");
            other->markRemoved(true);
            other->dispose();
            
        }
    }

    // Set grounded for all non-local players
    if (tagContainsPlayer(bd1->getName()) && bd1 != _movePhaseScene.getLocalPlayer().get()) {
        PlayerModel* player = dynamic_cast<PlayerModel*>(bd1);
        if (player && player->getSensorName()) {
            if (fd1 && *(player->getSensorName()) == *fd1) {
                _playerSensorFixtures[player].emplace(fix2);
            }
            if (fd2 && *(player->getSensorName()) == *fd2) {
                _playerSensorFixtures[player].emplace(fix1);
            }
            if (!_playerSensorFixtures[player].empty()) {
                player->setGrounded(true);
            }
        }
    }

    if (tagContainsPlayer(bd2->getName()) && bd2 != _movePhaseScene.getLocalPlayer().get()) {
        PlayerModel* player = dynamic_cast<PlayerModel*>(bd2);
        if (player && player->getSensorName()) {
            if (fd1 && *(player->getSensorName()) == *fd1) {
                _playerSensorFixtures[player].emplace(fix2);
            }
            if (fd2 && *(player->getSensorName()) == *fd2) {
                _playerSensorFixtures[player].emplace(fix1);
            }
            if (!_playerSensorFixtures[player].empty()) {
                player->setGrounded(true);
            }
        }
    }
    //Handles all Player Collisions in this section
        if (bd1 == _movePhaseScene.getLocalPlayer().get() || bd2 == _movePhaseScene.getLocalPlayer().get()) {
            //MANAGE COLLISIONS FOR NON-GROUNDED OBJECTS IN THIS SECTION
            // If we hit the "win" door, we are done
            if (bd2 == _movePhaseScene.getGoalDoor().get() || bd1 == _movePhaseScene.getGoalDoor().get()){
                _animateGoal = _reachedGoal;
                bool anim = _animateGoal;
                reachedGoal();
            }

            // If we hit a spike, we are DEAD
            else if (bd2->getName() == "spike" ||bd1->getName() == "spike"  ){
                killPlayer();
            }

            // If we hit a thorn, we are DEAD
            else if (bd2->getName() == "thorn" ||bd1->getName() == "thorn"  ){
                killPlayer();
            }

            //Treasure Collection
            else if (bd2->getName() == "treasure" ||bd1->getName() == "treasure")
            {
                CULog("Treasure collision");
                std::shared_ptr<PlayerModel> localPlayer = _movePhaseScene.getLocalPlayer();
                if (!localPlayer->hasTreasure && !localPlayer->isDead())
                {
                    CULog("Local player does not have treasure");
                    // Check if the treasure is stealable
                    if (_networkController->getTreasure()->isStealable()){
                        CULog("treasure is stealable");
                        // If the treasure is taken, release from player who has it
                        if (_networkController->getTreasure()->isTaken()){
                            CULog("Someone has the treasure");
                            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::TREASURE_STOLEN));
                        }
                        
                        // Local player takes treasure
                        CULog("Local Player takes treasure");
                        _sound->playSound("heehee");
                        _network->pushOutEvent(TreasureEvent::allocTreasureEvent(_network->getShortUID()));
                        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::TREASURE_TAKEN));
//                        CULog("Local Player takes treasure");
//                        _movePhaseScene.getLocalPlayer()->gainTreasure(_movePhaseScene.getTreasure());
                    }
                }
            }
            //MANAGE COLLISIONS FOR GROUNDED OBJECTS IN THIS SECTION
            else if (((_movePhaseScene.getLocalPlayer()->getSensorName() == fd2 && !tagContainsPlayer(bd1->getName())) ||
                (_movePhaseScene.getLocalPlayer()->getSensorName() == fd1 && !tagContainsPlayer(bd2->getName()))) && 
                (bd2->getName() != "fan" && bd1->getName() != "fan")
                ) {
                //Set player to grounded
                _movePhaseScene.getLocalPlayer()->setGrounded(true);
                CULog("LOCAL: GROUNDED TRUE");
                // Could have more than one ground
                _localSensorFixtures.emplace(_movePhaseScene.getLocalPlayer().get() == bd1 ? fix2 : fix1);

                //bounce if we hit a mushroom
                if (bd2->getName() == "mushroom" || bd1->getName() == "mushroom") {
                    if (_mushroomCooldown == 0) {
                        _sound->playSound("mushroom_boing");

                        b2Body* playerBody = _movePhaseScene.getLocalPlayer()->getBody();
                        b2Vec2 newVelocity = playerBody->GetLinearVelocity();
                        newVelocity.y = 15.0f;
                        playerBody->SetLinearVelocity(newVelocity);

                        _mushroomCooldown = 10;
                        CULog("Mushroom bounce triggered; cooldown set to 10 frames.");
                        physics2::Obstacle* obs = (bd2->getName()=="mushroom" ? bd2 : bd1);
                        if (auto mush = dynamic_cast<Mushroom*>(obs)) {
                            mush->triggerAnimation();
                            auto bounceEvent = std::make_shared<MushroomBounceEvent>();
                            bounceEvent = MushroomBounceEvent::allocMushroomBounceEvent(obs->getPosition());
                            CULog("sending event");
                            _network->pushOutEvent(bounceEvent);
                        }
                    }
                }

                //Handles Moving Platforms
                if ((bd2->getName() == "movingPlatform" || bd1->getName() == "movingPlatform") && _movePhaseScene.getLocalPlayer()->isGrounded())
                {
                    _movePhaseScene.getLocalPlayer()->setOnMovingPlat(true);
                    _movePhaseScene.getLocalPlayer()->setMovingPlat(bd1 == _movePhaseScene.getLocalPlayer().get() ? bd2 : bd1);
                }
            }
        }
    
}

void MovePhaseController::setGoalDoorAnimation(std::shared_ptr<scene2::SpriteNode> sprite) {
    _spinSpriteNode = sprite;

    _movePhaseScene.getGoalDoor()->setSceneNode(_spinSpriteNode);
    _spinSpriteNode->setVisible(true);
    //    _node->setScale(0.065f);

    _goalDoorTimeline = ActionTimeline::alloc();

    // Create the frame sequence
    // For an 8x8 spritesheet
    const int span = 32;

    std::vector<int> forward;
    for (int ii = 1; ii < span; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animation
    _spinAnimateSprite = AnimateSprite::alloc(forward);
    _goalDoorAction = _spinAnimateSprite->attach<scene2::SpriteNode>(_spinSpriteNode);
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

    // Set Grounded false for local player
    if ((_movePhaseScene.getLocalPlayer()->getSensorName() == fd2 && _movePhaseScene.getLocalPlayer().get() != bd1) ||
        (_movePhaseScene.getLocalPlayer()->getSensorName() == fd1 && _movePhaseScene.getLocalPlayer().get() != bd2))
    {
        _localSensorFixtures.erase(_movePhaseScene.getLocalPlayer().get() == bd1 ? fix2 : fix1);
        if (_localSensorFixtures.empty())
        {
            _movePhaseScene.getLocalPlayer()->setGrounded(false);
        }
    }
    
    // Set Grounded false for non-local players
    if (tagContainsPlayer(bd1->getName()) && bd1 != _movePhaseScene.getLocalPlayer().get()) {
        PlayerModel* player = dynamic_cast<PlayerModel*>(bd1);
        if (player && player->getSensorName()) {
            if (fd1 && *(player->getSensorName()) == *fd1) {
                _playerSensorFixtures[player].erase(fix2);
            }
            if (fd2 && *(player->getSensorName()) == *fd2) {
                _playerSensorFixtures[player].erase(fix1);
            }
            if (_playerSensorFixtures[player].empty()) {
                player->setGrounded(false);
            }
        }
    }

    if (tagContainsPlayer(bd2->getName()) && bd2 != _movePhaseScene.getLocalPlayer().get()) {
        PlayerModel* player = dynamic_cast<PlayerModel*>(bd2);
        if (player && player->getSensorName()) {
            if (fd1 && *(player->getSensorName()) == *fd1) {
                _playerSensorFixtures[player].erase(fix2);
            }
            if (fd2 && *(player->getSensorName()) == *fd2) {
                _playerSensorFixtures[player].erase(fix1);
            }
            if (_playerSensorFixtures[player].empty()) {
                player->setGrounded(false);
            }
        }
    }

    if ((bd1 == _movePhaseScene.getLocalPlayer().get() && bd2->getName() == "movingPlatform") ||
        (bd2 == _movePhaseScene.getLocalPlayer().get() && bd1->getName() == "movingPlatform"))
    {
        _movePhaseScene.getLocalPlayer()->setOnMovingPlat(false);
        _movePhaseScene.getLocalPlayer()->setMovingPlat(nullptr);
    }
}

void MovePhaseController::updateProgressBar(std::shared_ptr<PlayerModel> player) {
    string playerTag = player->getName();
    if (!player->isVisible() && !player->isDead()) {
        player->setVisible(true);
    }
    if (player->isDead()){
        _uiScene.removePlayerIcon(playerTag);
    }

    float player_pos = player->getPosition().x;
    if (player_pos < _playerStart){
        _uiScene.setPlayerIcon(0, _levelWidth, playerTag);
    }
    else if (player_pos > _levelWidth){
        _uiScene.setPlayerIcon(_levelWidth, _levelWidth, playerTag);
    }
    else{
        _uiScene.setPlayerIcon(player_pos - _playerStart, _levelWidth, playerTag);
    }

    if (player->hasTreasure){
        _uiScene.setTreasureIcon(true, playerTag);
    }
    else{
        _uiScene.setTreasureIcon(false, playerTag);
    }
}
