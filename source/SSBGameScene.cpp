//
//  SSBGameScene.cpp
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 18/2/25.
//

#include "SSBGameScene.h"
#include "Constants.h"
#include "Platform.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "SSBDudeModel.h"
#include "WindObstacle.h"
#include "LevelModel.h"

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
#pragma mark Level Geography

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

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
// #define WALL_VERTS 8
// #define WALL_COUNT  0

// float WALL[WALL_COUNT][WALL_VERTS] = {
//     { 0.0f, 1.0f, 0.0f, 0.0f, 20.0f, 0.0f, 20.0f, 1.0f }
// };
// float WALL[WALL_COUNT][WALL_VERTS];

///** The number of platforms */
// #define PLATFORM_VERTS  8
// #define PLATFORM_COUNT  1  // Only one ground platform
//
///** The single large ground platform */
// float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
//     { 0.0f, 1.0f, 0.0f, 0.0f, 32.0f, 0.0f, 32.0f, 1.0f }
// };

/** The goal door position */
float GOAL_POS[] = { 31.0f, 6.0f };
/** The initial position of the dude */

float DUDE_POS[] = { 1.0f, 3.0f};

/** The initial position of the treasure */
float TREASURE_POS[3][2] = { {14.5f, 7.5f}, {3.5f, 7.5f}, {9.5f, 1.5f}};



float SPIKE_POS[] = {5.5f, 1.5f};

#pragma mark -
#pragma mark Physics Constants
/** The new heavier gravity for this world (so it is not so floaty) */
#define DEFAULT_GRAVITY -28.9f
/** The density for most physics objects */
#define BASIC_DENSITY 0.0f
/** The density for a bullet */
#define HEAVY_DENSITY 10.0f
/** Friction of most platforms */
#define BASIC_FRICTION 0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION 0.1f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT 240

#pragma mark -
#pragma mark Asset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "gray"
/** The key for the platform texture in the asset manager*/
#define PLATFORM_TEXTURE   "platform"
/** The key for the 3x0.5 platform texture in the asset manager */
#define PLATFORM_LONG_TEXTURE   "platform_long"
/** The key for the moving platform texture in the asset manager*/
#define MOVING_TEXTURE   "moving"
/** The key for the spike texture in the asset manager */
#define SPIKE_TEXTURE "spike"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE    "goal"
/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"
/** The key for the treasure texture in the asset manager */
#define TREASURE_TEXTURE    "treasure"
/** The name of a wall (for object identification) */
#define WALL_NAME "wall"
/** Name of the wind texture*/
#define WIND_TEXTURE "up"
/** Name of the wind object(for identification)*/
#define WIND_NAME "gust"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME "platform"
/** The font for victory/failure messages */
#define MESSAGE_FONT    "retro"
/** The font for Round and Gem info */
#define INFO_FONT    "marker"

/** The message for winning the game */
#define WIN_MESSAGE "VICTORY!"
/** The color of the win message */
#define WIN_COLOR Color4::YELLOW
/** The message for losing the game */
#define LOSE_MESSAGE "FAILURE!"
/** The color of the lose message */
#define LOSE_COLOR      Color4::RED
/** The color of the info labels */
#define INFO_COLOR      Color4::WHITE
/** The key the basic game music */
#define GAME_MUSIC "game"
/** The key the victory game music */
#define WIN_MUSIC "win"
/** The key the failure game music */
#define LOSE_MUSIC "lose"
/** The sound effect for firing a bullet */
#define PEW_EFFECT "pew"
/** The sound effect for a bullet collision */
#define POP_EFFECT "pop"
/** The sound effect for jumping */
#define JUMP_EFFECT "jump"
/** The volume for the music */
#define MUSIC_VOLUME 0.7f
/** The volume for sound effects */
#define EFFECT_VOLUME 0.8f
/** The image for the left dpad/joystick */
#define LEFT_IMAGE      "dpad_left"
/** The image for the empty gem */
#define EMPTY_IMAGE      "gemEmpty"
/** The image for the full gem */
#define FULL_IMAGE      "gemFull"
/** The image for the right dpad/joystick */
#define RIGHT_IMAGE "dpad_right"
/** The image for the ready button */
#define READY_BUTTON "ready_button"

/** Color to outline the physics nodes */
#define DEBUG_COLOR Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY 192

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : Scene2(),
    _worldnode(nullptr),
    _debugnode(nullptr),
    _world(nullptr),
    _avatar(nullptr),
    _treasure(nullptr),
    _complete(false),
    _debug(false)

{
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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets)
{
    return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY));
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager> &assets, const Rect &rect)
{
    return init(assets, rect, Vec2(0, DEFAULT_GRAVITY));
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 * @param gravity   The gravitational force on this Box2d world
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager> &assets,
                     const Rect &rect, const Vec2 &gravity)
{
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }


    // Start in building mode
    _buildingMode = true;

    // Start up the input handler
    _assets = assets;
    _input.init(getBounds());

    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect, gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact *contact)
    {
        beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact *contact)
    {
        endContact(contact);
    };

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = _size.width == SCENE_WIDTH ? _size.width / rect.size.width : _size.height / rect.size.height;
    Vec2 offset = Vec2((_size.width - SCENE_WIDTH) / 2.0f, (_size.height - SCENE_HEIGHT) / 2.0f);
    _offset = offset;

    // TODO: Bring back background
