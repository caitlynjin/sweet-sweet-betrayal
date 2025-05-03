//
//  NetworkController.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 3/13/25.
//

#include <stdio.h>
#include "NetworkController.h"
#include "Constants.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;
using namespace cugl::netcode;
using namespace cugl::graphics;
using namespace cugl::physics2;
using namespace cugl::physics2::distrib;
using namespace cugl::audio;
using namespace Constants;

NetworkController::NetworkController() {

}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool NetworkController::init(const std::shared_ptr<AssetManager>& assets)
{
    _assets = assets;
    
    _network = cugl::physics2::distrib::NetEventController::alloc(_assets);
    _network->attachEventType<MessageEvent>();
    _network->attachEventType<ColorEvent>();
    _network->attachEventType<LevelEvent>();
    _network->attachEventType<ReadyEvent>();
    _network->attachEventType<ScoreEvent>();
    _network->attachEventType<TreasureEvent>();
    _network->attachEventType<AnimationEvent>();
    _localID = _network->getShortUID();
    _scoreController = ScoreController::alloc(_assets);
    
    // TODO: Create player-id hashmap
    
    
    return true;
}

void NetworkController::dispose(){
//    if (_active)
//    {
//        //TODO: Dispose variables
//        _readyButton = nullptr;
//        _rightButton = nullptr;
//        _leftButton = nullptr;
//        Scene2::dispose();
//    }
}

void NetworkController::resetNetwork(){
    _network->disconnect();
    _network->dispose();
    _network = cugl::physics2::distrib::NetEventController::alloc(_assets);
    _network->attachEventType<MessageEvent>();
    _network->attachEventType<ColorEvent>();
    _network->attachEventType<LevelEvent>();
    _network->attachEventType<ReadyEvent>();
    _network->attachEventType<ScoreEvent>();
    _network->attachEventType<TreasureEvent>();
    _localID = _network->getShortUID();
}

/** Flushes the connection and clears all events */
void NetworkController::flushConnection(){
    while (_network->isInAvailable()) {
       _network->popInEvent();
    }
}

/**
     * The method called to update the game mode.
     *
     * This is the nondeterministic version of a physics simulation. It is
     * provided for comparison purposes only.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
void NetworkController::update(float timestep){


}


/**
 * The method called to indicate the start of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the beginning of the core application loop,
 * which is concluded with a call to {@link #postUpdate}.
 *
 * This method should be used to process any events that happen early in
 * the application loop, such as user input or events created by the
 * {@link schedule} method. In particular, no new user input will be
 * recorded between the time this method is called and {@link #postUpdate}
 * is invoked.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #postUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void NetworkController::preUpdate(float dt){
    // We need to create collision filters for players in the world --> not automatically handled by the network
    // Should only be done one time --> once all players have joined the world
    if (!_filtersSet){
        trySetFilters();
    }
    
    _scoreController->preUpdate(dt);
    
}

/**
 * The method called to provide a deterministic application loop.
 *
 * This method provides an application loop that runs at a guaranteed fixed
 * timestep. This method is (logically) invoked every {@link getFixedStep}
 * microseconds. By that we mean if the method {@link draw} is called at
 * time T, then this method is guaranteed to have been called exactly
 * floor(T/s) times this session, where s is the fixed time step.
 *
 * This method is always invoked in-between a call to {@link #preUpdate}
 * and {@link #postUpdate}. However, to guarantee determinism, it is
 * possible that this method is called multiple times between those two
 * calls. Depending on the value of {@link #getFixedStep}, it can also
 * (periodically) be called zero times, particularly if {@link #getFPS}
 * is much faster.
 *
 * As such, this method should only be used for portions of the application
 * that must be deterministic, such as the physics simulation. It should
 * not be used to process user input (as no user input is recorded between
 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
 *
 * The time passed to this method is NOT the same as the one passed to
 * {@link #preUpdate}. It will always be exactly the same value.
 *
 * @param step  The number of fixed seconds for this step
 */
