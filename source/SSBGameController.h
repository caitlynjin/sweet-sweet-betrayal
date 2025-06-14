//
//  SSBGameController.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/16/25.
//

#ifndef __SSB_GAME_CONTROLLER_H__
#define __SSB_GAME_CONTROLLER_H__
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
#include "BuildPhaseController.h"
#include "MovePhaseController.h"
#include "NetworkController.h"
#include "SoundController.h"
#include "ObjectController.h"
#include "PauseScene.h"
//#include <cmath>

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;


/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class SSBGameController : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;
    std::shared_ptr<PlatformInput> _input;
    std::shared_ptr<BuildPhaseController> _buildPhaseController;
    std::shared_ptr<MovePhaseController> _movePhaseController;
    std::shared_ptr<ObjectController> _objectController;
    /** The network controller */
    std::shared_ptr<NetworkController> _networkController;
    /** The network  */
    std::shared_ptr<NetEventController> _network;
    /** Reference to the grid manager */
    std::shared_ptr<GridManager> _gridManager;
    /** Controller for handling audio logic */
    std::shared_ptr<SoundController> _sound;

    /** Reference to background scene */
    scene2::Scene2 _backgroundScene;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;

    /** The Box2D world */
    std::shared_ptr<cugl::physics2::distrib::NetWorld> _world;
    /** The list of objects */
    std::vector<std::shared_ptr<Object>> _objects;
    /** The list of parallax objects */
    std::vector<std::shared_ptr<Object>> _parallaxObjects;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    /** The offset from the world */
    Vec2 _offset;
    /** The initial camera position */
    Vec2 _initialCameraPos;

    /** Whether we are in build mode */
    bool _buildingMode;
    
    /** Whether a player has won the level */
    bool _hasVictory = false;
    /** Whether the game is paused */
    bool _isPaused = false;

    /** the level num */
    int _levelNum = 0;

    /** Countdown active for displaying scoreboard between rounds */
    int _scoreCountdown = -1;
    
    // Countdown between build ready and dispaying scoreboard
    int _beforeScoreBoard = 15;
    // frames between each player
    int _nextInRoundDelay = 30;
    // next index to show up in scoreboard
    size_t _nextInRoundIndex = 0;

public:
#pragma mark -
#pragma mark Constructors

    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    SSBGameController();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~SSBGameController() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    void disposeLevel();

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
    bool init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController>& sound);

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
    bool init(const std::shared_ptr<AssetManager>& assets,
              const Rect& rect);

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
    bool init(const std::shared_ptr<AssetManager>& assets,
        const Rect& rect, const Vec2& gravity, const std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> &sound);
    
    /** To be called after level select */
    bool finishInit();

    /** Creates all the parallax art objects. */

    void createParallaxObjects();
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This is the nondeterministic version of a physics simulation. It is
     * provided for comparison purposes only.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

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
    void preUpdate(float dt);

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
    void fixedUpdate(float step);

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
    void postUpdate(float remain);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset() override;

    void setSpriteBatch(const shared_ptr<SpriteBatch> &batch);

    void render() override;

    /** Sets the scene and associated scenes as active */
    void setElementsActive(bool value);
    /**
     * Sets whether mode is in building or play mode.
     *
     * @param value whether the level is in building mode.
     */
    void setBuildingMode(bool value);
    
    /**
     * Sets whether a player has won the current level.
     *
     * @param value whether a player has won
     */
    void setHasVictory(bool value);
    
    void setLevelNum(int level){
        _movePhaseController->setLevelNum(level);
        _levelNum = level;
    }
    
    int getLevelNum(){
        return _movePhaseController->getLevelNum();
    }
    
    /**
     Returns whether a player has won the current level.
     */
    bool getHasVictory() {return _hasVictory;};

    /**
     * @return true if the game is paused
     */
    bool getIsPaused() { return _isPaused; }

    /**
     * Sets whether the game is paused.
     */
    void setIsPaused(bool value) {
        _isPaused = value;
        _buildPhaseController->setIsPaused(value);
        _movePhaseController->setIsPaused(value);
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
    Vec2 convertScreenToBox2d(const Vec2& screenPos, float scale, const Vec2& offset);


  };

#endif /* __SSB_GAME_CONTROLLER_H__ */
