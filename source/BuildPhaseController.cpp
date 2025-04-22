//
//  BuildPhaseController.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/16/25.
//

#include "BuildPhaseController.h"
#include "Constants.h"
#include "Platform.h"
#include "Tile.h"
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

#pragma mark -
#pragma mark Constants
/** List of all inventory items that are placeable */
std::vector<Item> allInventoryItems = { PLATFORM, MOVING_PLATFORM, WIND, THORN, MUSHROOM };
/** List of all corresponding textures to items that are placeable */
std::vector<std::string> allAssetNames = { LOG_ICON, GLIDING_LOG_ICON, WIND_ICON, THORN_TILE_ICON, MUSHROOM_ICON };


#pragma mark -
#pragma mark Constructors
/**
 * Creates the move phase controller.
 */
BuildPhaseController::BuildPhaseController() {}

/**
 * Initializes the controller contents.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool BuildPhaseController::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::distrib::NetWorld>& world, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<ObjectController> objectController, std::shared_ptr<NetworkController> networkController, std::shared_ptr<Camera> camera) {
    if (assets == nullptr)
    {
        return false;
    }
    
    _assets = assets;
    _world = world;
    _input = input;
    _gridManager = gridManager;
    _objectController = objectController;
    _networkController = networkController;
    _network = networkController->getNetwork();

    // Initialize build phase scene
    _buildPhaseScene.init(assets, camera);

    // Initalize UI Scene
    _uiScene.init(assets, _gridManager);
    randomizeItems();
    _uiScene.initInventory(inventoryItems, assetNames);
    addInvButtonListeners();
    _uiScene.activateInventory(true);

    return true;
};

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void BuildPhaseController::dispose() {
    _buildPhaseScene.dispose();
    _uiScene.dispose();
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void BuildPhaseController::reset() {
    _buildPhaseScene.reset();
    randomizeItems();
    addInvButtonListeners();
    _uiScene.reset();
    
    // Reset controller variables
    _itemsPlaced = 0;
    _selectedItem = NONE;
    _selectedObject = nullptr;
    _prevPos = Vec2(0, 0);
    _readyMessageSent = false;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void BuildPhaseController::preUpdate(float dt) {
    // TODO: All of this logic should be moved to another method, such as gridManagerUpdate()
    /** The offset of finger placement to object indicator */
    Vec2 dragOffset = _input->getSystemDragOffset();

    _uiScene.preUpdate(dt);

    // Deactivate inventory buttons once all traps are placed
    _uiScene.activateInventory(_itemsPlaced == 0);
    if (!_input->isTouchDown() && _input->getInventoryStatus() == PlatformInput::PLACING) {
        _input->setInventoryStatus(PlatformInput::WAITING);
    }
    if (_input->isTouchDown() && (_input->getInventoryStatus() == PlatformInput::PLACING))
    {
        Vec2 screenPos = _input->getPosOnDrag();
        Vec2 gridPosWithOffset = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);

        auto trashBounds = _uiScene.getTrashButton()->getBoundingBox() * 2;
        Vec2 touchPos = _uiScene.getTrashButton()->worldToNodeCoords(screenPos);

        _uiScene.getTrashButton()->activate();
        if (trashBounds.contains(touchPos)) {
            CULog("Hovering over trash!");
            _uiScene.getTrashButton()->setDown(true);
        } else {
            _uiScene.getTrashButton()->setDown(false);
        }

        // Show placing object indicator when dragging object
        if (_selectedItem != NONE) {
            
            if (_selectedObject) {
                // Move the existing object to new position
                _selectedObject->setPositionInit(gridPosWithOffset);
                
                // Trigger obstacle update listener
                if (_selectedObject->getListener()) {
                    _selectedObject->getListener()(_selectedObject.get());
                }
            } else {
                _gridManager->setObject(gridPosWithOffset, _selectedItem);
            }
        }
    }
    else if (_input->getInventoryStatus() == PlatformInput::WAITING)
    {
        _gridManager->setSpriteInvisible();
        _uiScene.getTrashButton()->deactivate();

        if (_input->isTouchDown()) {
            // Attempt to move object that exists on the grid
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()), NONE);
            
            std::shared_ptr<Object> obj = _gridManager->moveObject(gridPos);

            // If object exists
            if (obj) {
                CULog("Selected existing object");
                if (!itemIsArtObject(obj->getItemType())) {
                    _selectedObject = obj;
                    _selectedItem = obj->getItemType();
                    // Set the current position of the object
                    _prevPos = _selectedObject->getPositionInit();

                    _input->setInventoryStatus(PlatformInput::PLACING);
                }

                
            }
        }
    }
    else if (_input->getInventoryStatus() == PlatformInput::PLACED)
    {
        Vec2 screenPos = _input->getPosOnDrag();
        Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);;

        auto trashBounds = _uiScene.getTrashButton()->getBoundingBox() * 2;
        Vec2 touchPos = _uiScene.getTrashButton()->worldToNodeCoords(screenPos);

        if (trashBounds.contains(touchPos)) {
            CULog("Deleted object");
            _uiScene.getTrashButton()->setDown(false);

            if (_selectedObject) {
                _itemsPlaced -= 1;
                
                _network->getPhysController()->removeSharedObstacle(_selectedObject);
                _objectController->removeObject(_selectedObject);
                _gridManager->deleteObject(_selectedObject);

                // Undarken inventory UI
                _uiScene.getInventoryOverlay()->setVisible(false);
            }
        } else {
            if (_selectedObject) {
                if (!_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem), _selectedItem)) {
                    // Move the object back to its original position
                    _selectedObject->setPositionInit(_prevPos);
                    _gridManager->addMoveableObject(_prevPos, _selectedObject);
                    _prevPos = Vec2(0, 0);
                } else {
                    // Move the existing object to new position
                    CULog("Reposition object");
                    _selectedObject->setPositionInit(gridPos);
                    if (_selectedObject->getItemType()== Item::PLATFORM) {
                        auto platform = std::dynamic_pointer_cast<Platform>(_selectedObject);
                        if (platform) {
                            platform->updateMoving(gridPos);
                        }
                    }
                    _gridManager->addMoveableObject(gridPos, _selectedObject);
                }

                // Trigger listener
                if (_selectedObject->getListener()) {
                    _selectedObject->getListener()(_selectedObject.get());
                }

                // Reset selected object
                _selectedObject = nullptr;
            } else {
                // Place new object on grid
                Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);;

                if (_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem), _selectedItem)) {
                    std::shared_ptr<Object> obj = placeItem(gridPos, _selectedItem);
                    _gridManager->addMoveableObject(gridPos, obj);

                    _itemsPlaced += 1;

                    // Update inventory UI
                    if (_itemsPlaced >= 1)
                    {
                        _uiScene.activateInventory(false);
                    }
                }
            }

            // Darken inventory UI
            _uiScene.getInventoryOverlay()->setVisible(true);
        }

        // Reset selected item
        _selectedItem = NONE;
        _selectedObject = nullptr;

        _input->setInventoryStatus(PlatformInput::WAITING);
    }
    

    _buildPhaseScene.preUpdate(dt);