void NetworkController::fixedUpdate(float step){
    if (!_localID){
        _localID = _network->getShortUID();
    }
    _scoreController->fixedUpdate(step);
    // Process messaging events
    if(_network->isInAvailable()){
        auto e = _network->popInEvent();
        // Check for MessageEvent
        if(auto mEvent = std::dynamic_pointer_cast<MessageEvent>(e)){
            processMessageEvent(mEvent);
        }
        // Check for ColorEvent
        if(auto cEvent = std::dynamic_pointer_cast<ColorEvent>(e)){
            processColorEvent(cEvent);
        }
        // Check for ReadyEvent
        if(auto rEvent = std::dynamic_pointer_cast<ReadyEvent>(e)){
            processReadyEvent(rEvent);
        }
        // Check for ScoreEvent
        if(auto sEvent = std::dynamic_pointer_cast<ScoreEvent>(e)){
            _scoreController->processScoreEvent(sEvent);
        }
        // Check for TreasureEvent
        if(auto tEvent = std::dynamic_pointer_cast<TreasureEvent>(e)){
            processTreasureEvent(tEvent);
        }
        // Check for AnimationEvent
        if(auto aEvent = std::dynamic_pointer_cast<AnimationEvent>(e)){
            processAnimationEvent(aEvent);
        }
        // Check for LevelEvent
        if(auto lEvent = std::dynamic_pointer_cast<LevelEvent>(e)){
            processLevelEvent(lEvent);
        }
    }
    _scoreController->setPlayerColors(_playerColorsById);

}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the end of the core application loop, which was
 * begun with a call to {@link #preUpdate}.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * This method should not be used to process user input, as no new input
 * will have been recorded since {@link #preUpdate} was called.
 *
 * Note that the time passed as a parameter is the time measured from the
 * last call to {@link #fixedUpdate}. That is because this method is used
 * to interpolate object position for animation.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void NetworkController::postUpdate(float remain){
    _scoreController->postUpdate(remain);
}


/**
 * Resets the status of the game so that we can play again.
 */
void NetworkController::reset(){
    // Reset score controller
    _scoreController->reset();
    
    // Reset network in-game variables
    _numReady = 0;
    _network->pushOutEvent(ReadyEvent::allocReadyEvent(_network->getShortUID(), _color, false));
    _numReset = 0;
    resetTreasureRandom();
    _readyMessageSent = false;
    _filtersSet = false;
    _resetLevel = false;
    
    _levelSelected = 0;
}


/**
 * Resets logic necessary to start another round
 */
void NetworkController::resetRound(){
    _numReady = 0;
    _numReset = 0;
}


#pragma mark -
#pragma mark Process Network Events
/**
 * This method takes a MessageEvent and processes it.
 */
void NetworkController::processMessageEvent(const std::shared_ptr<MessageEvent>& event){
    Message message = event->getMesage();
    switch (message) {
        case Message::COLOR_READY:
            _numColorReady++;
            break;
        case Message::BUILD_READY:
            // Increment number of players ready
            _numReady++;
            _network->pushOutEvent(ReadyEvent::allocReadyEvent(_network->getShortUID(), _color, true));
            break;
        
        case Message::MOVEMENT_END:
            // Increment number of players needed to be reset
            _numReset++;
            break;
            
        case Message::TREASURE_TAKEN:
            // Set the treasure as taken
            _treasure->setTaken(true);
            CULog("Treasure taken processed");
            break;
        case Message::TREASURE_LOST:
            // Reset treasure
            resetTreasure();
            break;
        case Message::TREASURE_STOLEN:
            // Remove treasure from all player references
            for (auto player : _playerList){
                if (player->hasTreasure){
                    player->removeTreasure();
                    _treasure->setTaken(false);
                }
            }
            break;
        case Message::TREASURE_WON:
            // Reset treasure
            resetTreasureRandom();
            break;
        case Message::MAKE_UNSTEALABLE:
            // Make treasure unstealable
            _treasure->setStealable(false);
            break;
        case Message::HOST_START:
            break;
            // Still need this?
        case Message::SCORE_UPDATE:
//            _network
            break;
        case Message::RESET_LEVEL:
            CULog("Reset received");
            _resetLevel = true;
            break;
        case Message::HOST_PICK:
            _levelSelected = 1;
        default:
            // Handle unknown message types
            std::cout << "Unknown message type received" << std::endl;
            break;
    }
}

/**
 * This method takes a ColorEvent and processes it.
 */
void NetworkController::processColorEvent(const std::shared_ptr<ColorEvent>& event){
    int playerID = event->getPlayerID();
    ColorType color = event->getColor();
    int prevColorInt = event->getPrevColor();
    
    // Store each color into map by player id
    _playerColorsById[playerID] = color;
    _playerIDs.push_back(playerID);
    
    if (_onColorTaken && playerID != _localID) {
        _onColorTaken(color, prevColorInt);
    }
}

/**
 * This method takes a LevelEvent and processes it.
 */
void NetworkController::processLevelEvent(const std::shared_ptr<LevelEvent>& event){
    _levelSelected = event->getLevelNum();
}
/**
 * This method takes a ReadyEvent and processes it.
 */
void NetworkController::processReadyEvent(const std::shared_ptr<ReadyEvent>& event){
    int playerID = event->getPlayerID();
    ColorType color = event->getColor();
    bool ready = event->getReady();

    for (auto player : _playerList){
        if (player->getColor() == color){
            player->setReady(ready);
        }
    }
}


/**
 * This method takes a TreasureEvent and processes it.
 */
void NetworkController::processTreasureEvent(const std::shared_ptr<TreasureEvent>& event){
    int playerID = event->getPlayerID();
    
    ColorType color = _playerColorsById[playerID];
    
    if (_color == color){
        _localPlayer->gainTreasure(_treasure);
    }
}

/**
 * This method takes a AnimationEvent and processes it.
 */
void NetworkController::processAnimationEvent(const std::shared_ptr<AnimationEvent>& event) {
    int uid        = event->getPlayerID();
    AnimationType anim   = event->getAnimation();
    bool activate = event->isActivate();
    int colorInt  = static_cast<int>(_playerColorsById[uid]);

//    CULog("AnimationEvent → UID=%d, color=%d, anim=%d, activate=%d",
//          uid, colorInt, static_cast<int>(anim), activate);

    static const char* ColorNames[] = {"Red","Blue","Green","Yellow"};
    std::string targetName = "player" + std::string(ColorNames[colorInt]);

    for (auto player : _playerList) {
        if (player->getName() == targetName) {
            player->processNetworkAnimation(anim, activate);
        }
    }
}




/** Resets the treasure to remove possession and return to spawn location */
void NetworkController::resetTreasure(){
    _treasure->setTaken(false);
    _treasure->setStealable(true);
    if (_isHost){
        _treasure->setPositionInit(_treasureSpawn);
    }
    
}


/** Resets the treasure to remove possession and return to random spawn location */
void NetworkController::resetTreasureRandom(){
    _treasure->setTaken(false);
    _treasure->setStealable(true);
    if (_isHost){
        _treasure->setPositionInit(pickRandSpawn());
    }
    
}

#pragma mark -
#pragma mark Networked Object Creation
/**
 * Creates a networked platform.
 *
 * @return the platform being created
 *
 * @param The platform being created (that has not yet been added to the physics world).
 */
std::shared_ptr<Object> NetworkController::createPlatformNetworked(Vec2 pos, Size size, string jsonType, float scale){
    
    //Use Platform Factory to create the platform boxObstacle and sprite
    auto params = _platFact->serializeParams(pos, size, jsonType, scale);
    // pair holds the boxObstacle and sprite to be used for the platform
    // Already added to _world after this call
    auto pair = _network->getPhysController()->addSharedObstacle(_platFactId, params);
    std::shared_ptr<Platform> plat = std::dynamic_pointer_cast<Platform>(pair.first);
    _objects->push_back(plat);
    return plat;
    
}
/**
 * Creates a networked moving platform.
 *
 * @return the moving platform being created
 */
std::shared_ptr<Object> NetworkController::createMovingPlatformNetworked(Vec2 pos, Size size, Vec2 end, float speed, float scale) {
    
    auto params = _movingPlatFact->serializeParams(pos, size, end, speed, scale);

    auto pair = _network->getPhysController()->addSharedObstacle(_movingPlatFactID, params);
    std::shared_ptr<Platform> plat = std::dynamic_pointer_cast<Platform>(pair.first);
    _objects->push_back(plat);
    return plat;
}

std::shared_ptr<Object> NetworkController::createTreasureNetworked(Vec2 pos, Size size, float scale, bool taken) {
    auto params = _treasureFact->serializeParams(pos, size, scale, taken);
    auto pair = _network->getPhysController()->addSharedObstacle(_treasureFactID, params);
    std::shared_ptr<Treasure> treasure = std::dynamic_pointer_cast<Treasure>(pair.first);
    _objects->push_back(treasure);
    return treasure;
}


std::shared_ptr<Object> NetworkController::createMushroomNetworked(Vec2 pos, Size size, float scale) {
    auto params = _mushroomFact->serializeParams(pos, size, scale);
    auto pair = _network->getPhysController()->addSharedObstacle(_mushroomFactID, params);
    std::shared_ptr<Mushroom> mushroom = std::dynamic_pointer_cast<Mushroom>(pair.first);
    _objects->push_back(mushroom);
    return mushroom;
}

std::shared_ptr<Object> NetworkController::createThornNetworked(Vec2 pos, Size size) {
    auto params = _thornFact->serializeParams(pos, size);
    auto pair = _network->getPhysController()->addSharedObstacle(_thornFactID, params);
    std::shared_ptr<Thorn> thorn = std::dynamic_pointer_cast<Thorn>(pair.first);
    _objects->push_back(thorn);
    return thorn;
}


std::shared_ptr<Object> NetworkController::createWindNetworked(Vec2 pos, Size size, float scale, Vec2 dir, Vec2 str) {
    auto params = _windFact->serializeParams(pos, size, scale,  dir, str);
    auto pair = _network->getPhysController()->addSharedObstacle(_windFactID, params);
    std::shared_ptr<WindObstacle> wind = std::dynamic_pointer_cast<WindObstacle>(pair.first);

    _objects->push_back(wind);
    return wind;
}



/**
 * Creates a networked player.
 *
 * @return the player being created
 *
 * @param The player being created (that has not yet been added to the physics world).
 */
std::shared_ptr<PlayerModel> NetworkController::createPlayerNetworked(Vec2 pos, float scale, ColorType color){
    CULog("CREATE PLAYER NETWORKED, COLOR: %d");
    auto params = _dudeFact->serializeParams(pos, scale, color);
    auto localPair = _network->getPhysController()->addSharedObstacle(_dudeFactID, params);
    return std::dynamic_pointer_cast<PlayerModel>(localPair.first);
}

#pragma mark -
#pragma mark Treasure Handling

/**
 Picks the next spawn point for the treasure at random.
 
 If a spawn point has been used already, it should be chosen again until all other spawn points have been used.
 */
Vec2 NetworkController::pickRandSpawn(){
    int maxIndex = static_cast<int>(_tSpawnPoints.size()) - 1;
    Vec2 spawnPoint;
    
    // Random number generator
    std::shared_ptr<cugl::Random> random = cugl::Random::alloc();
    int randomIndex;
    
    if (maxIndex > 0){
        bool foundSpawn = false;
        while (!foundSpawn){
            randomIndex = static_cast<int>(random->getClosedSint64(0, maxIndex));
            Vec2 randSpawn = _tSpawnPoints[randomIndex];
            
            // Check if random spawn has already been used recently
            if (std::count(_usedSpawns.begin(), _usedSpawns.end(), randSpawn) == 0){
                spawnPoint = randSpawn;
                foundSpawn = true;
            }
        }
    }
    else{
        spawnPoint = _tSpawnPoints[0];
    }
    
    // Add spawn point to used spawn list
    _usedSpawns.push_back(spawnPoint);
    
    // Check if we can clear _usedSpawns
    if (_usedSpawns.size() == _tSpawnPoints.size()){
        _usedSpawns.clear();
    }
    
    _treasureSpawn = spawnPoint;
    return spawnPoint;
}

#pragma mark -
#pragma mark Helpers

void NetworkController::setWorld(std::shared_ptr<cugl::physics2::distrib::NetWorld> world){
    _world = world;
    
    // Setup factories
    _platFact = PlatformFactory::alloc(_assets);
    _platFactId = _network->getPhysController()->attachFactory(_platFact);

    _dudeFact = DudeFactory::alloc(_assets);
    _dudeFactID = _network->getPhysController()->attachFactory(_dudeFact);
    
    _movingPlatFact = MovingPlatFactory::alloc(_assets);
    _movingPlatFactID = _network->getPhysController()->attachFactory(_movingPlatFact);
    // Setup Treasure Factory
    _treasureFact = TreasureFactory::alloc(_assets);
    _treasureFactID = _network->getPhysController()->attachFactory(_treasureFact);

    _mushroomFact = MushroomFactory::alloc(_assets);
    _mushroomFactID = _network->getPhysController()->attachFactory(_mushroomFact);

    _thornFact = ThornFactory::alloc(_assets);
    _thornFactID = _network->getPhysController()->attachFactory(_thornFact);
    
    _windFact = WindFactory::alloc(_assets);
    _windFactID = _network->getPhysController()->attachFactory(_windFact);
}

/**
 * This method attempts to set all the collision filters for the networked players.
 *
 * All filters should be set once the world contains the amount of connected players to avoid possible race condition.
 */
void NetworkController::trySetFilters(){
    // First, count how many players are in the world
    // We only want to set filters once all players are represented in the world
    int numPlayers = 0;

    const auto& obstacles = _world->getObstacles();
    std::vector<std::shared_ptr<PlayerModel>> playerListTemp;
    
    for (const auto& obstacle : obstacles) {
        
        if (tagContainsPlayer(obstacle->getName())){
            numPlayers += 1;
            
            // Try to cast to PlayerModel and add to our list if successful
            auto playerModel = std::dynamic_pointer_cast<PlayerModel>(obstacle);
            if (playerModel) {
                playerListTemp.push_back(playerModel);
                
            } else {
                CULog("Found player but casting failed");
            }
        }
    }
    
    // Check if we have all players in world, then set their collision filters
    if (numPlayers == _network->getNumPlayers()){
        _playerList = playerListTemp;
        
        // Loop through each obstacle
        for (auto& player : _playerList) {
            player->setFilterData();
        }
        for (auto& player : _playerList) {
            player->setEnabled(true);
        }
        
        _filtersSet = true;
    }
}


#pragma mark -
#pragma mark Factories



#pragma mark -
#pragma mark Dude Factory

/**
 * Generate a pair of Obstacle and SceneNode using the given parameters
 */
std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> DudeFactory::createObstacle(Vec2 pos, float scale, ColorType color) {
    auto image = _assets->get<Texture>(PLAYER_TEXTURE);

    auto player = PlayerModel::alloc(pos, image->getSize() / scale, scale, color);
    
    player->setShared(true);
    player->setDebugColor(DEBUG_COLOR);
    
    if (color == ColorType::RED){
        auto idleSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_RED_IDLE_TEXTURE), 1, 7, 7);
        player->setIdleAnimation(idleSpriteNode, 7);

        auto walkSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_RED_WALK_TEXTURE), 1, 3, 3);
        player->setWalkAnimation(walkSpriteNode, 3);

        auto glideSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_RED_GLIDE_TEXTURE), 1, 4, 4);
        player->setGlideAnimation(glideSpriteNode, 4);

        auto jumpSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_RED_JUMP_TEXTURE), 1, 5, 5);
        player->setJumpAnimation(jumpSpriteNode, 5);

        auto deathSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_RED_DEATH_TEXTURE), 1, 4, 4);
        player->setDeathAnimation(deathSpriteNode, 4);
    }
    else if (color == ColorType::BLUE){
        auto idleSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_BLUE_IDLE_TEXTURE), 1, 7, 7);
        player->setIdleAnimation(idleSpriteNode, 7);

        auto walkSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_BLUE_WALK_TEXTURE), 1, 3, 3);
        player->setWalkAnimation(walkSpriteNode, 3);

        auto glideSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_BLUE_GLIDE_TEXTURE), 1, 4, 4);
        player->setGlideAnimation(glideSpriteNode, 4);

        auto jumpSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_BLUE_JUMP_TEXTURE), 1, 5, 5);
        player->setJumpAnimation(jumpSpriteNode, 5);

        auto deathSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_BLUE_DEATH_TEXTURE), 1, 4, 4);
        player->setDeathAnimation(deathSpriteNode, 4);
    }
    else if (color == ColorType::GREEN){
        auto idleSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_GREEN_IDLE_TEXTURE), 1, 7, 7);
        player->setIdleAnimation(idleSpriteNode, 7);

        auto walkSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_GREEN_WALK_TEXTURE), 1, 3, 3);
        player->setWalkAnimation(walkSpriteNode, 3);

        auto glideSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_GREEN_GLIDE_TEXTURE), 1, 4, 4);
        player->setGlideAnimation(glideSpriteNode, 4);

        auto jumpSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_GREEN_JUMP_TEXTURE), 1, 5, 5);
        player->setJumpAnimation(jumpSpriteNode, 5);

        auto deathSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_GREEN_DEATH_TEXTURE), 1, 4, 4);
        player->setDeathAnimation(deathSpriteNode, 4);
    }
    else if (color == ColorType::YELLOW){
        auto idleSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_YELLOW_IDLE_TEXTURE), 1, 7, 7);
        player->setIdleAnimation(idleSpriteNode, 7);

        auto walkSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_YELLOW_WALK_TEXTURE), 1, 3, 3);
        player->setWalkAnimation(walkSpriteNode, 3);

        auto glideSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_YELLOW_GLIDE_TEXTURE), 1, 4, 4);
        player->setGlideAnimation(glideSpriteNode, 4);

        auto jumpSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_YELLOW_JUMP_TEXTURE), 1, 5, 5);
        player->setJumpAnimation(jumpSpriteNode, 5);

        auto deathSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_YELLOW_DEATH_TEXTURE), 1, 4, 4);
        player->setDeathAnimation(deathSpriteNode, 4);
    }
    
    
    return std::make_pair(player, player->getSceneNode());
}