//    _background = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BACKGROUND_TEXTURE));
//    _background->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//    _background->setPosition(Vec2(0,0));
//    _background->setScale(1.0f);
//    addChild(_background);

    // Create the scene graph
    std::shared_ptr<Texture> image;
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);

    _winnode = scene2::Label::allocWithText(WIN_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(_size.width / 2.0f, _size.height / 2.0f);
    _winnode->setForeground(WIN_COLOR);
    setComplete(false);

    _losenode = scene2::Label::allocWithText(LOSE_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _losenode->setAnchor(Vec2::ANCHOR_CENTER);
    _losenode->setPosition(_size.width / 2.0f, _size.height / 2.0f);
    _losenode->setForeground(LOSE_COLOR);
    setFailure(false);
    
//    _roundsnode = scene2::Label::allocWithText("Round: 1/" + std::to_string(TOTAL_ROUNDS), _assets->get<Font>(INFO_FONT));
//    _roundsnode->setAnchor(Vec2::ANCHOR_CENTER);
//    _roundsnode->setPosition(_size.width * .75,_size.height * .9);
//    _roundsnode->setForeground(INFO_COLOR);
//    _roundsnode->setVisible(true);
    
    float distance = _size.width * .05;
    for (int i = 0; i < TOTAL_GEMS; i++){
        std::shared_ptr<scene2::PolygonNode> scoreNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(EMPTY_IMAGE));
        scoreNode->SceneNode::setAnchor(Vec2::ANCHOR_CENTER);
        scoreNode->setPosition(_size.width * .15 + (i*distance),_size.height * .9);
        scoreNode->setScale(0.1f);
        scoreNode->setVisible(true);
        _scoreImages.push_back(scoreNode);
    }

    _leftnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_IMAGE));
    _leftnode->SceneNode::setAnchor(Vec2::ANCHOR_MIDDLE_RIGHT);
    _leftnode->setScale(0.35f);
    _leftnode->setVisible(false);

    _rightnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RIGHT_IMAGE));
    _rightnode->SceneNode::setAnchor(Vec2::ANCHOR_MIDDLE_LEFT);
    _rightnode->setScale(0.35f);
    _rightnode->setVisible(false);

//    std::shared_ptr<scene2::PolygonNode> readyNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
//    readyNode->setScale(0.8f);
//    _readyButton = scene2::Button::alloc(readyNode);
//    _readyButton->setAnchor(Vec2::ANCHOR_CENTER);
//    _readyButton->setPosition(_size.width * 0.91f, _size.height * 0.1f);
//    _readyButton->activate();
//    _readyButton->addListener([this](const std::string &name, bool down) {
//        if (down && _buildingMode) {
//            setBuildingMode(!_buildingMode);
//            _readyButton->setVisible(false);
//        }
//    });


    _gridManager = GridManager::alloc(DEFAULT_HEIGHT, DEFAULT_WIDTH, _scale, offset, assets);

    initInventory();

    _scrollpane = scene2::ScrollPane::allocWithBounds(getBounds() / 2);
    _scrollpane->setInterior(getBounds() / 2);
    _scrollpane->setConstrained(false);

    // Set initial camera position
    _camerapos = getCamera()->getPosition();

    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_winnode);
    addChild(_losenode);
//    addChild(_roundsnode);
    addChild(_leftnode);
    addChild(_rightnode);
    addChild(_scrollpane);
//    addChild(_readyButton);
    addChild(_gridManager->getGridNode());
    
    for (auto score : _scoreImages){
        _ui.addChild(score);
    }

    _ui.init(assets);

    populate();

    _active = true;
    _complete = false;
    setDebug(false);


    // XNA nostalgia
    Application::get()->setClearColor(Color4f::CORNFLOWER);

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose()
{
    if (_active)
    {
        _input.dispose();
        _world = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winnode = nullptr;
        _losenode = nullptr;
//        _roundsnode = nullptr;
        _leftnode = nullptr;
        _rightnode = nullptr;
        //_readyButton = nullptr;
        _gridManager->getGridNode() = nullptr;
        _complete = false;
        _debug = false;
        for (auto score : _scoreImages){
            score = nullptr;
        }
        _ui.dispose();
        Scene2::dispose();
    }
}

#pragma mark -
#pragma mark Build Mode

/**
 * Initializes the grid layout on the screen for build mode.
 */
void GameScene::initInventory()
{
    std::vector<Item> inventoryItems = {PLATFORM, MOVING_PLATFORM, WIND};
    std::vector<std::string> assetNames = {PLATFORM_LONG_TEXTURE, MOVING_TEXTURE, WIND_TEXTURE};

    // Set the background
    _inventoryBackground = scene2::PolygonNode::alloc();
    _inventoryBackground->setPosition(Vec2(_size.width*0.88, _size.height*0.2));
    _inventoryBackground->setContentSize(Size(_size.width*0.18, _size.height*0.8));
    _inventoryBackground->setColor(Color4(131,111,108));
    _inventoryBackground->setVisible(true);
    _ui.addChild(_inventoryBackground);

    float yOffset = 0;
    for (size_t itemNo = 0; itemNo < inventoryItems.size(); itemNo++)
    {
        std::shared_ptr<scene2::PolygonNode> itemNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(assetNames[itemNo]));
        std::shared_ptr<scene2::Button> itemButton = scene2::Button::alloc(itemNode);
        itemButton->setAnchor(Vec2::ANCHOR_TOP_RIGHT);
        itemButton->setPosition(_size.width - 10, _size.height - 100 - yOffset);
        itemButton->setName(itemToString(inventoryItems[itemNo]));
        itemButton->setVisible(true);
        itemButton->activate();
        itemButton->addListener([this, item = inventoryItems[itemNo]](const std::string &name, bool down) {
            if (down & _buildingMode) {
                _selectedItem = item;
                _input.setInventoryStatus(PlatformInput::PLACING);
            }
        });
        _inventoryButtons.push_back(itemButton);
        _ui.addChild(itemButton);
        yOffset += 80;
    }

    // Set the darkened overlay
    _inventoryOverlay = scene2::PolygonNode::alloc();
    _inventoryOverlay->setPosition(Vec2(_size.width * 0.88, _size.height * 0.2));
    _inventoryOverlay->setContentSize(Size(_size.width * 0.18, _size.height * 0.8));
    _inventoryOverlay->setColor(Color4(0, 0, 0, 128));
    _inventoryOverlay->setVisible(false);
    _ui.addChild(_inventoryOverlay);
}

/**
 * Creates an item of type item and places it at the grid position.
 *
 *@return the object being placed and created
 *
 * @param gridPos   The grid position to place the item at
 * @param item  The type of the item to be placed/created
 */