//    CULog("%f", _buildPhaseScene.getCamera()->getPosition().x);

    if ((_uiScene.getRightPressed() || _uiScene.getLeftPressed()) && !_accelerationStarted){
        _accelerationStarted = true;
        _accelerationStart = Application::get()->getEllapsedMicros();
    }
    else if (!(_uiScene.getRightPressed() || _uiScene.getLeftPressed())){
        _accelerationStarted = false;
    }

    // TODO: Segment out to another method, uiSceneUpdate()
    if (_uiScene.getRightPressed() && _buildPhaseScene.getCamera()->getPosition().x <= _objectController->getGoalPos().x * 64){
        Uint64 currentTime = Application::get()->getEllapsedMicros();
        Uint64 elapsedTime = currentTime - _accelerationStart;
        if (elapsedTime < 500000){
            _buildPhaseScene.getCamera()->translate(10, 0);
        }
        else{
            _buildPhaseScene.getCamera()->translate(20, 0);
        }
        _buildPhaseScene.getCamera()->update();
    }
    if (_uiScene.getLeftPressed() && _buildPhaseScene.getCamera()->getPosition().x >= 0){
        Uint64 currentTime = Application::get()->getEllapsedMicros();
        Uint64 elapsedTime = currentTime - _accelerationStart;
        if (elapsedTime < 500000){
            _buildPhaseScene.getCamera()->translate(-10, 0);
        }
        else{
            _buildPhaseScene.getCamera()->translate(-20, 0);
        }
        _buildPhaseScene.getCamera()->update();
    }

    if (_uiScene.getIsReady() && !_readyMessageSent){
//        CULog("send out event");
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::BUILD_READY));
        _readyMessageSent = true;
    }
    else if (!_uiScene.getIsReady()) {
        _readyMessageSent = false;
    }
}

void BuildPhaseController::setSpriteBatch(const shared_ptr<SpriteBatch> &batch) {
    _buildPhaseScene.setSpriteBatch(batch);
    _uiScene.setSpriteBatch(batch);
}

void BuildPhaseController::render() {
    _buildPhaseScene.render();
    _uiScene.render();
}

#pragma mark -
#pragma mark Helpers
/**
 * Processes the change between modes (movement and building mode).
 *
 * @param value whether the level is in building mode.
 */
