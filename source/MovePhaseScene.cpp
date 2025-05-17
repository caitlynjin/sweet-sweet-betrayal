//
//  MovePhaseScene.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/18/25.
//

#include "MovePhaseScene.h"
#include "Platform.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "PlayerModel.h"
#include "WindObstacle.h"
#include "LevelModel.h"
#include "ObjectController.h"
#include "Mushroom.h"

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

#pragma mark -
#pragma mark Scene Constants

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0 / 16.0

/** The number pixels in a Box2D unit */
#define BOX2D_UNIT 64.0f

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH (SCENE_WIDTH / BOX2D_UNIT) * 2
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT (SCENE_HEIGHT / BOX2D_UNIT)

#define FIXED_TIMESTEP_S 0.02f

#pragma mark -
#pragma mark Level Geography

/** The goal door position */
float GOAL_POS[] = { 47.0f, 4.0f };
/** The initial position of the dude */

float DUDE_POS[] = { 1.0f, 4.0f};


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
MovePhaseScene::MovePhaseScene() : Scene2(),
    _worldnode(nullptr),
    _debugnode(nullptr),
    _localPlayer(nullptr),
    _treasure(nullptr)
{}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MovePhaseScene::dispose() {
    _worldnode = nullptr;
    _debugnode = nullptr;
};

/**
 * Initializes the scene contents
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MovePhaseScene::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::distrib::NetWorld> world, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController, std::vector<std::shared_ptr<Object>>* objects) {
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }

    _assets = assets;
    _world = world;
    _gridManager = gridManager;
    _networkController = networkController;
    _network = networkController->getNetwork();
    _initialCameraPos = getCamera()->getPosition();
    _objects = objects;

    _scale = _size.width == SCENE_WIDTH ? _size.width / DEFAULT_WIDTH : _size.height / DEFAULT_HEIGHT;
    _scale /= getSystemScale();
    _offset = Vec2((_size.width - SCENE_WIDTH) / getSystemScale(), (_size.height - SCENE_HEIGHT)) / getSystemScale();

    // Create the scene graph
    std::shared_ptr<Texture> image;
    _worldnode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(_offset);
    addChild(_worldnode);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(_offset);
    addChild(_debugnode);

    // Initialize object controller
    _objectController = std::make_shared<ObjectController>(_assets, _world, _scale, _worldnode, _debugnode, _objects);

    addChild(_gridManager->getGridNode());

    _active = true;
    
//    populate();

    return true;
}

void MovePhaseScene::createLocalPlayer(){
    // HOST STARTS ON LEFT
    Vec2 pos = DUDE_POS;
    // CLIENT STARTS ON RIGHT
    if (_networkController->getLocalID() == 2) {
        pos += Vec2(0, 3);
    }
    if (_networkController->getLocalID() == 3) {
        pos += Vec2(3, 0);
    }
    if (_networkController->getLocalID() == 4) {
        pos += Vec2(3, 3.5);
    }
        
    ColorType playerColor = _networkController->getLocalColor();
    _localPlayer = _networkController->createPlayerNetworked(pos, _scale, playerColor);
    
    _networkController->setLocalPlayer(_localPlayer);
    
    _localPlayer->setDebugScene(_debugnode);
    _localPlayer->setLocal();
    _world->getOwnedObstacles().insert({ _localPlayer,0 });
    //If we are on keyboard, for debugging purposes turn off jump damping
    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        _localPlayer->setJumpDamping(false);
    }
    if (!_networkController->getIsHost()) {
        _network->getPhysController()->acquireObs(_localPlayer, 0);
    }
}


bool MovePhaseScene::rebuildLevel(std::vector<std::shared_ptr<Object>>* objects){
    // Create the scene graph
    std::shared_ptr<Texture> image;
    _worldnode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(_offset);
    addChild(_worldnode);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(_offset);
    addChild(_debugnode);

    // Initialize object controller
    _objectController = std::make_shared<ObjectController>(_assets, _world, _scale, _worldnode, _debugnode, objects);

    addChild(_gridManager->getGridNode());

    _active = true;
    
    return true;
}

/**
 * Lays out the game geography.
 *
 * Pay close attention to how we attach physics objects to a scene graph.
 * The simplest way is to make a subclass, like we do for the Dude.  However,
 * for simple objects you can just use a callback function to lightly couple
 * them.  This is what we do with the crates.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void MovePhaseScene::populate() {
#pragma mark : Goal door
    _goalDoor = _objectController->createGoalDoor(Vec2(GOAL_POS[0], GOAL_POS[1]));

#pragma mark : Level
    shared_ptr<LevelModel> level = make_shared<LevelModel>();
    level->setScale(_scale);
    std::string key;
    
    // Pick the level based on levelNum
    string levelName;
    
    if (_levelNum == 1){
        levelName = "json/party.json";
    }
    else if (_levelNum == 2){
        levelName = "json/gorges.json";
    }
    else if (_levelNum == 3){
        levelName = LEVEL_3;
    }
    else{
        CULog("NO LEVEL SET");
    }
    
    vector<shared_ptr<Object>> levelObjs = level->createLevelFromJson(levelName);
    _gridManager->clear();
    _objectController->setNetworkController(_networkController);
    for (auto& obj : levelObjs) {
        _objectController->processLevelObject(obj);
        _gridManager->addObject(obj);
        CULog("new object position: (%f, %f)", obj->getPositionInit().x, obj->getPositionInit().y);
    }
#pragma mark : Dude
    std::shared_ptr<scene2::SceneNode> node = scene2::SceneNode::alloc();
    std::shared_ptr<Texture> image = _assets->get<Texture>(PLAYER_TEXTURE);

    // HOST STARTS ON LEFT
    Vec2 pos = DUDE_POS;
    // CLIENT STARTS ON RIGHT
    if (_networkController->getLocalID() == 2) {
        pos += Vec2(0, 3);
    }
    if (_networkController->getLocalID() == 3) {
        pos += Vec2(3, 0);
    }
    if (_networkController->getLocalID() == 4) {
        pos += Vec2(3, 3.5);
    }
        
    ColorType playerColor = _networkController->getLocalColor();
    _localPlayer = _networkController->createPlayerNetworked(pos, _scale, playerColor);
    
    _networkController->setLocalPlayer(_localPlayer);

    // This is set to false to counter race condition with collision filtering
    // NetworkController sets this back to true once it sets collision filtering to all players
    // There is a race condition where players are colliding when they spawn in, causing a player to get pushed into the void
    // If I do not disable the player, collision filtering works after build phase ends, not sure why
    // TODO: Find a better solution, maybe only have players getting updated during movement phase
    _localPlayer->setEnabled(false);

    _localPlayer->setDebugScene(_debugnode);
    _localPlayer->setLocal();
    _world->getOwnedObstacles().insert({ _localPlayer,0 });
    //If we are on keyboard, for debugging purposes turn off jump damping
    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        _localPlayer->setJumpDamping(false);
    }
    if (!_networkController->getIsHost()) {
        _network->getPhysController()->acquireObs(_localPlayer, 0);
    }

#pragma mark : Treasure
    if(_networkController->getIsHost()){
        // Create Spawn Point for the treasure
        
        Vec2 spawnPoint = _networkController->pickRandSpawn();
        
        _treasure = std::dynamic_pointer_cast<Treasure>(
            _networkController->createTreasureNetworked(spawnPoint, Size(1, 1), _scale, false)
        );
//        
//        _treasure = std::dynamic_pointer_cast<Treasure>(_objectController->createTreasure(spawnPoint, Size(1,1), "default"));
        _networkController->setTreasure(_treasure);
        _networkController->setTreasureSpawn(spawnPoint);
    }
    CULog("CHUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUCCCCCCCCCK");
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void MovePhaseScene::reset() {
    resetPlayerProperties();
    
    _localPlayer = nullptr;
    _goalDoor = nullptr;
    _treasure = nullptr;
    _objects = nullptr;
    
    _levelNum = 0;

    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();

    _camera->setPosition(_initialCameraPos);
    _camera->update();

//    populate();
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void MovePhaseScene::preUpdate(float dt) {
    // Set up treasure for non-host player    
    if (_treasure == nullptr && !_networkController->getIsHost()){
        const auto& obstacles = _world->getObstacles();
        for (const auto& obstacle : obstacles) {
            
            if (obstacle->getName() == "treasure"){
                // Try to cast to Treasure and add to our list if successful
                auto treasure = std::dynamic_pointer_cast<Treasure>(obstacle);
                if (treasure) {
                    _treasure = treasure;
                    _networkController->setTreasure(_treasure);
                    
                } else {
                    CULog("Found player but casting failed");
                }
            }
        }
    }
    
    
    _camera->update();
}


#pragma mark -
#pragma mark Helpers
/**
 * Set whether the debug node is visible
 */
