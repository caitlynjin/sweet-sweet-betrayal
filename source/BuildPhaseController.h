//
//  BuildPhaseController.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/16/25.
//

#ifndef __SSB_BUILD_PHASE_CONTROLLER_H__
#define __SSB_BUILD_PHASE_CONTROLLER_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SSBGridManager.h"
#include "NetworkController.h"
#include "ObjectController.h"
#include "BuildPhaseScene.h"
#include "BuildPhaseUIScene.h"

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;

/**
 * This class is the move phase controller.
 */
class BuildPhaseController {

protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    std::shared_ptr<GridManager> _gridManager;
    std::shared_ptr<ObjectController> _objectController;
    std::shared_ptr<NetworkController> _networkController;
    /** The network  */
    std::shared_ptr<NetEventController> _network;
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<PlatformInput> _input;
    BuildPhaseScene _buildPhaseScene;
    BuildPhaseUIScene _uiScene;
    std::shared_ptr<PlayerModel> _player;

    /** The selected randomized items in the inventory */
    std::vector<Item> inventoryItems;
    /** The assets of the selected randomized items in the inventory */
    std::vector<std::string> assetNames;
    /** The selected item in build mode (new object) */
    Item _selectedItem = NONE;
    /** The selected object in build mode (object being moved) */
    std::shared_ptr<Object> _selectedObject;
    /** Previous position of object in build phase */
    Vec2 _prevPos = Vec2(0, 0);
    /** The number of items currently placed */
    int _itemsPlaced = 0;
    /** Whether the message has been sent */
    bool _readyMessageSent = false;
    /** Whether acceleration timer has started for camera button */
    bool _accelerationStarted = false;
    /** Starting time for acceleration timer */
    Uint64 _accelerationStart;


public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates the move phase controller.
     */
    BuildPhaseController();

    /**
     * Initializes the controller contents.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<ObjectController> objectController, std::shared_ptr<NetworkController> networkController, std::shared_ptr<Camera> camera);

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);

    void setSpriteBatch(const shared_ptr<SpriteBatch> &batch);

    void render();

#pragma mark -
#pragma mark Helpers
    /**
     * Processes the change between modes (movement and building mode).
     *
     * @param value whether the level is in building mode.
     */
    void processModeChange(bool value);
    
    /**
     * Assigns a callback function that will be executed when `setBuildingMode` is called.
     */
    void setBuildingModeCallback(std::function<void(bool)> callback);

    /**
     * Triggers a change in building mode.
     */
    void setBuildingMode(bool value);

    /**
     * Randomizes the items in the inventory and selects only a `count` of these items.
     *
     * @param count     the number of items to select
     */
    void randomizeItems(int count = 4);

    /**
     * Adds the inventory button listeners.
     */
    void addInvButtonListeners();

    /**
     * Creates an item of type item and places it at the grid position.
     *
     *@return the object being placed and created
     *
     * @param gridPos   The grid position to place the item at
     * @param item  The type of the item to be placed/created
     */
    std::shared_ptr<Object> placeItem(Vec2 gridPos, Item item);

    /**
     * Snaps the Box2D position to within the bounds of the build phase grid.
     *
     * @return the grid position
     *
     * @param screenPos    The screen position
     * @param item               The selected item being snapped to the grid
     */
    Vec2 snapToGrid(const Vec2 &gridPos, Item item);


};

#endif /* __SSB_BUILD_PHASE_CONTROLLER_H__ */
