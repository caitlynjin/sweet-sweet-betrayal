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
#include "SSBDudeModel.h"
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
    /** Reference to the level editor load button */
    std::shared_ptr<cugl::scene2::Button> _loadButton;
    /** Reference to the level editor paintbrush button */
    std::shared_ptr<cugl::scene2::Button> _paintButton;
    /** Reference to the right button */
    std::shared_ptr<cugl::scene2::Button> _rightButton;
    /** Reference to the left button */
    std::shared_ptr<cugl::scene2::Button> _leftButton;
    /** The text input for the file name to save to in level select mode. */
    std::shared_ptr<cugl::scene2::TextField> _fileSaveText;
    /** The text input for the file name to load in level select mode. */
    std::shared_ptr<cugl::scene2::TextField> _fileLoadText;

    /** Whether the player is ready to proceed to movement phase */
    bool _isReady = false;
    /** Whether right camera button is being pressed */
    bool _rightpressed = false;
    /** Whether left camera button is being pressed */
    bool _leftpressed = false;
    /** Whether we are in level editor mode */
    bool _isLevelEditor = false;
    /** Whether or not the level editor load button was clicked */
    bool _isTimeToLoad = false;
    /** Whether or not the level editor is currently in paintbrush mode.
    * Paintbrush mode makes it so that instead of dragging an object into place and releasing to create it,
    * you can simply drag your mouse over several tiles at once, placing a copy of the object
    * in all of those grid locations.
    * This is particularly useful for 1x1 tile placement.
    */
    bool _inPaintMode = false;

    /** Whether or not the paint BUTTON is currently down. */
    bool _paintButtonDown = false;

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

    /** Returns the text that the user input for the save file in level editor mode. */
    std::string getSaveFileName() {
        return _fileSaveText->getText();
    }
    /** Returns the text that the user input for the file to load into level editor mode. */
    std::string getLoadFileName() {
        return _fileLoadText->getText();
    }

    /** Returns the text object itself for the user's save file input. */
    std::shared_ptr<cugl::scene2::TextField> getSaveTextField() {
        return _fileSaveText;
    }

    /** Returns the text object itself for the user's load file input. */
    std::shared_ptr<cugl::scene2::TextField> getLoadTextField() {
        return _fileLoadText;
    }

    /** Gets whether or not the load button was clicked. */
    bool getLoadClicked() {
        return _isTimeToLoad;
    }
    
    /** Sets whether or not the load button was clicked. */
    void setLoadClicked(bool value);

    /** Gets whether or not we are currently in paint mode */
    bool isPaintMode() {
        return _inPaintMode;
    }

    /**
     * Sets whether the player has pressed the ready button to indicate they are done with build phase.
     */
    void setIsReady(bool isDone);

    /** Sets whether or not we are in level editor mode.
    * By default, we are not.
    */
    void setLevelEditor(bool value);

    /**
     * Gets the inventory buttons.
     */
    std::vector<std::shared_ptr<scene2::Button>> getInventoryButtons() { return _inventoryButtons; };

    /**
     * Gets the inventory overlay.
     */
    std::shared_ptr<scene2::PolygonNode> getInventoryOverlay() { return _inventoryOverlay; };


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

};

#endif /* __SSB_BUILD_PHASE_UI_SCENE_H__ */
