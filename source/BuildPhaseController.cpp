//
//  BuildPhaseController.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/16/25.
//

#include "BuildPhaseController.h"
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
bool BuildPhaseController::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<ObjectController> objectController, std::shared_ptr<NetworkController> networkController, std::shared_ptr<Camera> camera) {
    if (assets == nullptr)
    {
        return false;
    }
    
    _assets = assets;
    _input = input;
    _gridManager = gridManager;
    _objectController = objectController;
    _networkController = networkController;
    _network = networkController->getNetwork();

    // Initialize build phase scene
    _buildPhaseScene.init(assets, camera);

    // Initalize UI Scene
    _uiScene.init(assets);
    
    std::vector<Item> inventoryItems = {PLATFORM, MOVING_PLATFORM, WIND};
    std::vector<std::string> assetNames = {LOG_TEXTURE, GLIDING_LOG_TEXTURE, WIND_TEXTURE};
    _uiScene.initInventory(inventoryItems, assetNames);

    std::vector<std::shared_ptr<scene2::Button>> inventoryButtons = _uiScene.getInventoryButtons();
    for (size_t i = 0; i < inventoryButtons.size(); i++) {
        inventoryButtons[i]->addListener([this, item = inventoryItems[i]](const std::string &name, bool down) {
            if (down & buildingMode) {
                _selectedItem = item;
                _input->setInventoryStatus(PlatformInput::PLACING);
            }
        });
    }

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
    _uiScene.reset();
    _itemsPlaced = 0;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void BuildPhaseController::preUpdate(float dt) {
    if (buildingMode) {
        /** The offset of finger placement to object indicator */
        Vec2 dragOffset = Vec2(-1, 1);

        // Deactivate inventory buttons once all traps are placed
        _uiScene.activateInventory(_itemsPlaced == 0);

        if (_input->isTouchDown() && (_input->getInventoryStatus() == PlatformInput::PLACING))
        {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos), NONE);
            Vec2 gridPosWithOffset = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos) + dragOffset, _selectedItem);
            
            // Show placing object indicator when dragging object
            if (_selectedItem != NONE) {
//                CULog("Placing object");
                
                if (_selectedObject) {
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
        else if (_input->getInventoryStatus() == PlatformInput::WAITING)
        {
            _gridManager->setSpriteInvisible();
            
            if (_input->isTouchDown()) {
                // Attempt to move object that exists on the grid
                Vec2 screenPos = _input->getPosOnDrag();
                Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos), NONE);
                
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
        else if (_input->getInventoryStatus() == PlatformInput::PLACED)
        {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos) + dragOffset, _selectedItem);;
            
            if (_selectedObject) {
                if (!_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem))) {
                    // Move the object back to its original position
                    _selectedObject->setPosition(_prevPos);
                    _gridManager->addMoveableObject(_prevPos, _selectedObject);
                    _prevPos = Vec2(0, 0);
                } else {
                    // Move the existing object to new position
                    CULog("Reposition object");
                    _selectedObject->setPosition(gridPos);
                    if (_selectedObject->getItemType()== Item::PLATFORM) {
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
            } else {
                // Place new object on grid
                Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos) + dragOffset, _selectedItem);;
                
                if (_gridManager->canPlace(gridPos, itemToGridSize(_selectedItem))) {
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
            
            // Reset selected item
            _selectedItem = NONE;
            
            // Darken inventory UI
            _uiScene.getInventoryOverlay()->setVisible(true);
            _input->setInventoryStatus(PlatformInput::WAITING);
        }
    }

    _buildPhaseScene.preUpdate(dt);

    if (_uiScene.getRightPressed() && buildingMode){
        _buildPhaseScene.getCamera()->translate(10, 0);
        _buildPhaseScene.getCamera()->update();
    }
    if (_uiScene.getLeftPressed() && buildingMode){
        _buildPhaseScene.getCamera()->translate(-10, 0);
        _buildPhaseScene.getCamera()->update();
    }

    if (_uiScene.getIsReady() && !_readyMessageSent){
//        CULog("send out event");
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::BUILD_READY));
        _readyMessageSent = true;
    } else if (!_uiScene.getIsReady()) {
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
    buildingMode = value;

    _buildPhaseScene.setVisible(value);
    _buildPhaseScene.resetCameraPos();

    _uiScene.setVisible(value);
    _itemsPlaced = 0;

    if (value){
        _uiScene.setIsReady(false);
    }
}

/**
 * Assigns a callback function that will be executed when `setBuildingMode` is called.
 */
void BuildPhaseController::setBuildingModeCallback(std::function<void(bool)> callback) {
    _buildingModeCallback = callback;
}

/**
 * Triggers a change in building mode.
 */
void BuildPhaseController::setBuildingMode(bool value) {
    if (_buildingModeCallback) {
        _buildingModeCallback(value);  // Calls the GameController's `setBuildingMode`
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
    switch (item) {
        case (PLATFORM):
            return _networkController->createPlatformNetworked(gridPos, itemToSize(item), "log", _buildPhaseScene.getScale());
        case (MOVING_PLATFORM):
            return _networkController->createMovingPlatformNetworked(gridPos, itemToSize(item), gridPos + Vec2(3, 0), 1, _buildPhaseScene.getScale());
        case (WIND):
            return _objectController->createWindObstacle(gridPos, itemToSize(item), Vec2(0, 1.0), "default");
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
Vec2 BuildPhaseController::snapToGrid(const Vec2 &gridPos, Item item) {
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