/**
 * Helper method for converting normal parameters into byte vectors used for syncing.
 */
std::shared_ptr<std::vector<std::byte>> DudeFactory::serializeParams(Vec2 pos, float scale, ColorType color) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(scale);
    _serializer.writeSint32(static_cast<int>(color));
    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

/**
 * Generate a pair of Obstacle and SceneNode using serialized parameters.
 */
std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> DudeFactory::createObstacle(const std::vector<std::byte>& params) {
    _deserializer.reset();
    _deserializer.receive(params);
    float x = _deserializer.readFloat();
    float y = _deserializer.readFloat();
    Vec2 pos = Vec2(x,y);
    float scale = _deserializer.readFloat();
    ColorType color = static_cast<ColorType>(_deserializer.readSint32());
    return createObstacle(pos, scale, color);
}



#pragma mark -
#pragma mark Platform Factory

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> PlatformFactory::createObstacle(Vec2 pos, Size size, int jsonType, float scale){
    
    std::shared_ptr<Texture> image;
    if (jsonType == static_cast<int>(JsonType::TILE)) {
        image = _assets->get<Texture>(TILE_TEXTURE);
    }
    else if (jsonType == static_cast<int>(JsonType::PLATFORM)){
        image = _assets->get<Texture>(PLATFORM_TILE_TEXTURE);
    }
    else{
        image = _assets->get<Texture>(LOG_TEXTURE);
    }

    auto plat = Platform::alloc(pos, size, LOG_TEXTURE);

    plat->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    plat->setDensity(BASIC_DENSITY);
    plat->setFriction(BASIC_FRICTION);
    plat->setRestitution(BASIC_RESTITUTION);
    plat->setDebugColor(DEBUG_COLOR);
    plat->setName("platform");
    plat->setShared(true);

    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    plat->setSceneNode(sprite);

    return std::make_pair(plat, sprite);
}

