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

#define FIXED_TIMESTEP_S 0.02f


/** The goal door position */
float GOAL_POS[] = { 31.0f, 6.0f };
/** The initial position of the dude */

float DUDE_POS[] = { 1.0f, 3.0f};

/** The initial position of the treasure */
float TREASURE_POS[3][2] = { {14.5f, 7.5f}, {3.5f, 7.5f}, {9.5f, 1.5f}};





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
    _localPlayer(nullptr),
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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets, std::shared_ptr<NetworkController> networkController)
{
    return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), networkController);
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
                     const Rect &rect, const Vec2 &gravity, const std::shared_ptr<NetworkController> networkController)
{
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }
    
    _assets = assets;
    _networkController = networkController;
    
    // Networked physics world
    _world = physics2::distrib::NetWorld::alloc(rect,gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact *contact)
    {
        beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact *contact)
    {
        endContact(contact);
    };
    _world->update(FIXED_TIMESTEP_S);
    
    //TODO: Maybe move to NetworkController
    //Make a std::function reference of the linkSceneToObs function in game scene for network controller
    std::function<void(const std::shared_ptr<physics2::Obstacle>&,const std::shared_ptr<scene2::SceneNode>&)> linkSceneToObsFunc = [=,this](const std::shared_ptr<physics2::Obstacle>& obs, const std::shared_ptr<scene2::SceneNode>& node) {
        this->linkSceneToObs(obs,node);
    };
    
    // Init networking
    _network = networkController->getNetwork();
    
    //TODO: Change this to all be handled in Network Controller
    _network->enablePhysics(_world, linkSceneToObsFunc);
    
    // Set _world and _objects in networkController
    _networkController->setObjects(&_objects);
    _networkController->setWorld(_world);

    // Start in building mode
    _buildingMode = true;

    // Start up the input handler
    
    _input.init(getBounds());

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = _size.width == SCENE_WIDTH ? _size.width / rect.size.width : _size.height / rect.size.height;
    
    Vec2 offset = Vec2((_size.width - SCENE_WIDTH) / 2.0f, (_size.height - SCENE_HEIGHT) / 2.0f);
    _offset = offset;
    
    

    _backgroundScene.init();

    // TODO: Bring back background
    _background = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BACKGROUND_TEXTURE));
    _background->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _background->setPosition(Vec2(0,0));
    _background->setScale(2.1f);
    _backgroundScene.addChild(_background);

    // Create the scene graph
    std::shared_ptr<Texture> image;
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    //initialize object controller
    _objectController = std::make_shared<ObjectController>(_assets, _world, _scale, _worldnode, _debugnode, &_objects);

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

    std::shared_ptr<scene2::PolygonNode> jumpNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(JUMP_BUTTON));
    _jumpbutton = scene2::Button::alloc(jumpNode);
    _jumpbutton->setAnchor(Vec2::ANCHOR_CENTER);
    _jumpbutton->setPosition(_size.width * 0.85f, _size.height * 0.25f);
    _jumpbutton->setVisible(false);
    _jumpbutton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _didjump = true;
        }
        else{
            _didjump = false;
        }
    });

    std::shared_ptr<scene2::PolygonNode> glideNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(GLIDE_BUTTON));
    _glidebutton = scene2::Button::alloc(glideNode);
    _glidebutton->setAnchor(Vec2::ANCHOR_CENTER);
    _glidebutton->setPosition(_size.width * 0.85f, _size.height * 0.25f);
    _glidebutton->setVisible(false);
    _glidebutton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _didglide = true;
        }
        else{
            _didglide = false;
        }
    });


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
    addChild(_leftnode);
    addChild(_rightnode);
    addChild(_scrollpane);
    addChild(_gridManager->getGridNode());
    _ui.addChild(_jumpbutton);
    _ui.addChild(_glidebutton);
    
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
        _backgroundScene.dispose();
        _world = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winnode = nullptr;
        _losenode = nullptr;
        _leftnode = nullptr;
        _rightnode = nullptr;
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
    std::vector<std::string> assetNames = {LOG_TEXTURE, GLIDING_LOG_TEXTURE, WIND_TEXTURE};

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
        case (PLATFORM): {
            return _networkController->createPlatformNetworked(gridPos, Size(3,1), "log", _scale);
        }
        case (MOVING_PLATFORM):
            return _networkController->createMovingPlatformNetworked(gridPos, Size(3, 1), gridPos + Vec2(3, 0), 1, _scale);
        case (WIND):
            return _objectController->createWindObstacle(gridPos, Size(1, 1), Vec2(0, 1.0), "default");
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
    // Reset all controllers --> not sure if necessary
    _networkController->reset();
    
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    _localPlayer = nullptr;
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

    _goalDoor = _objectController->createGoalDoor(Vec2(GOAL_POS[0], GOAL_POS[1]));

