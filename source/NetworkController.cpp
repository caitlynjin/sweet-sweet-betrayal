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
    _network->attachEventType<ScoreEvent>();
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

/**
     * The method called to update the game mode.
     *
     * This is the nondeterministic version of a physics simulation. It is
     * provided for comparison purposes only.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
void NetworkController::update(float timestep){
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
        // Check for ScoreEvent
        if(auto sEvent = std::dynamic_pointer_cast<ScoreEvent>(e)){
            _scoreController->processScoreEvent(sEvent);
        }
        
    }
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
    
    // Test all color map

//    CULog("PlayerIDs size: %d", static_cast<int>(_playerIDs.size()));
//    for (int id : _playerIDs){
//        CULog("Player id: %d, Player Color: %d", id, static_cast<int>(_playerColorsById[id]));
//    }
    
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
        // Check for ScoreEvent
        if(auto sEvent = std::dynamic_pointer_cast<ScoreEvent>(e)){
            _scoreController->processScoreEvent(sEvent);
        }
    }

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

}


/**
 * Resets the status of the game so that we can play again.
 */
void NetworkController::reset(){
    // TODO: Might need to add reset logic

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
        case Message::BUILD_READY:
            // Increment number of players ready
            _numReady++;
            break;
        
        case Message::MOVEMENT_END:
            // Increment number of players needed to be reset
            _numReset++;
            break;
            
        case Message::TREASURE_TAKEN:
            // Increment number of players needed to be reset
            _treasure->setTaken(true);
            break;
        case Message::TREASURE_LOST:
            // Reset treasure
            resetTreasure();
            break;
        case Message::HOST_START:
            // Send message for everyone to send player id and color
            _network->pushOutEvent(ColorEvent::allocColorEvent(_network->getShortUID(), _color));
            break;
        case Message::SCORE_UPDATE:
//            _network
            break;
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
    
    // Store each color into map by player id
    _playerColorsById[playerID] = color;
    _playerIDs.push_back(playerID);
}

/** Resets the treasure to remove possession and return to spawn location */
void NetworkController::resetTreasure(){
    _treasure->setTaken(false);
    _treasure->setPosition(_treasureSpawn);
    
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
    auto params = _platFact->serializeParams(pos + size/2, size, jsonType, scale);
    // pair holds the boxObstacle and sprite to be used for the platform
    // Already added to _world after this call
    auto pair = _network->getPhysController()->addSharedObstacle(_platFactId, params);

    // Cast the obstacle to a BoxObstacle
    auto boxObstacle = std::dynamic_pointer_cast<cugl::physics2::BoxObstacle>(pair.first);

    
    std::shared_ptr<scene2::SceneNode> sprite = pair.second;
    // Check if the cast was successful
    if (boxObstacle) {
        // Assign the boxObstacle that was made to the platform
        std::shared_ptr<Platform> plat = Platform::alloc(pos, size, boxObstacle);
        _objects->push_back(plat);
        return plat;
    } else {
        // Handle case where the obstacle is not a BoxObstacle
        CULog("Error: Expected a BoxObstacle but got a different type");
        return nullptr;
    }
}
/**
 * Creates a networked moving platform.
 *
 * @return the moving platform being created
 */