/**
 * Helper method for converting normal parameters into byte vectors used for syncing.
 */
std::shared_ptr<std::vector<std::byte>> PlatformFactory::serializeParams(Vec2 pos, Size size, string jsonType, float scale) {
    // Cast jsonType to an int for serializer
    int type;
    if (jsonType == "tile"){
        type = static_cast<int>(JsonType::TILE);
    }
    else if (jsonType == "platform"){
        type = static_cast<int>(JsonType::PLATFORM);
    }
    else{
        type = static_cast<int>(JsonType::LOG);
    }
    
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(size.width);
    _serializer.writeFloat(size.height);
    _serializer.writeSint32(type);
    _serializer.writeFloat(scale);
    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

/**
 * Generate a pair of Obstacle and SceneNode using serialized parameters.
 */
std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> PlatformFactory::createObstacle(const std::vector<std::byte>& params) {

    _deserializer.reset();
    _deserializer.receive(params);
    float x = _deserializer.readFloat();
    float y = _deserializer.readFloat();
    Vec2 pos = Vec2(x,y);
    x = _deserializer.readFloat();
    y = _deserializer.readFloat();
    Size size = Size(x,y);
    int type = _deserializer.readSint32();
    float scale = _deserializer.readFloat();
    
    return createObstacle(pos, size, type, scale);
}



#pragma mark -
#pragma mark Moving Platform Factory

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> MovingPlatFactory::createObstacle(Vec2 pos, Size size, Vec2 end, float speed, float scale) {
    std::shared_ptr<Texture> image = _assets->get<Texture>(GLIDING_LOG_TEXTURE);
    
    std::shared_ptr<Platform> movPlat = Platform::allocMoving(pos, size, pos, end, speed);
    
    movPlat->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    movPlat->setDensity(BASIC_DENSITY);
    movPlat->setFriction(BASIC_FRICTION);
    movPlat->setRestitution(BASIC_RESTITUTION);
    movPlat->setDebugColor(DEBUG_COLOR);
    movPlat->setName("movingPlatform");

    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    movPlat->setSceneNode(sprite);

    return std::make_pair(movPlat, sprite);
}


std::shared_ptr<std::vector<std::byte>> MovingPlatFactory::serializeParams(Vec2 pos, Size size, Vec2 end, float speed, float scale) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(size.width);
    _serializer.writeFloat(size.height);
    _serializer.writeFloat(end.x);
    _serializer.writeFloat(end.y);
    _serializer.writeFloat(speed);
    _serializer.writeFloat(scale);
    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> MovingPlatFactory::createObstacle(const std::vector<std::byte>& params) {
    _deserializer.reset();
    _deserializer.receive(params);
    float posx = _deserializer.readFloat();
    float posy = _deserializer.readFloat();
    Vec2 pos(posx, posy);
    float width = _deserializer.readFloat();
    float height = _deserializer.readFloat();
    Size size(width, height);
    float endx = _deserializer.readFloat();
    float endy = _deserializer.readFloat();
    Vec2 end(endx, endy);
    float speed = _deserializer.readFloat();
    float scale = _deserializer.readFloat();
    
    return createObstacle(pos, size, end, speed, scale);
}

#pragma mark -
#pragma mark Treasure Factory

/**
 * Creates a treasure obstacle and its associated scene node.
 *
 * This implementation mimics ObjectController::createTreasure but also uses the provided scale.
 *
 * @param pos The position of the treasure in Box2D coordinates.
 * @param size The size of the treasure.
 * @param scale The scale factor for the treasure.
 * @param taken Whether the treasure has been taken.
 *
 * @return A pair consisting of a physics obstacle and a scene node.
 */
std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
TreasureFactory::createObstacle(Vec2 pos, Size size, float scale, bool taken) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("treasure");
    auto treasure = Treasure::alloc(pos, image->getSize() / scale, scale);
    
    auto animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("treasure-sheet"), 1, 32, 32);
    treasure->setAnimation(animNode);

    treasure->setName("treasure");
    treasure->setDebugColor(Color4::YELLOW);
    treasure->setPositionInit(pos);
    treasure->setShared(true);
    
    return std::make_pair(treasure, treasure->getSceneNode());
}

