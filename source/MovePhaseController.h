//
//  MovePhaseController.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/18/25.
//

#ifndef __SSB_MOVE_PHASE_CONTROLLER_H__
#define __SSB_MOVE_PHASE_CONTROLLER_H__
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
#include "MovePhaseScene.h"
#include "MovePhaseUIScene.h"
#include "SoundController.h"

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;

/**
 * This class is the move phase controller.
 */
class MovePhaseController {
    
    // TODO: Should be in Mushroom class
    int _mushroomCooldown = 0;

protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::distrib::NetWorld> _world;

    std::shared_ptr<GridManager> _gridManager;
    std::shared_ptr<ObjectController> _objectController;
    std::shared_ptr<NetworkController> _networkController;
    /** The network  */
    std::shared_ptr<NetEventController> _network;
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<PlatformInput> _input;
    /** Controller for sound */
    std::shared_ptr<SoundController> _sound;
    /** Camera for the move phase scene */
    std::shared_ptr<Camera> _camera;
    MovePhaseScene _movePhaseScene;
    MovePhaseUIScene _uiScene;

    /** A list of all objects to be updated during each animation frame. */
    std::vector<std::shared_ptr<Object>> _objects;
    /** The sensor fixtures for the local player */
    std::unordered_set<b2Fixture*> _localSensorFixtures;
    /** The sensor fixtures for each player (excluding the local player) */
    std::unordered_map<PlayerModel*, std::unordered_set<b2Fixture*>> _playerSensorFixtures;

    /** The total amount of rounds */
    int const TOTAL_ROUNDS = 5;
    /** The total amount of gems */
    int const TOTAL_GEMS = 3;
    /** The current round the player is on */
    int _currRound = 1;

    /** Countdown active for winning or losing */
    int _countdown;
    /** Level width */
    float _levelWidth;
    /** Starting player position */
    float _playerStart;

    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    /** Whether we have failed at this world (and need a reset) */
    bool _failed;
    /** Whether we have reached the goal or not */
    bool _reachedGoal = false;
    /** Whether or not it is time to animate the goal */
    bool _animateGoal = false;

    /** Whether the game is paused */
    bool _isPaused = false;
    /** Whether the controls and scene elements are active */
    bool _isActive = true;
    bool _controlEnabled = true;

    cugl::ActionFunction _goalDoorAction;
    /** Manager to process the animation actions */
    std::shared_ptr<AnimateSprite> _spinAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _spinSpriteNode;
    std::shared_ptr<cugl::ActionTimeline> _goalDoorTimeline;



public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates the move phase controller.
     */
    MovePhaseController();

    /**
     * Initializes the controller contents.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::distrib::NetWorld> world, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> &sound);

    
    /** Gets called after level select scene */
    bool finishInit();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     Disposes for purpose of re-entering a level after already completing one.
     */
    void disposeLevel();

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Resets the status of the game so that we can play again.
     */
    void resetRound(); 
    
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

    /**
     * The method called to indicate the end of a deterministic loop.
     *
     * @param remain    The amount of time (in seconds) last fixedUpdate
     */
    void postUpdate(float remain);

    /**
    Handles wind collisions
    */
    void windUpdate(std::shared_ptr <WindObstacle> wind, float dt);

    void setSpriteBatch(const shared_ptr<SpriteBatch> &batch);
    
    void setGridManger(const shared_ptr<GridManager> gridManager){
        _gridManager = gridManager;
        _movePhaseScene.setGridManager(gridManager);
    }
    
    void setInput(const shared_ptr<PlatformInput> input){
        _input = input;
    }

    void render();
    
    /**
     * Kills the player for the round.
     */
    void killPlayer();
    
    MovePhaseScene& getMovePhaseScene(){
        return _movePhaseScene;
    }
    
    void rebuildMovePhase(){
        _movePhaseScene.rebuildLevel(&_objects);
    }
    /**
     *  Called when player reaches the goal
     */
    void reachedGoal();

    /**
     * Processes the change between modes (movement and building mode).
     *
     * @param value whether the level is in building mode.
     */
    void processModeChange(bool value);
    
    
    /**
     * Activates or deactivates the scoreboard node
     *
     * @param value whether to activate the scoreboard node
     */
    void scoreboardActive(bool value){
        _uiScene.setScoreboardVisible(value);
        _uiScene.disableUI(value);
    };
    
    void inRoundNodesActive(const std::string& username) {
        _uiScene.inRoundNodesActive(username);
    }

    /**
     * Updates progress bar
     *
     * @param player information of player position and treasure status
     */
    void updateProgressBar(std::shared_ptr<PlayerModel> player);

    void setGoalDoorAnimation(std::shared_ptr<scene2::SpriteNode> sprite);

    /** Sets whether the scenes are active*/
    void setActive(bool value);


#pragma mark -
#pragma mark Attribute Functions
    /**
     * Gets the camera for the move phase scene
     */
    std::shared_ptr<Camera> getCamera() { return _camera; };

    /**
     * Gets the object controller
     */
    std::shared_ptr<ObjectController> getObjectController() { return _objectController; };
    
    /**
     * Gets the object list
     */
    std::vector<std::shared_ptr<Object>> getObjects(){
        return _objects;
    };

    /**
     * Gets the current countdown count
     */
    int getCountdown() { return _countdown; };

    /**
     * Sets the current countdown count
     */
    void setCountdown(int value) { _countdown = value; };

    /**
     * Gets local player
     */
    shared_ptr<PlayerModel> getLocalPlayer() { return _movePhaseScene.getLocalPlayer(); };
    
    void setLevelNum(int level){
        _movePhaseScene.setLevelNum(level);
    }
    
    int getLevelNum(){
        return _movePhaseScene.getLevelNum();
    }

#pragma mark -
#pragma mark State Access
    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }

    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _movePhaseScene.setDebugVisible(value); }

    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }

    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value);

    /**
    * Returns true if the level is failed.
    *
    * If true, the level will reset after a countdown
    *
    * @return true if the level is failed.
    */
    bool isFailure() const { return _failed; }

    /**
    * Sets whether the level is failed.
    *
    * If true, the level will reset after a countdown
    *
    * @param value whether the level is failed.
    */
    void setFailure(bool value);

    /**
    * Sets the level up for the next round.
    *
    * When called, the level will reset after a countdown.

     @param reachedGoal whether the player has reached the goal.
    */
    void nextRound(bool reachedGoal = false);

    /**
     * @return true if the game is paused
     */
    bool getIsPaused() { return _isPaused; }

    /**
     * Sets whether the game is paused.
     */
    void setIsPaused(bool value) { _isPaused = value; _uiScene.setIsPaused(value); }

#pragma mark -
#pragma mark Collision Handling
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

    /**
    * Processes the start of a collision
    *
    * This method is called when we first get a collision between two objects.  We use
    * this method to test if it is the "right" kind of collision.  In particular, we
    * use it to test if we make it to the win door.  We also us it to eliminate bullets.
    *
    * @param  contact  The two bodies that collided
    */
    void beginContact(b2Contact* contact);

    /**
    * Processes the end of a collision
    *
    * This method is called when we no longer have a collision between two objects.
    * We use this method allow the character to jump again.
    *
    * @param  contact  The two bodies that collided
    */
    void endContact(b2Contact* contact);

};

#endif /* __SSB_MOVE_PHASE_CONTROLLER_H__ */