std::shared_ptr<Object> NetworkController::createMovingPlatformNetworked(Vec2 pos, Size size, Vec2 end, float speed, float scale) {
    
    auto params = _movingPlatFact->serializeParams(pos + size/2, size, end + size/2, speed, scale);

    auto pair = _network->getPhysController()->addSharedObstacle(_movingPlatFactID, params);

    // Attempt to cast the obstacle to a BoxObstacle.
    auto boxObstacle = std::dynamic_pointer_cast<cugl::physics2::BoxObstacle>(pair.first);
    std::shared_ptr<scene2::SceneNode> sprite = pair.second;
    if (boxObstacle) {
        std::shared_ptr<Platform> plat = Platform::allocMoving(pos, size, pos, end, speed, boxObstacle);
        _objects->push_back(plat);
        return plat;
    } else {
        
        CULog("Error: Expected a BoxObstacle but got a different type");
        return nullptr;
    }
}
std::shared_ptr<Object> NetworkController::createTreasureNetworked(Vec2 pos, Size size, float scale, bool taken) {
    CULog("creating treasure");
    auto params = _treasureFact->serializeParams(pos, size, scale, taken);
    auto pair = _network->getPhysController()->addSharedObstacle(_treasureFactID, params);

    auto boxObstacle = std::dynamic_pointer_cast<cugl::physics2::BoxObstacle>(pair.first);
    std::shared_ptr<scene2::SceneNode> sprite = pair.second;
    
    if (boxObstacle) {
        std::shared_ptr<Treasure> treasure = Treasure::alloc(pos, size, scale, taken, boxObstacle);
        _objects->push_back(treasure);
        return treasure;
    } else {
        CULog("Error: Expected a BoxObstacle but got a different type");
        return nullptr;
    }
}

std::shared_ptr<Object> NetworkController::createTreasureClient(Vec2 pos, Size size, float scale, bool isTaken){
    // Find the hitbox in network world
    std::shared_ptr<cugl::physics2::BoxObstacle> box;
    const auto& obstacles = _world->getObstacles();
    for (const auto& obstacle : obstacles) {
        if (obstacle->getName() == "treasure"){
            CULog("treasure is set for client");
            box = std::dynamic_pointer_cast<BoxObstacle>(obstacle);
            break;
        }
    }
    
    // Rest of initialization
    std::shared_ptr<Texture> image;
    std::shared_ptr<scene2::PolygonNode> sprite;
    Vec2 treasurePos = pos;
    image = _assets->get<Texture>("treasure");
    _treasure = Treasure::alloc(treasurePos,image->getSize()/scale,scale, false, box);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    _treasure->setSceneNode(sprite);
    _treasure->getObstacle()->setDebugColor(Color4::YELLOW);

    _treasure->setPosition(pos);
    _objects->push_back(_treasure);
    return _treasure;
}


std::shared_ptr<Object> NetworkController::createMushroomNetworked(Vec2 pos, Size size, float scale) {
    CULog("creating mushroom");
    auto params = _mushroomFact->serializeParams(pos+size/2, size, scale);
    auto pair = _network->getPhysController()->addSharedObstacle(_mushroomFactID, params);

    auto boxObstacle = std::dynamic_pointer_cast<cugl::physics2::BoxObstacle>(pair.first);
    std::shared_ptr<scene2::SceneNode> sprite = pair.second;
    
    if (boxObstacle) {
        std::shared_ptr<Mushroom> mushroom = Mushroom::alloc(pos+size/2, size, scale, boxObstacle);
        _objects->push_back(mushroom);
        return mushroom;
    } else {
        CULog("Error: Expected a BoxObstacle but got a different type");
        return nullptr;
    }
}


/**
 * Creates a networked player.
 *
 * @return the player being created
 *
 * @param The player being created (that has not yet been added to the physics world).
 */
std::shared_ptr<PlayerModel> NetworkController::createPlayerNetworked(Vec2 pos, float scale){
    auto params = _dudeFact->serializeParams(pos, scale);
    auto localPair = _network->getPhysController()->addSharedObstacle(_dudeFactID, params);
    return std::dynamic_pointer_cast<PlayerModel>(localPair.first);
}


