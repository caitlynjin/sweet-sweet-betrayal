//
//  BuildPhaseUIScene.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/17/25.
//

#ifndef __SSB_BUILD_PHASE_UI_SCENE_H__
#define __SSB_BUILD_PHASE_UI_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "PlayerModel.h"
#include "SSBGridManager.h"
#include "Platform.h"
#include "WindObstacle.h"
#include "Treasure.h"
#include "MessageEvent.h"
#include "NetworkController.h"
#include "ObjectController.h"

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;

/**
 * This class is the build phase UI scene.
 */
class BuildPhaseUIScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;
    /** The grid manager */
    std::shared_ptr<GridManager> _gridManager;

    /** Reference to the background of the inventory */
    std::shared_ptr<scene2::PolygonNode> _inventoryBackground;
    /** Reference to the overlay of the inventory */
    std::shared_ptr<scene2::PolygonNode> _inventoryOverlay;
    /** Reference to build mode inventory buttons */
    std::vector<std::shared_ptr<scene2::Button>> _inventoryButtons;
    /** Reference to the ready button */
    std::shared_ptr<cugl::scene2::Button> _readyButton;
    /** Reference to the right button */
    std::shared_ptr<cugl::scene2::Button> _rightButton;
    /** Reference to the left button */
    std::shared_ptr<cugl::scene2::Button> _leftButton;
    /** Reference to the trash button */
    std::shared_ptr<cugl::scene2::Button> _trashButton;
    /** Reference to the timer */
    std::shared_ptr<cugl::scene2::Label> _timer;

    /** Whether the player is ready to proceed to movement phase */
    bool _isReady = false;
    /** Whether right camera button is being pressed */
    bool _rightpressed = false;
    /** Whether left camera button is being pressed */
    bool _leftpressed = false;

    /** Starting time for the build mode timer */
    Uint64 _startTime;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    BuildPhaseUIScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~BuildPhaseUIScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes the scene contents
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<GridManager> gridManager);

    /**
     * Initializes the grid layout on the screen for build mode.
     */
    void initInventory(std::vector<Item> inventoryItems, std::vector<std::string> assetNames);

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Resets the status of the game so that we can play again.
     *
     * This method disposes of the world and creates a new one.
     */
    void reset();

    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);

#pragma mark -
#pragma mark Attribute Functions
    /**
    * @return true if the right button was pressed
    */
    bool getRightPressed() {
        return _rightpressed;
    }

    /**
    * @return true if the left button was pressed
    */
    bool getLeftPressed() {
        return _leftpressed;
    }

    /**
     * @return true if the ready button was pressed
     */
    bool getIsReady() {
        return _isReady;
    }


    /**
     * Sets whether the player has pressed the ready button to indicate they are done with build phase.
     */
    void setIsReady(bool isDone);

    /**
     * Gets the inventory buttons.
     */
    std::vector<std::shared_ptr<scene2::Button>> getInventoryButtons() { return _inventoryButtons; };

    /**
     * Gets the inventory overlay.
     */
    std::shared_ptr<scene2::PolygonNode> getInventoryOverlay() { return _inventoryOverlay; };

    /**
     * Gets the trash button.
     */
    std::shared_ptr<scene2::Button> getTrashButton() { return _trashButton; };


#pragma mark -
#pragma mark Helpers
    /**
     * Set whether the elements of this scene are visible or not
     */
    void setVisible(bool value);

    /**
     * Whether to activate the inventory.
     */
    void activateInventory(bool value);

    /**
     * Set the inventory buttons for each item.
     */
    void setInventoryButtons(std::vector<Item> inventoryItems, std::vector<std::string> assetNames);

};

#endif /* __SSB_BUILD_PHASE_UI_SCENE_H__ */