std::shared_ptr<Object> GameScene::placeItem(Vec2 gridPos, Item item) {

    switch (item) {
        case (PLATFORM):
            return createPlatform(gridPos, Size(3, 1), "default");
        case (MOVING_PLATFORM):
            return createMovingPlatform(gridPos, Size(3, 1), gridPos + Vec2(3, 0), 1);
        case (WIND):
            return createWindObstacle(gridPos, Size(1, 1), Vec2(0, 1.0), "default");
        case (NONE):
            return nullptr;
    }
}

#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset()
{
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    _avatar = nullptr;
    _goalDoor = nullptr;
//    if (_growingWall && _world->getObstacles().count(_growingWall) > 0)
//    {
//        _world->removeObstacle(_growingWall);
//        _worldnode->removeChild(_growingWallNode);
//    }
//    
//    _growingWall = nullptr;
//    _growingWallNode = nullptr;
//    _growingWallWidth = 0.1f;
    _treasure = nullptr;

    _currRound = 1;
    _died = false;
    _reachedGoal = false;

    setFailure(false);
    setComplete(false);
    setBuildingMode(true);
    getCamera()->setPosition(_camerapos);
    getCamera()->update();
    for (size_t i = 0; i < _inventoryButtons.size(); i++)
    {
        _inventoryButtons[i]->activate();
    }
    _inventoryOverlay->setVisible(false);
    //_readyButton->setVisible(true);
    _itemsPlaced = 0;
    _ui.reset();

    populate();
}