#pragma mark : Wind

    shared_ptr<LevelModel> level = make_shared<LevelModel>();

    // THIS WILL GENERATE A JSON LEVEL FILE. This is how to do it:
    //
   // level->createJsonFromLevel("json/test2.json", Size(32, 32), _objects);
    std::string key;
    vector<shared_ptr<Object>> levelObjs = level->createLevelFromJson("json/test2.json");
    for (auto& obj : levelObjs) {
        _objectController->processLevelObject(obj);
    }
    //level->createJsonFromLevel("level2ndTest.json", level->getLevelSize(), theObjects);
#pragma mark : Dude
    std::shared_ptr<scene2::SceneNode> node = scene2::SceneNode::alloc();
    std::shared_ptr<Texture> image = _assets->get<Texture>(DUDE_TEXTURE);
    
    // HOST STARTS ON LEFT
    Vec2 pos = DUDE_POS;
//    pos += Vec2(4, 5);
    // CLIENT STARTS ON RIGHT
    if (_networkController->getLocalID() == 2){
        pos += Vec2(2, 0);
    }
    
    _localPlayer = _networkController->createPlayerNetworked(pos, _scale);
    // This is set to false to counter race condition with collision filtering
    // NetworkController sets this back to true once it sets collision filtering to all players
    // There is a race condition where players are colliding when they spawn in, causing a player to get pushed into the void
    // If I do not disable the player, collision filtering works after build phase ends, not sure why
    // TODO: Find a better solution, maybe only have players getting updated during movement phase
    _localPlayer->setEnabled(false);
    
    _localPlayer->setDebugScene(_debugnode);
    
    _world->getOwnedObstacles().insert({_localPlayer,0});
    if (!_networkController->getIsHost()){
        _network->getPhysController()->acquireObs(_localPlayer, 0);
    }


#pragma mark : Treasure

    _objectController->createTreasure(Vec2(TREASURE_POS[0]), Size(1, 1), "default");


    // Play the background music on a loop.
    // TODO: Uncomment for music
    //    std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
    //    AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);
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
    //TODO: Remove later
//    if (_buildingMode){
//        _localPlayer->setAwake(false);
//    }
//    else{
//        _localPlayer->setAwake(true);
//    }
    
    _networkController->preUpdate(dt);
    
    
    _input.update(dt);

    
    // Process Networking
    if (_buildingMode && (_networkController->getNumReady() >= _network->getNumPlayers())){
        // Exit build mode and switch to movement phase
        setBuildingMode(!_buildingMode);
        _networkController->setNumReady(0);
    }
    
    // Update objects
    for (auto it = _objects.begin(); it != _objects.end(); ++it) {
        (*it)->update(dt);
    }
    
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
                    CULog("Reposition object");
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
            if (!_localPlayer->isGrounded())
            {
                _localPlayer->setGlide(true);
            }
        }
        else if (!_input.isRightDown()) {
            _localPlayer->setGlide(false);
        }