/**
 * Serializes the parameters for a treasure.
 *
 * This method converts the provided parameters (including scale) into a byte vector
 * suitable for network transmission so that the treasure can be recreated on other clients.
 *
 * @param pos The position of the treasure.
 * @param size The size of the treasure.
 * @param jsonType The type identifier for the treasure.
 * @param scale The scale factor for the treasure.
 *
 * @return A shared pointer to a byte vector containing the serialized parameters.
 */
std::shared_ptr<std::vector<std::byte>>
TreasureFactory::serializeParams(Vec2 pos, Size size, float scale, bool taken) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(size.width);
    _serializer.writeFloat(size.height);
    _serializer.writeFloat(scale);
    _serializer.writeBool(taken);
    //TODO: serialize if we have more jsontype

    
    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

/**
 * Creates a treasure obstacle using serialized parameters.
 *
 * This method decodes the serialized parameters (including scale) and creates the corresponding
 * treasure obstacle along with its scene node.
 *
 * @param params The byte vector containing serialized parameters.
 *
 * @return A pair consisting of a physics obstacle and a scene node.
 */
std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
TreasureFactory::createObstacle(const std::vector<std::byte>& params) {
    _deserializer.reset();
    _deserializer.receive(params);
    
    float posX = _deserializer.readFloat();
    float posY = _deserializer.readFloat();
    Vec2 pos(posX, posY);
    
    float width = _deserializer.readFloat();
    float height = _deserializer.readFloat();
    Size size(width, height);
    float scale = _deserializer.readFloat();
    bool taken = _deserializer.readBool();
    
    return createObstacle(pos, size, scale, taken);
}

