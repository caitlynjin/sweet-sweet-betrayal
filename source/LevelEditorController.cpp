//
//  LevelEditorController.cpp
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/28/25.
//

#include "LevelEditorController.h"
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
#define SCENE_WIDTH 2048
#define SCENE_HEIGHT 1152

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0 / 16.0

/** The number pixels in a Box2D unit */
#define BOX2D_UNIT 64.0f

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH (SCENE_WIDTH / BOX2D_UNIT) * 2
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT (SCENE_HEIGHT / BOX2D_UNIT) * 1

#define FIXED_TIMESTEP_S 0.02f

/** The goal door position */
float GOAL_POSITION[] = { 39.0f, 3.0f };

#pragma mark -
#pragma mark Constructors
/**
 * Creates the controller.
 */
LevelEditorController::LevelEditorController() : Scene2(), _world(nullptr){}

bool LevelEditorController::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound, bool levelEditing)
{
    return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), networkController, sound, levelEditing);
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
bool LevelEditorController::init(const std::shared_ptr<AssetManager>& assets,
    const Rect& rect, const Vec2& gravity, const std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound, bool levelEditing)
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
    _network = networkController->getNetwork();
    _sound = sound;

    // Networked physics world
    _world = physics2::distrib::NetWorld::alloc(rect, gravity);
    _world->activateCollisionCallbacks(true);

    _world->update(FIXED_TIMESTEP_S);

    _world->onBeginContact = [this](b2Contact* contact)
        {
        };
    _world->onEndContact = [this](b2Contact* contact)
        {
        };
    // Start up the input handler
    _input = std::make_shared<PlatformInput>();
    _input->init(getBounds());

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = _size.width == SCENE_WIDTH ? _size.width / rect.size.width : _size.height / rect.size.height;
    _scale *= getSystemScale();
    Vec2 offset = Vec2((_size.width - SCENE_WIDTH) / 2.0f, (_size.height - SCENE_HEIGHT) / 2.0f);
    _offset = offset;

    // Initialize background
    _backgroundScene.init();
    _background = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BACKGROUND_TEXTURE));
    _background->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _background->setPosition(Vec2(0, 0));
    _background->setScale(2.1f);
    _backgroundScene.addChild(_background);

    _gridManager = GridManager::alloc(DEFAULT_HEIGHT, DEFAULT_WIDTH, _scale * 2, offset, assets);

    shared_ptr<scene2::OrderedNode> worldnode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    worldnode->setPosition(_offset);
    worldnode->setScale(worldnode->getScale() * 2);
    shared_ptr<scene2::SceneNode> _worldnode = dynamic_pointer_cast<scene2::SceneNode>(worldnode);
    addChild(_worldnode);

    shared_ptr<scene2::SceneNode> _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(_offset);
    //addChild(_debugnode);

    std::function<void(const std::shared_ptr<physics2::Obstacle>&, const std::shared_ptr<scene2::SceneNode>&)> linkSceneToObsFunc = [=, this](const std::shared_ptr<physics2::Obstacle>& obs, const std::shared_ptr<scene2::SceneNode>& node) {
        this->_levelEditorScene.linkSceneToObs(obs, node, _scale, &_worldnode);
    };
    // Initialize object controller
    
    _objectController = std::make_shared<ObjectController>(_assets, _world, _scale, _worldnode, _debugnode, &_objects);
    _camera = getCamera();
    addChild(_gridManager->getGridNode());

#pragma mark : Goal door
    _goalDoor = _objectController->createGoalDoor(Vec2(GOAL_POSITION[0], GOAL_POSITION[1]));

#pragma mark : Level
    shared_ptr<LevelModel> level = make_shared<LevelModel>();

    // THIS WILL GENERATE A JSON LEVEL FILE. This is how to do it:
    //
   // level->createJsonFromLevel("json/test2.json", Size(32, 32), _objects);
    std::string key;
    vector<shared_ptr<Object>> levelObjs = level->createLevelFromJson("json/test9.json");
    for (auto& obj : levelObjs) {
        _objectController->processLevelObject(obj, true);
        // This is necessary to remove the object with the eraser.
        // Also, in level editor, everything should be moveable.
        _gridManager->addMoveableObject(obj->getPosition(), obj);
        CULog("new object position: (%f, %f)", obj->getPosition().x, obj->getPosition().y);
    }


    // Initialize build phase controller
    initBuildingLogic(assets, _input, _gridManager, _objectController, _networkController, _camera);

    _active = true;
    Application::get()->setClearColor(Color4f::CORNFLOWER);

    return true;
}