//        if (_input.getRightTapped()) {
//            _input.setRightTapped(false);
//            if (!_avatar->isGrounded())
//            {
//                _avatar->setGlide(true);
//            }
//        }
//        else if (!_input.isRightDown()) {
//            _avatar->setGlide(false);
//
//        }
        _localPlayer->setGlide(_didglide);


        _localPlayer->setMovement(_input.getHorizontal() * _localPlayer->getForce());
        _localPlayer->setJumping(_didjump);
        _localPlayer->applyForce();


        if (_localPlayer->isJumping() && _localPlayer->isGrounded())
        {

            std::shared_ptr<Sound> source = _assets->get<Sound>(JUMP_EFFECT);
            AudioEngine::get()->play(JUMP_EFFECT, source, false, EFFECT_VOLUME);
        }
        
        
        for (auto it = _objects.begin(); it != _objects.end(); ++it) {
            (*it)->update(dt);
        }


        if (_localPlayer->isGrounded() && !_glidebutton->isDown()){
            _jumpbutton->activate();
            _jumpbutton->setVisible(true);
            _glidebutton->deactivate();
            _glidebutton->setVisible(false);
            _didglide = false;
        }
        else if (!_localPlayer->isGrounded() && !_jumpbutton->isDown()){
            _jumpbutton->deactivate();
            _jumpbutton->setVisible(false);
            _glidebutton->activate();
            _glidebutton->setVisible(true);
            _didjump = false;
        }

    }

    // TODO: Commented out camera code for now
    if (!_buildingMode){
        getCamera()->setPosition(Vec3(_localPlayer->getPosition().x * 51 + SCENE_WIDTH / 3.0f, getCamera()->getPosition().y, 0));
    }
    getCamera()->update();
    
