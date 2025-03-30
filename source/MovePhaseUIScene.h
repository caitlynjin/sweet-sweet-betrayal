//
//  MovePhaseUIScene.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/18/25.
//

#ifndef __SSB_MOVE_PHASE_UI_SCENE_H__
#define __SSB_MOVE_PHASE_UI_SCENE_H__
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
class MovePhaseUIScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** Reference to the label for counting rounds */
    std::shared_ptr<cugl::scene2::Label> _roundsLabel;
    /** Reference to the win message label */
    std::shared_ptr<scene2::Label> _winnode;
    /** Reference to the lose message label */
    std::shared_ptr<scene2::Label> _losenode;
    /** Reference to the rounds message label */
//    std::shared_ptr<scene2::Label> _roundsnode;
    /** Score images */
    std::vector<std::shared_ptr<scene2::PolygonNode>> _scoreImages;
    /** Reference to the left joystick image */
    std::shared_ptr<scene2::PolygonNode> _leftnode;
    /** Reference to the right joystick image */
    std::shared_ptr<scene2::PolygonNode> _rightnode;
    /** Reference to the jump button */
    std::shared_ptr<cugl::scene2::Button> _jumpbutton;
    /** Reference to the glide button */
    std::shared_ptr<cugl::scene2::Button> _glidebutton;
    /** Reference to the progress bar */
    std::shared_ptr<cugl::scene2::PolygonNode> _progressBar;
    /** Reference to the red icon */
    std::shared_ptr<cugl::scene2::PolygonNode> _redIcon;
    /** Reference to the blue icon */
    std::shared_ptr<cugl::scene2::PolygonNode> _blueIcon;
    /** Reference to the treasure icon */
    std::shared_ptr<cugl::scene2::PolygonNode> _treasureIcon;

    /** Total numer of rounds */
    int _totalRounds;
    /** The total amount of rounds */
    int const TOTAL_ROUNDS = 5;
    /** The total amount of gems */
    int const TOTAL_GEMS = 3;
    /** The number of players */
    int _numPlayers;

    /** Whether player is jumping */
    bool _didjump;
    /** Whether player is gliding */
    bool _didglide;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    MovePhaseUIScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MovePhaseUIScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes the scene contents
     *
     * @param assets    The (loaded) assets for this game mode
     * @param players   The number of players
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets, int players);

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

    /**
     * Sets whether mode is in building or play mode.
     *
     * @param value whether the level is in building mode.
     */
    void setBuildingMode(bool value);

#pragma mark -
#pragma mark Attribute Functions
    /**
     * Sets the total number of rounds
     */
    void setTotalRounds(int value);

    /**
     * Get whether the player is gliding
     */
    bool getDidGlide() { return _didglide; };

    /**
     * Sets whether the player is gliding
     */
    void setDidGlide(bool value) { _didglide = value; };

    /**
     * Get whether the player is jumping
     */
    bool getDidJump() { return _didjump; };

    /**
     * Sets whether the player is jumping
     */
    void setDidJump(bool value) { _didjump = value; };


#pragma mark -
#pragma mark Helpers
    /**
     * Set whether the elements of this scene are visible or not
     */
    void setVisible(bool value);

    /**
     * Set whether the win node is visible or not
     */
    void setWinVisible(bool value);

    /**
     * Set whether the lose node is visible or not
     */
    void setLoseVisible(bool value);

    /**
     * Sets the left joystick to be visible.
     */
    void setLeftVisible();

    /**
     * Sets the right joystick to be visible.
     */
    void setRightVisible();

    /**
     * Sets both joysticks to hidden.
     */
    void setJoystickHidden();

    /**
     * Set joystick positions.
     */
    void setJoystickPosition(Vec2 pos);

    /**
     * Get whether the glide button is held down.
     */
    bool isGlideDown();

    /**
     * Get whether the jump button is held down.
     */
    bool isJumpDown();

    /**
     * Set the glide button active.
     */
    void setGlideButtonActive();

    /**
     * Set the jump button active.
     */
    void setJumpButtonActive();

    /**
     * Set the score image at this gem index to full.
     */
    void setScoreImageFull(int index);

    /**
     * Sets current position of red player icon
     *
     * @param pos       The position of the player relative to where the icon will be
     * @param width     The width of the level
     */
    void setRedIcon(float pos, float width);

    /**
     * Sets current position of blue player icon
     *
     * @param pos       The position of the player relative to where the icon will be
     * @param width     The width of the level
     */
    void setBlueIcon(float pos, float width);

    /**
     * Has treasure icon appear in player icon on progress bar if player collects it
     *
     * @param has       Whether or not the player has the treasure
     * @param color     Which color icon the treasure will appear on top of
     */
    void setTreasureIcon(bool has, int color);

    /**
     * Updates round counter
     *
     * @param cur       The current round number
     * @param total     The total number of rounds
     */
    void updateRound(int cur, int total);

};

#endif /* __SSB_MOVE_PHASE_UI_SCENE_H__ */
