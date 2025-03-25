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
    std::vector<Item> inventoryItems;
    std::vector<std::string> assetNames;

    // Eh it'll probably be useful to have this if-statement once ArtObjects are finished
    // This way we can make them only accessible in level editor mode
    if (_isLevelEditor) {
        inventoryItems = { PLATFORM, MOVING_PLATFORM, WIND, SPIKE, TREASURE, TILE_ALPHA};
        assetNames = { LOG_TEXTURE, GLIDING_LOG_TEXTURE, WIND_TEXTURE, SPIKE_TILE_TEXTURE, TREASURE_TEXTURE, TILE_TEXTURE };
    }
    else {
        inventoryItems = { PLATFORM, MOVING_PLATFORM, WIND, SPIKE};
        assetNames = { LOG_TEXTURE, GLIDING_LOG_TEXTURE, WIND_TEXTURE, SPIKE_TILE_TEXTURE };
    }
    
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
    if (_isLevelEditor) {
        _uiScene.getSaveTextField()->activate();
        _uiScene.getLoadTextField()->activate();
    }

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
        Vec2 dragOffset = _input->getSystemDragOffset();

        // Deactivate inventory buttons once all traps are placed
        _uiScene.activateInventory(_itemsPlaced == 0 || _isLevelEditor);

        if (_input->isTouchDown() && (_input->getInventoryStatus() == PlatformInput::PLACING))
        {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()), NONE);
            Vec2 gridPosWithOffset = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);
            
            // Show placing object indicator when dragging object
            if (_selectedItem != NONE) {
                //CULog("Placing object");
                
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
        else if (_input->getInventoryStatus() == PlatformInput::WAITING)
        {
            _gridManager->setSpriteInvisible();
            
            if (_input->isTouchDown()) {
                // Attempt to move object that exists on the grid
                Vec2 screenPos = _input->getPosOnDrag();
                Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()), NONE);
                
                std::shared_ptr<Object> obj = _gridManager->removeObject(gridPos);
                
                
                // If object exists
                if (obj) {
                    CULog("Selected existing object");
                    _selectedObject = obj;
                    _selectedItem = obj->getItemType();
                    _input->setInventoryStatus(PlatformInput::PLACING);
                }
            }
        }
        else if (_input->getInventoryStatus() == PlatformInput::PLACED)
        {
            Vec2 screenPos = _input->getPosOnDrag();
            Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);;
            
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
                    if (_selectedObject->getItemType()== Item::PLATFORM) {
                        auto platform = std::dynamic_pointer_cast<Platform>(_selectedObject);
                        if (platform) {
                            platform->updateMoving(gridPos);
                        }
                    }
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
                Vec2 gridPos = snapToGrid(_buildPhaseScene.convertScreenToBox2d(screenPos, getSystemScale()) + dragOffset, _selectedItem);;
                
                if (!_gridManager->hasObject(gridPos)) {
                    std::shared_ptr<Object> obj = placeItem(gridPos, _selectedItem);
                    //obj->setTexture(_assets->get<Texture>(itemToAssetName(_selectedItem)));
                    _gridManager->addObject(gridPos, obj);
                    
                    _itemsPlaced += 1;
                    
                    // Update inventory UI
                    if (_itemsPlaced >= 1)
                    {
                        _uiScene.activateInventory(_isLevelEditor);
                    }
                }
            }
            
            // Reset selected item
            _selectedItem = NONE;
            _selectedObject = nullptr;
            
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

    if (_uiScene.getIsReady() && !_readyMessageSent && !_isLevelEditor){
//        CULog("send out event");
        _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::BUILD_READY));
        _readyMessageSent = true;
    }
    else if (_uiScene.getIsReady() && _isLevelEditor) {
        // Save the level to a file
        shared_ptr<LevelModel> level = make_shared<LevelModel>();
        level->createJsonFromLevel("json/" + _uiScene.getSaveFileName() + ".json", Size(100, 100), _objectController->getObjects());
    }
    else if (!_uiScene.getIsReady()) {
        _readyMessageSent = false;
    }

    if (_uiScene.getLoadClicked() && _isLevelEditor) {
        // Load the level stored in this file

        // TODO (maybe): save the shared_ptr<LevelModel> somewhere more efficiently
        _objectController->getObjects()->clear();
        shared_ptr<LevelModel> level = make_shared<LevelModel>();
        vector<shared_ptr<Object>> objects = level->createLevelFromJson("json/" + _uiScene.getLoadFileName() + ".json");
        for (auto& obj : objects) {
            _objectController->processLevelObject(obj, _isLevelEditor);
        }
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

/** Sets whether or not we are in level editor mode.
    * By default, we are not.
    */
void BuildPhaseController::setLevelEditor(bool value) {
    _isLevelEditor = value;
    _uiScene.setLevelEditor(value);
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
            if (_isLevelEditor) {
                return _objectController->createPlatform(gridPos, Size(3, 1), "log");
            }
            else {
                return _networkController->createPlatformNetworked(gridPos, Size(3, 1), "log", _buildPhaseScene.getScale() / getSystemScale());
            }
        case (MOVING_PLATFORM):
            return _networkController->createMovingPlatformNetworked(gridPos, Size(3, 1), gridPos + Vec2(3, 0), 1, _buildPhaseScene.getScale() / getSystemScale());
        case (WIND):
            return _objectController->createWindObstacle(gridPos, Size(1, 1), Vec2(0, 1.0), "default");
        case (SPIKE):
            return _objectController->createSpike(gridPos, Size(1, 1), _buildPhaseScene.getScale() / getSystemScale(), 0, "default");
        case (TREASURE):
            // For now, assuming that players won't be able to place treasure.
            // No need to make it networked here since this code should only run in the level editor.
            return _objectController->createTreasure(gridPos + Vec2(0.5f, 0.5f), Size(1, 1), "default");
        case (TILE_ALPHA):
            // For now, this is the same as any other platform (but not networked, and should only be accessible from the level editor).
            return _objectController->createPlatform(gridPos, Size(1, 1), "tile");
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