std::shared_ptr<Object> GameScene::createPlatform(std::shared_ptr<Platform> plat) {
    std::shared_ptr<Texture> image;
    if (plat->getJsonType() == "wall") {
        image = _assets->get<Texture>(PLATFORM_TEXTURE);
    }
    else {
        image = _assets->get<Texture>(PLATFORM_LONG_TEXTURE);
    }

    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Poly2 poly(Rect(plat->getPosition().x, plat->getPosition().y, plat->getSize().width - blendingOffset, plat->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Set the physics attributes
    plat->getObstacle()->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    plat->getObstacle()->setDensity(BASIC_DENSITY);
    plat->getObstacle()->setFriction(BASIC_FRICTION);
    plat->getObstacle()->setRestitution(BASIC_RESTITUTION);
    plat->getObstacle()->setDebugColor(DEBUG_COLOR);
    plat->getObstacle()->setName("platform");

    poly *= _scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, poly);
    addObstacle(plat->getObstacle(), sprite, 1); // All walls share the same texture
    _objects.push_back(plat);

    return plat;
}
/**
 * Creates a new platform.
 *
 * @return the platform being created
 *
 * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
 * @param size The dimensions (width, height) of the platform.
 */
std::shared_ptr<Object> GameScene::createPlatform(Vec2 pos, Size size, string jsonType) {
    std::shared_ptr<Platform> plat = Platform::alloc(pos, size, jsonType);
    return createPlatform(plat);
}
/**
 * Creates a moving platform.
 *
 * @return the moving platform
 *
 * @param pos The bottom left position of the platform starting position
 * @param size The dimensions of the platform.
 * @param end The bottom left position of the platform's destination.
 * @param speed The speed at which the platform moves.
 */
std::shared_ptr<Object> GameScene::createMovingPlatform(Vec2 pos, Size size, Vec2 end, float speed) {
    std::shared_ptr<Texture> image = _assets->get<Texture>(MOVING_TEXTURE);
    
    std::shared_ptr<Platform> plat = Platform::allocMoving(pos, size, pos, end, speed);

    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Poly2 poly(Rect(plat->getPosition().x, plat->getPosition().y, plat->getSize().width - blendingOffset, plat->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    plat->getObstacle()->setDensity(BASIC_DENSITY);
    plat->getObstacle()->setFriction(BASIC_FRICTION);
    plat->getObstacle()->setRestitution(BASIC_RESTITUTION);
    plat->getObstacle()->setDebugColor(DEBUG_COLOR);
    plat->getObstacle()->setName("movingPlatform");

    poly *= _scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, poly);

    addObstacle(plat->getObstacle(), sprite, 1);
    _objects.push_back(plat);

    return plat;
}
/**
 * Create the growing wall if not created. Otherwise, increase its width
 *
 * @param timestep  The elapsed time since the last frame.
 */

void GameScene::updateGrowingWall(float timestep)
{
    // Increase the width
    _growingWallWidth += _growingWallGrowthRate * timestep;

    // Remove the old wall if it exists
    if (_growingWall && _world->getObstacles().count(_growingWall) > 0)
    {
        _world->removeObstacle(_growingWall);
        _worldnode->removeChild(_growingWallNode);
    }

    // Create a new polygon for the wall
    Poly2 wallPoly;
    wallPoly.vertices.push_back(Vec2(0, DEFAULT_HEIGHT*.80));
    wallPoly.vertices.push_back(Vec2(0, 0));
    wallPoly.vertices.push_back(Vec2(_growingWallWidth, 0));
    wallPoly.vertices.push_back(Vec2(_growingWallWidth, DEFAULT_HEIGHT*.80));

    EarclipTriangulator triangulator;
    triangulator.set(wallPoly.vertices);
    triangulator.calculate();
    wallPoly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Create the collision box
    _growingWall = physics2::PolygonObstacle::allocWithAnchor(wallPoly, Vec2::ANCHOR_BOTTOM_LEFT);
    _growingWall->setName("growingWall");
    _growingWall->setBodyType(b2_staticBody);
    _growingWall->setDensity(BASIC_DENSITY);
    _growingWall->setFriction(BASIC_FRICTION);
    _growingWall->setRestitution(BASIC_RESTITUTION);
    _growingWall->setDebugColor(Color4::RED);

    wallPoly *= _scale;
    _growingWallNode = scene2::PolygonNode::allocWithPoly(wallPoly);
    _growingWallNode->setColor(Color4::RED);
    _growingWallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

    addObstacle(_growingWall, _growingWallNode, true);
}

/**
 * Creates a new spike.
 * @param pos The position of the bottom left corner of the spike in Box2D coordinates.
 * @param size The dimensions (width, height) of the spike.
 */
std::shared_ptr<Object> GameScene::createSpike(Vec2 pos, Size size, float scale, float angle, string jsonType)
{
    std::shared_ptr<Spike> spk = Spike::alloc(pos, size, scale, angle, jsonType);
    return createSpike(spk);
}

std::shared_ptr<Object> GameScene::createSpike(std::shared_ptr<Spike> spk)
{
    std::shared_ptr<Texture> image = _assets->get<Texture>(SPIKE_TEXTURE);

    // Set the physics attributes
    spk->getObstacle()->setBodyType(b2_staticBody);
    spk->getObstacle()->setDensity(BASIC_DENSITY);
    spk->getObstacle()->setFriction(BASIC_FRICTION);
    spk->getObstacle()->setRestitution(BASIC_RESTITUTION);
    spk->getObstacle()->setDebugColor(DEBUG_COLOR);
    spk->getObstacle()->setName("spike");

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    spk->setSceneNode(sprite, spk->getAngle());
    addObstacle(spk->getObstacle(), sprite);
    _objects.push_back(spk);
    return spk;
}

std::shared_ptr<Object> GameScene::createTreasure(Vec2 pos, Size size, string jsonType){
    std::shared_ptr<Texture> image;
    std::shared_ptr<scene2::PolygonNode> sprite;
    Vec2 treasurePos = pos;
    image = _assets->get<Texture>("treasure");
    _treasure = Treasure::alloc(treasurePos,image->getSize()/_scale,_scale);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    _treasure->setSceneNode(sprite);
    addObstacle(_treasure->getObstacle(),sprite);
    _treasure->getObstacle()->setName("treasure");
    _treasure->getObstacle()->setDebugColor(Color4::YELLOW);

    _treasure->setPosition(pos);
    _objects.push_back(_treasure);
    return _treasure;
}

std::shared_ptr<Object> GameScene::createTreasure(std::shared_ptr<Treasure> _treasure) {
    return createTreasure(_treasure->getPosition(), _treasure->getSize(), _treasure->getJsonType());
}

/**
 * Creates a new windobstacle
 *
 * @return the wind obstacle
 *
 * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
 * @param size The dimensions (width, height) of the platform.
 */
std::shared_ptr<Object> GameScene::createWindObstacle(Vec2 pos, Size size, Vec2 gust, string jsonType)
{
    std::shared_ptr<Texture> image = _assets->get<Texture>(WIND_TEXTURE);
    std::shared_ptr<WindObstacle> wind = WindObstacle::alloc(pos, size, gust);

    // Allow movement of obstacle
    wind->getObstacle()->setBodyType(b2_dynamicBody);

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);

    wind->setTrajectory(gust);
    wind->setPosition(pos);

    addObstacle(wind->getObstacle(), sprite, 1); // All walls share the same texture

    _objects.push_back(wind);

    return wind;
}

std::shared_ptr<Object> GameScene::createWindObstacle(std::shared_ptr<WindObstacle> wind)
{
    return createWindObstacle(wind->getPosition(), wind->getSize(), wind->gustDir(), wind->getJsonType());
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

void GameScene::populate()
{
#pragma mark : Goal door
    std::shared_ptr<Texture> image = _assets->get<Texture>(GOAL_TEXTURE);
    std::shared_ptr<scene2::PolygonNode> sprite;
    std::shared_ptr<scene2::WireNode> draw;

    // Create obstacle
    Vec2 goalPos = GOAL_POS;
    Size goalSize(image->getSize().width / _scale,
                  image->getSize().height / _scale);
    _goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);

    // Set the physics attributes
    _goalDoor->setBodyType(b2_staticBody);
    _goalDoor->setDensity(0.0f);
    _goalDoor->setFriction(0.0f);
    _goalDoor->setRestitution(0.0f);
    _goalDoor->setSensor(true);

    // Add the scene graph nodes to this object
    sprite = scene2::PolygonNode::allocWithTexture(image);
    sprite->setColor(Color4(1,255,0));
    _goalDoor->setDebugColor(DEBUG_COLOR);
    addObstacle(_goalDoor, sprite);

//#pragma mark : Walls
    // All walls and platforms share the same texture
//    image = _assets->get<Texture>(EARTH_TEXTURE);
//    std::string wname = "wall";
//    for (int ii = 0; ii < WALL_COUNT; ii++) {
//        std::shared_ptr<physics2::PolygonObstacle> wallobj;
//
//        Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]),WALL_VERTS/2);
//        // Call this on a polygon to get a solid shape
//        EarclipTriangulator triangulator;
//        triangulator.set(wall.vertices);
//        triangulator.calculate();
//        wall.setIndices(triangulator.getTriangulation());
//        triangulator.clear();
//
//        wallobj = physics2::PolygonObstacle::allocWithAnchor(wall,Vec2::ANCHOR_CENTER);
//        // You cannot add constant "".  Must stringify
//        wallobj->setName(std::string(WALL_NAME)+strtool::to_string(ii));
//        wallobj->setName(wname);
//
//        // Set the physics attributes
//        wallobj->setBodyType(b2_staticBody);
//        wallobj->setDensity(BASIC_DENSITY);
//        wallobj->setFriction(BASIC_FRICTION);
//        wallobj->setRestitution(BASIC_RESTITUTION);
//        wallobj->setDebugColor(DEBUG_COLOR);
//
//        wall *= _scale;
//        sprite = scene2::PolygonNode::allocWithTexture(image,wall);
//        addObstacle(wallobj,sprite,1);  // All walls share the same texture
//    }

// #pragma mark : Platforms
//     for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
//         std::shared_ptr<physics2::PolygonObstacle> platobj;
//         Poly2 platform(reinterpret_cast<Vec2*>(PLATFORMS[ii]),4);
//
//         EarclipTriangulator triangulator;
//         triangulator.set(platform.vertices);
//         triangulator.calculate();
//         platform.setIndices(triangulator.getTriangulation());
//         triangulator.clear();
//
//         platobj = physics2::PolygonObstacle::allocWithAnchor(platform,Vec2::ANCHOR_CENTER);
//         // You cannot add constant "".  Must stringify
//         platobj->setName(std::string(PLATFORM_NAME)+strtool::to_string(ii));
//
//         // Set the physics attributes
//         platobj->setBodyType(b2_staticBody);
//         platobj->setDensity(BASIC_DENSITY);
//         platobj->setFriction(BASIC_FRICTION);
//         platobj->setRestitution(BASIC_RESTITUTION);
//         platobj->setDebugColor(DEBUG_COLOR);
//
//         platform *= _scale;
//         sprite = scene2::PolygonNode::allocWithTexture(image,platform);
//         addObstacle(platobj,sprite,1);
//     }
#pragma mark : Wind
//    createWindObstacle(Vec2(2.5, 1.5), Size(1, 1), Vec2(0, 10));

    shared_ptr<LevelModel> level = make_shared<LevelModel>();

    // THIS WILL GENERATE A JSON LEVEL FILE. This is how to do it:
    //
   // level->createJsonFromLevel("json/test2.json", Size(32, 32), _objects);
    std::string key;
    vector<shared_ptr<Object>> levelObjs = level->createLevelFromJson("json/test2.json");
    for (auto it = levelObjs.begin(); it != levelObjs.end(); ++it) {
        key = (*it)->getJsonKey();
        if (key == "platforms") {
            createPlatform(dynamic_pointer_cast<Platform>(*it));
        }
        else if (key == "spikes") {
            createSpike(dynamic_pointer_cast<Spike>(*it));
        }
        else if (key == "treasures") {
            createTreasure(dynamic_pointer_cast<Treasure>(*it));
        }
        else if (key == "windObstacles") {
            createWindObstacle(dynamic_pointer_cast<WindObstacle>(*it));
        }
    }
    //level->createJsonFromLevel("level2ndTest.json", level->getLevelSize(), theObjects);
#pragma mark : Dude

    Vec2 dudePos = DUDE_POS;
    std::shared_ptr<scene2::SceneNode> node = scene2::SceneNode::alloc();
    image = _assets->get<Texture>(DUDE_TEXTURE);
    _avatar = DudeModel::alloc(dudePos, image->getSize() / _scale, _scale);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    _avatar->setSceneNode(sprite);
    _avatar->setDebugColor(DEBUG_COLOR);
    addObstacle(_avatar, sprite); // Put this at the very front

#pragma mark : Spikes
    /*createSpike(Vec2(13, 1), Size(1, 1), _scale);
    createSpike(Vec2(14, 1), Size(1, 1), _scale);
    createSpike(Vec2(8, 8), Size(1, 1), _scale, CU_MATH_DEG_TO_RAD(180));
    createSpike(Vec2(9, 8), Size(1, 1), _scale, CU_MATH_DEG_TO_RAD(180));
    createSpike(Vec2(10, 8), Size(1, 1), _scale, CU_MATH_DEG_TO_RAD(180));
    createSpike(Vec2(17, 4), Size(1, 1), _scale);
    createSpike(Vec2(18, 4), Size(1, 1), _scale);
    createSpike(Vec2(16, 3), Size(1, 1), _scale, CU_MATH_DEG_TO_RAD(90));
    createSpike(Vec2(3, 8), Size(1, 1), _scale, CU_MATH_DEG_TO_RAD(180));
    createSpike(Vec2(5, 5), Size(1, 1), _scale, CU_MATH_DEG_TO_RAD(270));
    
#pragma mark : Walls
    createPlatform(Vec2(0, 0), Size(6, 1), true);
    createPlatform(Vec2(13, 0), Size(7, 1), true);
    createPlatform(Vec2(19, 1), Size(1, 9), true);
    createPlatform(Vec2(0, 1), Size(1, 9), true);
    createPlatform(Vec2(1, 9), Size(18, 1), true);
    createPlatform(Vec2(1, 9), Size(18, 1), true);
    createPlatform(Vec2(17, 3), Size(2, 1), true);
    createPlatform(Vec2(1, 9), Size(18, 1), true);
    createPlatform(Vec2(3, 5), Size(2, 1), true);*/

    //level->createJsonFromLevel("json/test2.json", Size(32, 32), _objects);
    
    // KEEP TO REMEMBER HOW TO MAKE MOVING PLATFORM
    //    createMovingPlatform(Vec2(3, 4), Sizef(2, 1), Vec2(8, 4), 1.0f);

#pragma mark : Treasure

    createTreasure(Vec2(TREASURE_POS[0]), Size(1, 1), "default");


    // Play the background music on a loop.
    // TODO: Uncomment for music
    //    std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
    //    AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);
}

/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is to make a subclass of a physics object, like we did
 * with dude.  The other is to use callback functions to loosely couple
 * the two.  This function is an example of the latter.
 *
 * @param obj             The physics object to add
 * @param node            The scene graph node to attach it to
 * @param zOrder          The drawing order
 * @param useObjPosition  Whether to update the node's position to be at the object's position
 */
void GameScene::addObstacle(const std::shared_ptr<physics2::Obstacle> &obj,
                            const std::shared_ptr<scene2::SceneNode> &node,
                            bool useObjPosition)
{
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);

    // Position the scene graph node (enough for static objects)
    if (useObjPosition)
    {
        node->setPosition(obj->getPosition() * _scale);
    }
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() != b2_staticBody)
    {
        scene2::SceneNode *weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=, this](physics2::Obstacle *obs) {
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle()); });
    }
}