#pragma mark -
#pragma mark Mushroom Factory

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
MushroomFactory::createObstacle(Vec2 pos, Size size, float scale) {
    std::shared_ptr<Texture> texture = _assets->get<Texture>("mushroom");
    
    std::shared_ptr<Mushroom> mush = Mushroom::alloc(pos, size, scale);
    
    mush->setBodyType(b2_dynamicBody);
    mush->setDensity(BASIC_DENSITY);
    mush->setFriction(BASIC_FRICTION);
    mush->setRestitution(BASIC_RESTITUTION);
    mush->setName("mushroom");
    mush->setDebugColor(DEBUG_COLOR);
    mush->setShared(true);

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(texture);
    mush->setSceneNode(sprite);

    return std::make_pair(mush, sprite);
}


std::shared_ptr<std::vector<std::byte>>
MushroomFactory::serializeParams(Vec2 pos, Size size, float scale) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(size.width);
    _serializer.writeFloat(size.height);
    _serializer.writeFloat(scale);

    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
MushroomFactory::createObstacle(const std::vector<std::byte>& params) {
    _deserializer.reset();
    _deserializer.receive(params);
    
    float posX = _deserializer.readFloat();
    float posY = _deserializer.readFloat();
    Vec2 pos(posX, posY);
    
    float width  = _deserializer.readFloat();
    float height = _deserializer.readFloat();
    Size size(width, height);
    
    float scale = _deserializer.readFloat();
    
    return createObstacle(pos, size, scale);
}