/**
 * Initializes the controller contents.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool LevelEditorController::initBuildingLogic(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<ObjectController> objectController, std::shared_ptr<NetworkController> networkController, std::shared_ptr<Camera> camera) {
    
    if (assets == nullptr)
    {
        return false;
    }

    //_networkController = networkController;
    //_network = networkController->getNetwork();

    // Initialize build phase scene
    _levelEditorScene.init(assets, camera);

    // Initalize UI Scene
    _uiScene.init(assets, _gridManager);
    std::vector<Item> inventoryItems;
    std::vector<std::string> assetNames;
        
    inventoryItems = { PLATFORM, WIND, SPIKE, TREASURE, TILE_ALPHA, ART_OBJECT };
    assetNames = { LOG_TEXTURE, WIND_TEXTURE, SPIKE_TILE_TEXTURE, TREASURE_TEXTURE, TILE_TEXTURE, TILE_TEXTURE};

    _uiScene.initInventory(inventoryItems, assetNames);

    std::vector<std::shared_ptr<scene2::Button>> inventoryButtons = _uiScene.getInventoryButtons();
    for (size_t i = 0; i < inventoryButtons.size(); i++) {
        inventoryButtons[i]->addListener([this, item = inventoryItems[i]](const std::string& name, bool down) {
            if (down) {
                _selectedItem = item;
                _input->setInventoryStatus(PlatformInput::PLACING);
            }
            });
    }

    _uiScene.activateInventory(true);
    _uiScene.getSaveTextField()->activate();
    _uiScene.getLoadTextField()->activate();

    return true;
};

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelEditorController::dispose() {
    if (_active)
    {
        _world = nullptr;
        _gridManager->getGridNode() = nullptr;

        _input->dispose();
        _backgroundScene.dispose();
        _levelEditorScene.dispose();
        _uiScene.dispose();
        Scene2::dispose();
    }
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void LevelEditorController::reset() {
    _world->clear();
    _levelEditorScene.reset();
    _uiScene.reset();
    _itemsPlaced = 0;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void LevelEditorController::preUpdate(float dt) {
        _input->update(dt);

        /** The offset of finger placement to object indicator */
        Vec2 dragOffset = _input->getSystemDragOffset();

        if (!_input->isTouchDown() && !_uiScene.isPaintMode() && !_uiScene.isEraserMode() && _input->getInventoryStatus() == PlatformInput::PLACING) {
            _input->setInventoryStatus(PlatformInput::WAITING);
        }
        // TODO: add code to prevent save/load buttons/text fields from clicking an object down while in paintbrush mode
        // Also, same for clicking the paintbrush button itself
        if (_input->isTouchDown() && !_uiScene.isEraserMode() && (_input->getInventoryStatus() == PlatformInput::PLACING || (_uiScene.isPaintMode() && _selectedItem != Item::NONE && !_uiScene.getLeftPressed() && !_uiScene.getRightPressed())))
        {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_levelEditorScene.convertScreenToBox2d(screenPos, getSystemScale()), NONE);
            Vec2 gridPosWithOffset = snapToGrid(_levelEditorScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);

            // Show placing object indicator when dragging object
            if (_selectedItem != NONE) {

                if (_selectedObject) {
                    // Move the existing object to new position
                    _selectedObject->setPosition(gridPosWithOffset);

                    // Trigger obstacle update listener
                    if (_selectedObject->getObstacle()->getListener()) {
                        _selectedObject->getObstacle()->getListener()(_selectedObject->getObstacle().get());
                    }
                }
                else {
                    _gridManager->setObject(gridPosWithOffset, _selectedItem);
                }
            }
            if (_uiScene.isPaintMode() && (_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem), _selectedItem))) {
                std::shared_ptr<Object> obj = placeItem(gridPos, _selectedItem);
                // might go back to addObject() for levelEditor??? just keep this in mind
                _gridManager->addMoveableObject(gridPos, obj);

                _itemsPlaced += 1;
            }
        }
        else if (_uiScene.isEraserMode() && _input->isTouchDown()) {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_levelEditorScene.convertScreenToBox2d(screenPos + dragOffset, getSystemScale()), NONE);
            std::shared_ptr<Object> obj = _gridManager->moveObject(gridPos);
            // This REQUIRES the object to have been moveable when it got loaded in.
            // Otherwise, it FAILS.
            if (obj) {
                vector<shared_ptr<Object>> objs = *(_objectController->getObjects());
                CULog("length before erase: %d", (*(_objectController->getObjects())).size());
                auto it = (*(_objectController->getObjects())).begin();
                int index = 0;
                // This code erases an object from ObjectController, but not from the world.
                /*while (it != (*(_objectController->getObjects())).end()) {
                    if (**it == *obj) {
                        (*(_objectController->getObjects())).erase((*(_objectController->getObjects())).begin() + index);
                        CULog("%d", (*(_objectController->getObjects())).size());
                        break;
                    }
                    else {
                        CULog("%f", (**it).getPosition().x);
                        CULog("%f", (*obj).getPosition().x);
                        CULog("%f", (*obj).getPosition().y);
                        CULog("");
                    }
                    index++;
                    ++it;
                }*/

                /*This code doesn't really work as is but I left it in for now. */
                //auto it = objs.erase(find(objs.begin(), objs.end() - 1, obj));
                //CULog("length before erase: %d", (*(_objectController->getObjects())).size());
                obj->setPosition(Vec2(2, 2));
                if (obj->getObstacle()->getListener()) {
                    obj->getObstacle()->getListener()(obj->getObstacle().get());
                }
            }
        }
        else if (_input->getInventoryStatus() == PlatformInput::WAITING)
        {
            _gridManager->setSpriteInvisible();

            if (_input->isTouchDown()) {
                // Attempt to move object that exists on the grid
                Vec2 screenPos = _input->getPosOnDrag();
                Vec2 gridPos = snapToGrid(_levelEditorScene.convertScreenToBox2d(screenPos, getSystemScale()), NONE);

                std::shared_ptr<Object> obj = _gridManager->moveObject(gridPos);

                // If object exists
                if (obj) {
                    CULog("Selected existing object");
                    _selectedObject = obj;
                    _selectedItem = obj->getItemType();

                    // Set the current position of the object
                    _prevPos = _selectedObject->getPosition();

                    _input->setInventoryStatus(PlatformInput::PLACING);
                }
            }
        }
        else if (_input->getInventoryStatus() == PlatformInput::PLACED && !_uiScene.isPaintMode() && !_uiScene.isEraserMode())
        {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_levelEditorScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);;
            if (_selectedObject) {
                if (!_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem), _selectedItem)) {
                    // Move the object back to its original position
                    _selectedObject->setPosition(_prevPos);
                    _gridManager->addMoveableObject(_prevPos, _selectedObject);
                    _prevPos = Vec2(0, 0);
                }
                else {
                    // Move the existing object to new position
                    CULog("Reposition object");
                    _selectedObject->setPosition(gridPos);
                    if (_selectedObject->getItemType() == Item::PLATFORM) {
                        auto platform = std::dynamic_pointer_cast<Platform>(_selectedObject);
                        if (platform) {
                            platform->updateMoving(gridPos);
                        }
                    }
                    _gridManager->addMoveableObject(gridPos, _selectedObject);
                }

                // Trigger listener
                if (_selectedObject->getObstacle()->getListener()) {
                    _selectedObject->getObstacle()->getListener()(_selectedObject->getObstacle().get());
                }

                // Reset selected object
                _selectedObject = nullptr;
            }
            else {
                // Place new object on grid
                Vec2 gridPos = snapToGrid(_levelEditorScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);;

                if (_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem), _selectedItem)) {
                    std::shared_ptr<Object> obj = placeItem(gridPos, _selectedItem);
                    // might go back to addObject() for levelEditor??? just keep this in mind
                    _gridManager->addMoveableObject(gridPos, obj);

                    _itemsPlaced += 1;
                }
            }

            // Reset selected item
            _selectedItem = NONE;
            _selectedObject = nullptr;

            // Darken inventory UI
            _input->setInventoryStatus(PlatformInput::WAITING);
        }

    _levelEditorScene.preUpdate(dt);

    //CULog("%f", _buildPhaseScene.getCamera()->getPosition().x);

    if (_uiScene.getRightPressed() && _levelEditorScene.getCamera()->getPosition().x <= 2240) {
        _levelEditorScene.getCamera()->translate(10, 0);
        _levelEditorScene.getCamera()->update();
    }
    if (_uiScene.getLeftPressed() && _levelEditorScene.getCamera()->getPosition().x >= 0) {
        _levelEditorScene.getCamera()->translate(-10, 0);
        _levelEditorScene.getCamera()->update();
    }

    else if (_uiScene.getIsReady()) {
        // Save the level to a file
        shared_ptr<LevelModel> level = make_shared<LevelModel>();
        level->createJsonFromLevel("json/" + _uiScene.getSaveFileName() + ".json", Size(100, 100), _objectController->getObjects());
        _uiScene.setIsReady(false);
    }

    if (_uiScene.getLoadClicked()) {
        // Load the level stored in this file

        // TODO: (maybe): save the shared_ptr<LevelModel> somewhere more efficiently
        _objectController->getObjects()->clear();
        shared_ptr<LevelModel> level = make_shared<LevelModel>();
        vector<shared_ptr<Object>> objects = level->createLevelFromJson("json/" + _uiScene.getLoadFileName() + ".json");
        for (auto& obj : objects) {
            _objectController->processLevelObject(obj, true);
            _gridManager->addMoveableObject(obj->getPosition(), obj);
        }

        // To avoid rerunning the loading logic next frame
        _uiScene.setLoadClicked(false);
    }
}