void MovePhaseScene::setDebugVisible(bool value) {
    _debugnode->setVisible(value);
}

/**
 * Resets the camera position to the initial state.
 */
void MovePhaseScene::resetCameraPos() {
    _camera->setPosition(_initialCameraPos);
}

/**
 * Resets the player properties at the end of a round.
 */
void MovePhaseScene::resetPlayerProperties() {
    _localPlayer->setPosition(Vec2(DUDE_POS));
    _localPlayer->resetMovement();
    if (_localPlayer->hasTreasure){
        _localPlayer->removeTreasure();
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::TREASURE_WON));
    }
    
    std::vector<std::shared_ptr<PlayerModel>> players = _networkController->getPlayerList();
    for (auto player : players){
        player->setDead(false);
    }

}


/**
 * Converts from screen to Box2D coordinates.
 *
 * @return the Box2D position
 *
 * @param screenPos    The screen position
 */
Vec2 MovePhaseScene::convertScreenToBox2d(const Vec2 &screenPos)
{
    Vec2 adjusted = screenPos - _offset;

    // Adjust for camera position
    Vec2 worldPos = adjusted + (_camera->getPosition() - _initialCameraPos);

    float xBox2D = worldPos.x / _scale;
    float yBox2D = worldPos.y / _scale;

    // Converts to the specific grid position
    int xGrid = xBox2D;
    int yGrid = yBox2D;

    return Vec2(xGrid, yGrid);
}

/**
 * This method links a scene node to the obstacle.
}

/**
 * This method links a scene node to the obstacle.
 *
 * This method adds a listener so that the sceneNode will move along with the obstacle.
 */
void MovePhaseScene::linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj,
    const std::shared_ptr<scene2::SceneNode>& node) {

    node->setPosition(obj->getPosition() * _scale);
    if (!_worldnode){
        return;
    }

    _worldnode->addChild(node);
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=,this](physics2::Obstacle* obs) {
            float leftover = Application::get()->getFixedRemainder() / 1000000.f;
            Vec2 pos = obs->getPosition() + leftover * obs->getLinearVelocity();
            float angle = obs->getAngle() + leftover * obs->getAngularVelocity();
            weak->setPosition(pos * _scale);
            weak->setAngle(angle);
        });
    }
}