//    for (auto it = _objects.begin(); it != _objects.end(); ++it) {
//        (*it)->update(dt);
//    }
    // increase growing wall
    if (!_buildingMode)
    {
//        updateGrowingWall(dt);
    }

    _ui.preUpdate(dt);
    if (_ui.getReadyDone() && !_readyMessageSent){
//        CULog("send out event");
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::BUILD_READY));
        _readyMessageSent = true;
    } else if (!_ui.getReadyDone()) {
        _readyMessageSent = false;
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
    _world->update(FIXED_TIMESTEP_S);
    
    // Update all controller
    _networkController->fixedUpdate(step);
    
    _ui.fixedUpdate(step);

    
    if(_network->isInAvailable()){
        auto e = _network->popInEvent();
        if(auto mEvent = std::dynamic_pointer_cast<MessageEvent>(e)){
            processMessageEvent(mEvent);
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
void GameScene::postUpdate(float remain)
{
    // Since items may be deleted, garbage collect
    _world->garbageCollect();
    
    // Update all controllers
    _networkController->fixedUpdate(remain);

    // Record failure if necessary.
    if (!_failed && _localPlayer->getY() < 0)
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
            if (_localPlayer->_hasTreasure){
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
        if(_localPlayer->_hasTreasure){
            _localPlayer->removeTreasure();
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
    _localPlayer->setPosition(Vec2(DUDE_POS));
    _localPlayer->removeTreasure();
    _died = false;
    _reachedGoal = false;
    
    // Reset growing wall
//    _growingWallWidth = 0.1f;
//    _growingWallNode->setVisible(false);

    
    // Return to building mode
    _itemsPlaced = 0;
    setBuildingMode(true);
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
    _ui.visibleButtons(value);

    _camera->setPosition(_camerapos);

    if (value){
        _jumpbutton->deactivate();
        _glidebutton->deactivate();
        _glidebutton->setVisible(false);
        _ui.setReadyDone(false);
    }
    else{
        _jumpbutton->activate();
    }
    _jumpbutton->setVisible(!value);
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

    
    // Check if both are players and disable contact
        if ((bd1->getName() == "player" && bd2->getName() == "player") ||
            (bd1 == _localPlayer.get() && bd2->getName() == "player") ||
            (bd2 == _localPlayer.get() && bd1->getName() == "player")) {
            contact->SetEnabled(false);
        }
    // See if we have landed on the ground.

    if (((_localPlayer->getSensorName() == fd2 && _localPlayer.get() != bd1) ||
        (_localPlayer->getSensorName() == fd1 && _localPlayer.get() != bd2)) && (bd2->getName() != "gust" && bd1->getName() != "gust"))
    {
        
        _localPlayer->setGrounded(true);
    }

//    if ((_localPlayer->getSensorName() == fd2 && _localPlayer.get() != bd1) ||
//        (_localPlayer->getSensorName() == fd1 && _localPlayer.get() != bd2))
//    {
//        _localPlayer->setGrounded(true);
//
//        // Could have more than one ground
//        _sensorFixtures.emplace(_localPlayer.get() == bd1 ? fix2 : fix1);
//    }
    
    if ((_localPlayer->getSensorName() == fd2 && _localPlayer.get() != bd1 && bd1->getName() != "player") ||
        (_localPlayer->getSensorName() == fd1 && _localPlayer.get() != bd2 && bd2->getName() != "player")) {
        _localPlayer->setGrounded(true);
        
        // Could have more than one ground
        _sensorFixtures.emplace(_localPlayer.get() == bd1 ? fix2 : fix1);
    }

    // If we hit the "win" door, we are done
    if((bd1 == _localPlayer.get()   && bd2 == _goalDoor.get()) ||
        (bd1 == _goalDoor.get() && bd2 == _localPlayer.get())) {
        _reachedGoal = true;
        
    }
    // If we hit a spike, we are DEAD
    if ((bd1 == _localPlayer.get() && bd2->getName() == "spike") ||
        (bd1->getName() == "spike" && bd2 == _localPlayer.get())) {
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

    if ((bd1 == _localPlayer.get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _localPlayer.get()))
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
            _localPlayer->addWind(thing->getTrajectory());
        }
    }
    
//    if ((bd1 == _localPlayer.get() && bd2 == _growingWall.get()) ||
//        (bd1 == _growingWall.get() && bd2 == _localPlayer.get()))
//    {
//        _died = true;
//
//    }

    if ((bd1 == _localPlayer.get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _localPlayer.get()))
    {
        // CULog("WIND");
        _localPlayer->addWind(Vec2(0, 6));
    }

    if ((bd1 == _localPlayer.get() && bd2->getName() == "movingPlatform" && _localPlayer->isGrounded()) ||
        (bd2 == _localPlayer.get() && bd1->getName() == "movingPlatform" && _localPlayer->isGrounded()))
    {
//        CULog("moving platform");
        _localPlayer->setOnMovingPlat(true);
        _localPlayer->setMovingPlat(bd1 == _localPlayer.get() ? bd2 : bd1);

        // If we hit a spike, we are DEAD
        if ((bd1 == _localPlayer.get() && bd2->getName() == "spike") ||
            (bd1->getName() == "spike" && bd2 == _localPlayer.get()))
        {
            _died = true;
        }
    }

    // If we collide with a treasure, we pick it up
    if ((bd1 == _localPlayer.get() && bd2->getName() == "treasure") ||
        (bd1->getName() == "treasure" && bd2 == _localPlayer.get()))
    {
        if (!_localPlayer->_hasTreasure)
        {
            _localPlayer->gainTreasure(_treasure);
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

    if ((_localPlayer->getSensorName() == fd2 && _localPlayer.get() != bd1) ||
        (_localPlayer->getSensorName() == fd1 && _localPlayer.get() != bd2))
    {
        _sensorFixtures.erase(_localPlayer.get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty())
        {
            _localPlayer->setGrounded(false);
        }
    }

    if ((bd1 == _localPlayer.get() && bd2->getName() == "movingPlatform") ||
        (bd2 == _localPlayer.get() && bd1->getName() == "movingPlatform"))
    {
//        CULog("disable movement platform");
        _localPlayer->setOnMovingPlat(false);
        _localPlayer->setMovingPlat(nullptr);
    }

    if ((bd1 == _localPlayer.get() && bd2->getName() == "gust") ||
        (bd1->getName() == "gust" && bd2 == _localPlayer.get()))
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
            _localPlayer->addWind(-(thing->getTrajectory()));
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


/**
 * This method takes a MessageEvent and processes it.
 */
void GameScene::processMessageEvent(const std::shared_ptr<MessageEvent>& event){
    Message message = event->getMesage();
    switch (message) {
            case Message::BUILD_READY:
                // Increment number of players ready
                // TODO: Find better way of handling
                _networkController->setNumReady(_networkController->getNumReady() + 1);
                break;
            
            default:
                // Handle unknown message types
                std::cout << "Unknown message type received" << std::endl;
                break;
        }
}


void GameScene::setSpriteBatch(const shared_ptr<SpriteBatch> &batch) {
    _backgroundScene.setSpriteBatch(batch);
    Scene2::setSpriteBatch(batch);
    _ui.setSpriteBatch(batch);
}

void GameScene::render() {
    _backgroundScene.render();
    Scene2::render();
    _ui.render();
}


void GameScene::linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj,
    const std::shared_ptr<scene2::SceneNode>& node) {

    node->setPosition(obj->getPosition() * _scale);
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