#pragma mark -
#pragma mark Physics Handling
/**
 * The method called to update the game mode.
 *
 * This is the nondeterministic version of a physics simulation. It is
 * provided for comparison purposes only.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep)
{
    _input.update(timestep);

    if (_buildingMode)
    {
        // Removed because duplicate code in preupdate causes issues
    }
    else
    {
        // Process the toggled key commands
        if (_input.didDebug())
        {
            setDebug(!isDebug());
        }
        if (_input.didReset())
        {
            reset();
        }
        if (_input.didExit())
        {
            CULog("Shutting down");
            Application::get()->quit();
        }

        // Process the movement
        if (_input.withJoystick())
        {
            if (_input.getHorizontal() < 0)
            {
                _leftnode->setVisible(true);
                _rightnode->setVisible(false);
            }
            else if (_input.getHorizontal() > 0)
            {
                _leftnode->setVisible(false);
                _rightnode->setVisible(true);
            }
            else
            {
                _leftnode->setVisible(false);
                _rightnode->setVisible(false);
            }
            _leftnode->setPosition(_input.getJoystick());
            _rightnode->setPosition(_input.getJoystick());
        }
        else
        {
            _leftnode->setVisible(false);
            _rightnode->setVisible(false);
        }

        _avatar->setMovement(_input.getHorizontal() * _avatar->getForce());
        _avatar->setJumping(_input.didJump());
        _avatar->applyForce();

        if (_avatar->isJumping() && _avatar->isGrounded())
        {

            std::shared_ptr<Sound> source = _assets->get<Sound>(JUMP_EFFECT);
            AudioEngine::get()->play(JUMP_EFFECT, source, false, EFFECT_VOLUME);
        }

        if (_avatar->isGrounded())
        {
            _input.setGlide(false);
        }
        /**Checks if we are gliding, by seeing if we are out of a jump and if we are holding down the right side of the screen.*/
        if (_input.isRightDown() && _input.canGlide())
        {

            _avatar->setGlide(true);
        }
        else
        {
            _avatar->setGlide(false);
        }
    }

    for (auto &obj : _objects)
    {
        obj->update(timestep);
    }


    // Turn the physics engine crank.

    _world->update(timestep);
    _ui.update(timestep);
    
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
void GameScene::preUpdate(float dt)
{
    _input.update(dt);

    if (_buildingMode)
    {
        /** The offset of finger placement to object indicator */
        Vec2 dragOffset = Vec2(-1, 1);

        // Deactivate inventory buttons once all traps are placed
        if (_itemsPlaced == 0){
            for (size_t i = 0; i < _inventoryButtons.size(); i++)
            {
                _inventoryButtons[i]->activate();
                _inventoryOverlay->setVisible(false);
            }
        }
        
        if (_input.isTouchDown() && (_input.getInventoryStatus() == PlatformInput::PLACING))
        {
            Vec2 screenPos = _input.getPosOnDrag();
            Vec2 gridPos = snapToGrid(convertScreenToBox2d(screenPos, _scale, _offset), NONE);
            Vec2 gridPosWithOffset = snapToGrid(convertScreenToBox2d(screenPos, _scale, _offset) + dragOffset, _selectedItem);

            // Show placing object indicator when dragging object
            if (_selectedItem != NONE) {
                CULog("Placing object");

                if (_selectedObject) {
                    // Set the current position of the object
                    _prevPos = gridPos;

                    // Move the existing object to new position
                    _selectedObject->setPosition(gridPosWithOffset);

                    // Trigger obstacle update listener
                    if (_selectedObject->getObstacle()->getListener()) {
                        _selectedObject->getObstacle()->getListener()(_selectedObject->getObstacle().get());
                    }
                } else {
                    _gridManager->setObject(gridPosWithOffset, _selectedItem);
                }
            }
        }
        else if (_input.getInventoryStatus() == PlatformInput::WAITING)
        {
            _gridManager->setSpriteInvisible();

            if (_input.isTouchDown()) {
                // Attempt to move object that exists on the grid
                Vec2 screenPos = _input.getPosOnDrag();
                Vec2 gridPos = snapToGrid(convertScreenToBox2d(screenPos, _scale, _offset), NONE);

                std::shared_ptr<Object> obj = _gridManager->removeObject(gridPos);
                
                // If object exists
                if (obj) {
                    CULog("Selected existing object");
                    _selectedObject = obj;
                    _selectedItem = obj->getItemType();

                    _gridManager->removeObject(gridPos);
                    _input.setInventoryStatus(PlatformInput::PLACING);
                }
            }
        }
        else if (_input.getInventoryStatus() == PlatformInput::PLACED)
        {
            Vec2 screenPos = _input.getPosOnDrag();
            Vec2 gridPos = snapToGrid(convertScreenToBox2d(screenPos, _scale, _offset) + dragOffset, _selectedItem);;

            if (_selectedObject) {
                if (_gridManager->hasObject(gridPos)) {
                    // Move the object back to its original position
                    _selectedObject->setPosition(_prevPos);
                    _gridManager->addObject(_prevPos, _selectedObject);

                    _prevPos = Vec2(0, 0);
                } else {
                    // Move the existing object to new position
                    _selectedObject->setPosition(gridPos);
                    _gridManager->addObject(gridPos, _selectedObject);
                }

                // Trigger listener
                if (_selectedObject->getObstacle()->getListener()) {
                    _selectedObject->getObstacle()->getListener()(_selectedObject->getObstacle().get());
                }

                // Reset selected object
                _selectedObject = nullptr;
            } else {
                // Place new object on grid
                Vec2 gridPos = snapToGrid(convertScreenToBox2d(screenPos, _scale, _offset) + dragOffset, _selectedItem);;

                if (!_gridManager->hasObject(gridPos)) {
                    std::shared_ptr<Object> obj = placeItem(gridPos, _selectedItem);
                    _gridManager->addObject(gridPos, obj);

                    _itemsPlaced += 1;

                    // Update inventory UI
                    if (_itemsPlaced >= 1)
                    {
                        for (size_t i = 0; i < _inventoryButtons.size(); i++)
                        {
                            _inventoryButtons[i]->deactivate();
                        }
                    }
                }
            }

            // Reset selected item
            _selectedItem = NONE;

            // Darken inventory UI
            _inventoryOverlay->setVisible(true);
            _input.setInventoryStatus(PlatformInput::WAITING);
        }

    }
    else
    {
        // Process the toggled key commands
        if (_input.didDebug())
        {
            setDebug(!isDebug());
        }
        if (_input.didReset())
        {
            reset();
        }
        if (_input.didExit())
        {
            CULog("Shutting down");
            Application::get()->quit();
        }

        // Process the movement
        if (_input.withJoystick())
        {
            if (_input.getHorizontal() < 0)
            {
                _leftnode->setVisible(true);
                _rightnode->setVisible(false);
            }
            else if (_input.getHorizontal() > 0)
            {
                _leftnode->setVisible(false);
                _rightnode->setVisible(true);
            }
            else
            {
                _leftnode->setVisible(false);
                _rightnode->setVisible(false);
            }
            _leftnode->setPosition(_input.getJoystick());
            _rightnode->setPosition(_input.getJoystick());
        }
        else
        {
            _leftnode->setVisible(false);
            _rightnode->setVisible(false);
        }

        //THE GLIDE BULLSHIT SECTION
        if (_input.getRightTapped()) {
            _input.setRightTapped(false);
            if (!_avatar->isGrounded())
            {
                _avatar->setGlide(true);
            }
        }
        else if (!_input.isRightDown()) {
            _avatar->setGlide(false);

        }

        _avatar->setMovement(_input.getHorizontal() * _avatar->getForce());
        _avatar->setJumping(_input.didJump());
        _avatar->applyForce();

        if (_avatar->isJumping() && _avatar->isGrounded())
        {

            std::shared_ptr<Sound> source = _assets->get<Sound>(JUMP_EFFECT);
            AudioEngine::get()->play(JUMP_EFFECT, source, false, EFFECT_VOLUME);
        }
        
        
        for (auto it = _objects.begin(); it != _objects.end(); ++it) {
            (*it)->update(dt);
        }
    }

    // TODO: Commented out camera code for now
    if (!_buildingMode){
        getCamera()->setPosition(Vec3(_avatar->getPosition().x * 51 + SCENE_WIDTH / 3.0f, getCamera()->getPosition().y, 0));
    }
    getCamera()->update();
    
    for (auto it = _objects.begin(); it != _objects.end(); ++it) {
        (*it)->update(dt);
    }
    // increase growing wall
    if (!_buildingMode)
    {
//        updateGrowingWall(dt);
    }

    _ui.preUpdate(dt);
    if (_ui.getReadyPressed()){
        setBuildingMode(false);
    }
    if (_ui.getRightPressed() && _buildingMode){
        getCamera()->translate(10, 0);
        getCamera()->update();
    }
    if (_ui.getLeftPressed() && _buildingMode){
        getCamera()->translate(-10, 0);
        getCamera()->update();
    }

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
void GameScene::fixedUpdate(float step)
{
    // Turn the physics engine crank.
    if (!_buildingMode){
        _world->update(step);
    }
    _ui.fixedUpdate(step);
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
void GameScene::postUpdate(float remain)
{
    // Since items may be deleted, garbage collect
    _world->garbageCollect();

    // Record failure if necessary.
    if (!_failed && _avatar->getY() < 0)
    {
        setFailure(true);
    }
    
    if (!_failed && _died){
        setFailure(true);
    }
    
    if(_reachedGoal){
        nextRound(true);
    }

    // Reset the game if we win or lose.
    if (_countdown > 0)
    {
        _countdown--;
    }
    else if (_countdown == 0)
    {
        reset();
    }
    _ui.postUpdate(remain);
}

/**
 * Sets whether the level is completed.
 *
 * If true, the level will advance after a countdown
 *
 * @param value whether the level is completed.
 */
void GameScene::setComplete(bool value)
{
    bool change = _complete != value;
    _complete = value;
    if (value && change)
    {
        std::shared_ptr<Sound> source = _assets->get<Sound>(WIN_MUSIC);
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
        _winnode->setVisible(true);
        _countdown = EXIT_COUNT;
    }
    else if (!value)
    {
        _winnode->setVisible(false);
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
void GameScene::setFailure(bool value) {
    if (value) {
        // If next round available, do not fail
        if (_currRound < TOTAL_ROUNDS){
            if (_avatar->_hasTreasure){
                _treasure->setPosition(Vec2(TREASURE_POS[_currGems]));
            }
            
            nextRound();
            return;
        }
        
        std::shared_ptr<Sound> source = _assets->get<Sound>(LOSE_MUSIC);
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
        _losenode->setVisible(true);
        _countdown = EXIT_COUNT;
    }
    else
    {
        _losenode->setVisible(false);
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
void GameScene::nextRound(bool reachedGoal) {
    // Check if player won before going to next round
    if (reachedGoal){
        if(_avatar->_hasTreasure){
            _avatar->removeTreasure();
            // Increment total treasure collected
            _currGems += 1;
            // Update score image
            _scoreImages.at(_currGems-1)->setTexture(_assets->get<Texture>(FULL_IMAGE));
            
            // Check if player won
            if (_currGems == TOTAL_GEMS){
                setComplete(true);
                return;
            }
            else{
                // Set up next treasure if collected in prev round
                _treasure->setPosition(Vec2(TREASURE_POS[_currGems]));
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
//   _roundsnode->setText("Round: " + std::to_string(_currRound) + "/" + std::to_string(TOTAL_ROUNDS));
    _ui.updateRound(_currRound, TOTAL_ROUNDS);

    // FIND BETTER SOLUTION LATER
//    removeChild(_roundsnode);
//    _roundsnode = scene2::Label::allocWithText("Round: " + std::to_string(_currRound) + "/" + std::to_string(TOTAL_ROUNDS), _assets->get<Font>(INFO_FONT));
//    _roundsnode->setAnchor(Vec2::ANCHOR_CENTER);
//    _roundsnode->setPosition(_size.width * .75,_size.height * .9);
//    _roundsnode->setForeground(INFO_COLOR);
//    _roundsnode->setVisible(true);
//    addChild(_roundsnode);
    
    setFailure(false);
    
    // Reset player properties
    _avatar->setPosition(Vec2(DUDE_POS));
    _avatar->removeTreasure();
    _died = false;
    _reachedGoal = false;
    
    // Reset growing wall
//    _growingWallWidth = 0.1f;
//    _growingWallNode->setVisible(false);

    
    // Return to building mode
    //_readyButton->setVisible(true);
    _itemsPlaced = 0;
    setBuildingMode(true);

    _ui.visibleButtons();
    
    
}

/**
 * Sets whether mode is in building or play mode.
 *
 * @param value whether the level is in building mode.
 */
void GameScene::setBuildingMode(bool value) {
    _buildingMode = value;

    _gridManager->getGridNode()->setVisible(value);
    for (size_t i = 0; i < _inventoryButtons.size(); i++)
    {
        _inventoryButtons[i]->setVisible(value);
    }
    _inventoryOverlay->setVisible(value);
    _inventoryBackground->setVisible(value);

    _camera->setPosition(_camerapos);
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
void GameScene::beginContact(b2Contact *contact)
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

    // See if we have landed on the ground.
    if (((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2)) && (bd2->getName() != "gust" && bd1->getName() != "gust"))
    {

        _avatar->setGrounded(true);
        // Could have more than one ground
        _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    }

    // If we hit the "win" door, we are done
    if((bd1 == _avatar.get()   && bd2 == _goalDoor.get()) ||
        (bd1 == _goalDoor.get() && bd2 == _avatar.get())) {
        _reachedGoal = true;
        
    }
    // If we hit a spike, we are DEAD
    if ((bd1 == _avatar.get() && bd2->getName() == "spike") ||
        (bd1->getName() == "spike" && bd2 == _avatar.get())) {
        //        setFailure(true);
        _died = true;
    }


    // If the player collides with the growing wall, game over
//    if ((bd1 == _avatar.get() && bd2 == _growingWall.get()) ||
//        (bd1 == _growingWall.get() && bd2 == _avatar.get()))
//    {
//        _died = true;
//
//    }

    if ((bd1 == _avatar.get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _avatar.get()))
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
             _avatar->addWind(thing->getTrajectory());
        }
    }

    if ((bd1 == _avatar.get() && bd2->getName() == "movingPlatform" && _avatar->isGrounded()) ||
        (bd2 == _avatar.get() && bd1->getName() == "movingPlatform" && _avatar->isGrounded()))
    {
        CULog("moving platform");
        _avatar->setOnMovingPlat(true);
        _avatar->setMovingPlat(bd1 == _avatar.get() ? bd2 : bd1);

        // If we hit a spike, we are DEAD
        if ((bd1 == _avatar.get() && bd2->getName() == "spike") ||
            (bd1->getName() == "spike" && bd2 == _avatar.get()))
        {
            _died = true;
        }
    }

    // If we collide with a treasure, we pick it up
    if ((bd1 == _avatar.get() && bd2->getName() == "treasure") ||
        (bd1->getName() == "treasure" && bd2 == _avatar.get()))
    {
        if (!_avatar->_hasTreasure)
        {
            _avatar->gainTreasure(_treasure);
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
void GameScene::endContact(b2Contact *contact)
{
    b2Fixture *fix1 = contact->GetFixtureA();
    b2Fixture *fix2 = contact->GetFixtureB();

    b2Body *body1 = fix1->GetBody();
    b2Body *body2 = fix2->GetBody();

    std::string *fd1 = reinterpret_cast<std::string *>(fix1->GetUserData().pointer);
    std::string *fd2 = reinterpret_cast<std::string *>(fix2->GetUserData().pointer);

    physics2::Obstacle *bd1 = reinterpret_cast<physics2::Obstacle *>(body1->GetUserData().pointer);
    physics2::Obstacle *bd2 = reinterpret_cast<physics2::Obstacle *>(body2->GetUserData().pointer);

    if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2))
    {
        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty())
        {
            _avatar->setGrounded(false);
        }
    }

    if ((bd1 == _avatar.get() && bd2->getName() == "movingPlatform") ||
        (bd2 == _avatar.get() && bd1->getName() == "movingPlatform"))
    {
        CULog("disable movement platform");
        _avatar->setOnMovingPlat(false);
        _avatar->setMovingPlat(nullptr);
    }

    if ((bd1 == _avatar.get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _avatar.get()))
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
            _avatar->addWind(-(thing->getTrajectory()));
        }
    }
}

#pragma mark -
#pragma mark Helpers

/**
 * Converts from screen to Box2D coordinates.
 *
 * @return the Box2D position
 *
 * @param screenPos    The screen position
 * @param scale             The screen to world scale
 * @param offset           The offset of the scene to the world
 */
Vec2 GameScene::convertScreenToBox2d(const Vec2 &screenPos, float scale, const Vec2 &offset)
{
    Vec2 adjusted = screenPos - offset;

    // Adjust for camera position
    Vec2 worldPos = adjusted + (_camera->getPosition() - _camerapos);

    float xBox2D = worldPos.x / scale;
    float yBox2D = worldPos.y / scale;

    // Converts to the specific grid position
    int xGrid = xBox2D;
    int yGrid = yBox2D;

    return Vec2(xGrid, yGrid);
}

/**
 * Snaps the Box2D position to within the bounds of the build phase grid.
 *
 * @return the grid position
 *
 * @param screenPos    The screen position
 * @param item               The selected item being snapped to the grid
 */
Vec2 GameScene::snapToGrid(const Vec2 &gridPos, Item item) {
    Size offset = itemToSize(item) - Vec2(1, 1);

    int xGrid = gridPos.x;
    int yGrid = gridPos.y;

    // Snaps the placement to inside the grid
    int maxRows = _gridManager->getNumRows() - 1;
    int maxCols = _gridManager->getNumColumns() - 1;

    xGrid = xGrid < 0 ? 0 : xGrid;
    yGrid = yGrid < 0 ? 0 : yGrid;
    xGrid = xGrid + offset.width > maxCols ? maxCols - offset.width : xGrid;
    yGrid = yGrid + offset.height > maxRows ? maxRows - offset.height : yGrid;

    return Vec2(xGrid, yGrid);
}

void GameScene::setSpriteBatch(const shared_ptr<SpriteBatch> &batch) {
    Scene2::setSpriteBatch(batch);
    _ui.setSpriteBatch(batch);
}

void GameScene::render() {
    Scene2::render();
    _ui.render();
}