#pragma mark -
#pragma mark Thorn Factory

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
ThornFactory::createObstacle(Vec2 pos, Size size) {
    std::shared_ptr<Texture> texture = _assets->get<Texture>(THORN_TEXTURE);

    std:shared_ptr<Thorn> thorn = Thorn::alloc(pos, size);
    
    thorn->setBodyType(b2_dynamicBody);
    thorn->setDensity(BASIC_DENSITY);
    thorn->setFriction(BASIC_FRICTION);
    thorn->setRestitution(BASIC_RESTITUTION);
    thorn->setName("thorn");
    thorn->setDebugColor(DEBUG_COLOR);
    thorn->setShared(true);

    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(texture, 1, 1);
    thorn->setSceneNode(sprite);

    return std::make_pair(thorn, sprite);
}


std::shared_ptr<std::vector<std::byte>>
ThornFactory::serializeParams(Vec2 pos, Size size) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(size.width);
    _serializer.writeFloat(size.height);

    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
ThornFactory::createObstacle(const std::vector<std::byte>& params) {
    _deserializer.reset();
    _deserializer.receive(params);

    float posX = _deserializer.readFloat();
    float posY = _deserializer.readFloat();
    Vec2 pos(posX, posY);

    float width  = _deserializer.readFloat();
    float height = _deserializer.readFloat();
    Size size(width, height);

    return createObstacle(pos, size);
}