void LevelEditorController::fixedUpdate(float dt) {
    // Turn the physics engine crank.
    _world->update(FIXED_TIMESTEP_S);
}

void LevelEditorController::postUpdate(float remain)
{
    // Since items may be deleted, garbage collect
    _world->garbageCollect();
}

void LevelEditorController::setSpriteBatch(const shared_ptr<SpriteBatch>& batch) {
    Scene2::setSpriteBatch(batch);
    _levelEditorScene.setSpriteBatch(batch);
    _uiScene.setSpriteBatch(batch);
}

void LevelEditorController::render() {
    _backgroundScene.render();
    Scene2::render();
    _levelEditorScene.render();
    _uiScene.render();
}

#pragma mark -
#pragma mark Helpers

/**
 * Creates an item of type item and places it at the grid position.
 *
 *@return the object being placed and created
 *
 * @param gridPos   The grid position to place the item at
 * @param item  The type of the item to be placed/created
 */
std::shared_ptr<Object> LevelEditorController::placeItem(Vec2 gridPos, Item item) {
    CULog("There are %d objects", _objectController->getObjects()->size());
    shared_ptr<Object> obj;
    switch (item) {
    case (PLATFORM):
        return _objectController->createPlatform(gridPos, itemToSize(item), "log");
    case (MOVING_PLATFORM):
        return nullptr;//_networkController->createMovingPlatformNetworked(gridPos, itemToSize(item), gridPos + Vec2(3, 0), 1, _levelEditorScene.getScale() / getSystemScale());
    case (WIND):
        return _objectController->createWindObstacle(gridPos, itemToSize(item), Vec2(0, 4.0), Vec2(0, 3.0), "default");
    case (SPIKE):
        return _objectController->createSpike(gridPos, itemToSize(item), _levelEditorScene.getScale() / getSystemScale(), 0, "default");
    case (MUSHROOM):
        return _networkController->createMushroomNetworked(gridPos, Size(2, 1), _levelEditorScene.getScale());
    case (TREASURE):
        // For now, assuming that players won't be able to place treasure.
        // No need to make it networked here since this code should only run in the level editor.
        // Also, this offset of (0.5, 0.5) seems to be necessary - probably not worth debugging further since this is level editor mode only.
        return _objectController->createTreasure(gridPos + Vec2(0.5f, 0.5f), itemToSize(item), "default");
    case (TILE_ALPHA):
        // For now, this is the same as any other platform (but not networked, and should only be accessible from the level editor).
        obj = _objectController->createPlatform(gridPos, itemToSize(item), "tile");
        obj->setItemType(TILE_ALPHA);
        return obj;
    case (ART_OBJECT):
        return _objectController->createArtObject(gridPos, itemToSize(item), _levelEditorScene.getScale() / getSystemScale(), 0, "default");
    case (NONE):
        return nullptr;
    }
}

/**
 * Snaps the Box2D position to within the bounds of the build phase grid.
 *
 * @return the grid position
 *
 * @param screenPos    The screen position
 * @param item               The selected item being snapped to the grid
 */
Vec2 LevelEditorController::snapToGrid(const Vec2& gridPos, Item item) {
    Size offset = itemToGridSize(item) - Vec2(1, 1);

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