void BuildPhaseController::processModeChange(bool value) {
    _buildPhaseScene.setVisible(value);
    _buildPhaseScene.resetCameraPos();

    _uiScene.setVisible(value);
    _itemsPlaced = 0;

    if (value){
        randomizeItems();
        addInvButtonListeners();
        _uiScene.setIsReady(false);
        _networkController->resetRound();
    }
}

/**
 * Randomizes the items in the inventory and selects only a `count` of these items.
 *
 * @param count     the number of items to select
 */
void BuildPhaseController::randomizeItems(int count) {
    std::vector<std::pair<Item, std::string>> pairedItems;
    for (int i = 0; i < allInventoryItems.size(); ++i) {
        pairedItems.emplace_back(allInventoryItems[i], allAssetNames[i]);
    }

    // Shuffle the full set
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pairedItems.begin(), pairedItems.end(), g);

    // Clear previous round
    inventoryItems.clear();
    assetNames.clear();

    // Take the first `count` items
    for (int i = 0; i < count; ++i) {
        inventoryItems.push_back(pairedItems[i].first);
        assetNames.push_back(pairedItems[i].second);
        CULog("%s", pairedItems[i].second.c_str());
    }

    _uiScene.setInventoryButtons(inventoryItems, assetNames);
}

/**
 * Adds the inventory button listeners.
 */
void BuildPhaseController::addInvButtonListeners() {
    std::vector<std::shared_ptr<scene2::Button>> inventoryButtons = _uiScene.getInventoryButtons();
    for (size_t i = 0; i < inventoryButtons.size(); i++) {
        inventoryButtons[i]->addListener([this, item = inventoryItems[i]](const std::string &name, bool down) {
            if (down) {
                _selectedItem = item;
                _input->setInventoryStatus(PlatformInput::PLACING);
            }
        });
    }
}

/**
 * Creates an item of type item and places it at the grid position.
 *
 *@return the object being placed and created
 *
 * @param gridPos   The grid position to place the item at
 * @param item  The type of the item to be placed/created
 */
std::shared_ptr<Object> BuildPhaseController::placeItem(Vec2 gridPos, Item item) {
    shared_ptr<Object> obj;
    switch (item) {
        case (PLATFORM):
            return _networkController->createPlatformNetworked(gridPos, itemToSize(item), "log", _buildPhaseScene.getScale() / getSystemScale());
        case (MOVING_PLATFORM):
            return _networkController->createMovingPlatformNetworked(gridPos, itemToSize(item), gridPos + Vec2(3, 0), 1, _buildPhaseScene.getScale() / getSystemScale());
        case (WIND):
            return _networkController->createWindNetworked(gridPos, itemToSize(item), Vec2(0, 4.0), Vec2(0, 3.0));
        case (SPIKE):
            return _objectController->createSpike(gridPos, itemToSize(item), _buildPhaseScene.getScale() / getSystemScale(), 0, "default");
        case (THORN):
            return _networkController->createThornNetworked(gridPos, itemToSize(item));
        case (MUSHROOM):
            return _networkController->createMushroomNetworked(gridPos, itemToSize(item), _buildPhaseScene.getScale() / getSystemScale());
        case (TREASURE):
            // For now, assuming that players won't be able to place treasure.
            // No need to make it networked here since this code should only run in the level editor.
            // Also, this offset of (0.5, 0.5) seems to be necessary - probably not worth debugging further since this is level editor mode only.
            return _objectController->createTreasure(gridPos + Vec2(0.5f, 0.5f), itemToSize(item), "default");
        case (TILE_ITEM):
            // For now, this is the same as any other platform (but not networked, and should only be accessible from the level editor).
            obj = _objectController->createTile(gridPos, itemToSize(item), "default", _buildPhaseScene.getScale());
            obj->setItemType(TILE_ITEM);
            return obj;
        case (NONE):
            return nullptr;
        default:
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
Vec2 BuildPhaseController::snapToGrid(const Vec2 &gridPos, Item item) {
    // Consider size of item
    Size offset = itemToGridSize(item) - Vec2(1, 1);

    int xGrid = gridPos.x;
    int yGrid = gridPos.y;

    // Snaps the placement to inside the grid
    int minRow = ROW_OFFSET_BOT;
    int maxRow = MAX_ROWS - ROW_OFFSET_TOP - 1;
    int maxCol = _gridManager->getNumColumns() - 1;

    xGrid = xGrid < 0 ? 0 : xGrid;
    yGrid = yGrid < minRow ? minRow : yGrid;
    xGrid = xGrid + offset.width > maxCol ? maxCol - offset.width : xGrid;
    yGrid = yGrid + offset.height > maxRow ? maxRow - offset.height : yGrid;

    return Vec2(xGrid, yGrid);
}