#pragma mark -
#pragma mark Wind Factory

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
WindFactory::createObstacle(Vec2 pos, Size size,float scale, const Vec2 windDirection, const Vec2 windStrength) {
    //Allocate Fan Animations
    std::shared_ptr<WindObstacle> wind = WindObstacle::alloc(pos, size, scale, windDirection, windStrength);

    auto animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(FAN_TEXTURE_ANIMATED), 1, 4, 4);
    wind->setFanAnimation(animNode, 4);
    auto animNode1 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_1), 1, 14, 14);
    auto animNode2 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_2), 1, 14, 14);
    auto animNode3 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_3), 1, 14, 14);
    auto animNode4 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_4), 1, 14, 14);
    std::vector<std::shared_ptr<scene2::SpriteNode>> gusts;

    gusts.push_back(animNode1);
    gusts.push_back(animNode2);
    gusts.push_back(animNode3);
    gusts.push_back(animNode4);

    wind->setGustAnimation(gusts, 14);

    wind->setPositionInit(pos);
    wind->setEnabled(false);

    // IN ORDER TO NETWORK GUST ANIMIATIONS, MAY NEED TO ADD GUST SPRITE AS CHILD TO FANSPRITE --> TAKE A LOOK AT HOW PLAYER ANIMATIONS ARE SETUP IN DUDE FACTORY, ALL ANIMATIONS ARE CHILDREN OF A ROOT NODE

    return std::make_pair(wind, wind->getSceneNode());
}


std::shared_ptr<std::vector<std::byte>>
WindFactory::serializeParams(Vec2 pos, Size size, float scale, Vec2 windDirection, Vec2 windStrength) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(size.width);
    _serializer.writeFloat(size.height);
    _serializer.writeFloat(scale);
    _serializer.writeFloat(windDirection.x);
    _serializer.writeFloat(windDirection.y);
    _serializer.writeFloat(windStrength.x);
    _serializer.writeFloat(windStrength.y);
    

    return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
}

std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>>
WindFactory::createObstacle(const std::vector<std::byte>& params) {
    _deserializer.reset();
    _deserializer.receive(params);

    float posX = _deserializer.readFloat();
    float posY = _deserializer.readFloat();
    Vec2 pos(posX, posY);

    float width  = _deserializer.readFloat();
    float height = _deserializer.readFloat();
    Size size(width, height);

    float scale = _deserializer.readFloat();
    
    float dirX = _deserializer.readFloat();
    float dirY = _deserializer.readFloat();
    Vec2 dir(dirX, dirY);
    
    float strX = _deserializer.readFloat();
    float strY = _deserializer.readFloat();
    Vec2 str(strX, strY);

    return createObstacle(pos, size, scale, dir, str);
}