#pragma mark -
#pragma mark Helpers

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
        if (obstacle->getName() == "player"){
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

void NetworkController::addPlayerColor(){
    if (_isHost){
        _color = ColorType::RED;
    }
    else{
        // Determine color by order joined
        _color = static_cast<ColorType>(_network->getNumPlayers() - 1);
    }
    _playerColorAdded = true;
}


#pragma mark -
#pragma mark Factories



#pragma mark -
#pragma mark Dude Factory

/**
 * Generate a pair of Obstacle and SceneNode using the given parameters
 */
std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> DudeFactory::createObstacle(Vec2 pos, float scale) {
    auto image = _assets->get<Texture>(PLAYER_TEXTURE);

    auto player = PlayerModel::alloc(pos, image->getSize() / scale, scale);
    
    player->setShared(true);
    player->setDebugColor(DEBUG_COLOR);
    
    auto idleSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_IDLE_TEXTURE), 1, 7, 7);
    player->setIdleAnimation(idleSpriteNode);
    
    auto walkSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_WALK_TEXTURE), 1, 3, 3);
    player->setWalkAnimation(walkSpriteNode);
    
    auto glideSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_GLIDE_TEXTURE), 1, 4, 4);
    player->setGlideAnimation(glideSpriteNode);
    
    auto jumpSpriteNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(PLAYER_JUMP_TEXTURE), 1, 5, 5);
    player->setJumpAnimation(jumpSpriteNode);
    
    return std::make_pair(player, player->getSceneNode());
}

/**
 * Helper method for converting normal parameters into byte vectors used for syncing.
 */
std::shared_ptr<std::vector<std::byte>> DudeFactory::serializeParams(Vec2 pos, float scale) {
    _serializer.reset();
    _serializer.writeFloat(pos.x);
    _serializer.writeFloat(pos.y);
    _serializer.writeFloat(scale);
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
    return createObstacle(pos, scale);
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

    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Poly2 poly(Rect(pos.x, pos.y, size.width - blendingOffset, size.height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Set the physics attributes
    std::shared_ptr<cugl::physics2::BoxObstacle> box = cugl::physics2::BoxObstacle::alloc(pos, Size(size.width, size.height));
    
    box->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    box->setDensity(BASIC_DENSITY);
    box->setFriction(BASIC_FRICTION);
    box->setRestitution(BASIC_RESTITUTION);
    box->setDebugColor(DEBUG_COLOR);
    box->setName("platform");
    
    box->setShared(true);

    poly *= scale;
    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);

    
    return std::make_pair(box, sprite);
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

    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Poly2 poly(Rect(pos.x, pos.y, size.width - blendingOffset, size.height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    std::shared_ptr<cugl::physics2::BoxObstacle> box = cugl::physics2::BoxObstacle::alloc(pos, Size(size.width, size.height));
    
    box->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    box->setDensity(BASIC_DENSITY);
    box->setFriction(BASIC_FRICTION);
    box->setRestitution(BASIC_RESTITUTION);
    box->setDebugColor(DEBUG_COLOR);
    box->setName("movingPlatform");

  
    poly *= scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, poly);
    
    return std::make_pair(box, sprite);
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
    
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    

    auto treasure = Treasure::alloc(pos, image->getSize() / scale, scale);
    
    treasure->setSceneNode(sprite);
    treasure->getObstacle()->setName("treasure");
    treasure->getObstacle()->setDebugColor(Color4::YELLOW);
    treasure->setPosition(pos);
    treasure->getObstacle()->setShared(true);
    
    return std::make_pair(treasure->getObstacle(), sprite);
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

    // float blendingOffset = 0.01f;

    // Poly2 poly(Rect(pos.x, pos.y, size.width - blendingOffset, size.height - blendingOffset));

    // EarclipTriangulator triangulator;
    // triangulator.set(poly.vertices);
    // triangulator.calculate();
    // poly.setIndices(triangulator.getTriangulation());
    // triangulator.clear();
    
    std::shared_ptr<Texture> texture = _assets->get<Texture>("mushroom");
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(texture);
    
    auto mushroom = Mushroom::alloc(pos, size, scale);
    mushroom->getObstacle()->setBodyType(b2_dynamicBody);
    mushroom->getObstacle()->setDensity(BASIC_DENSITY);
    mushroom->getObstacle()->setFriction(BASIC_FRICTION);
    mushroom->getObstacle()->setRestitution(BASIC_RESTITUTION);
    mushroom->getObstacle()->setName("mushroom");
    mushroom->getObstacle()->setDebugColor(Color4::YELLOW);
    mushroom->setPosition(pos);
    mushroom->getObstacle()->setShared(true);
    
    return std::make_pair(mushroom->getObstacle(), sprite);
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
