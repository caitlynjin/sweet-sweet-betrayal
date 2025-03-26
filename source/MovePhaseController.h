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
public:
    /** Whether we are in build mode */
    bool buildingMode = true;

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
    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture*> _sensorFixtures;

    /** The total amount of rounds */
    int const TOTAL_ROUNDS = 5;
    /** The total amount of gems */
    int const TOTAL_GEMS = 3;
    /** The current round the player is on */
    int _currRound = 1;
    /** How many gems the player collected and won */
    int _currGems = 0;
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
    /** Whether the player has died */
    bool _died = false;
    /** Whether the player has reached the goal */
    bool _reachedGoal = false;

    std::function<void(bool)> _buildingModeCallback;

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
    bool init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::distrib::NetWorld>& world, std::shared_ptr<PlatformInput> input, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound);

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Resets the controller
     */
    void reset();

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Resets the status of the game so that we can play again.
     */
    void resetRound();

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

    void setSpriteBatch(const shared_ptr<SpriteBatch> &batch);

    void render();
    
    /**
     * Kills the player for the round.
     */
    void killPlayer();

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
     * Gets the current countdown count
     */
    int getCountdown() { return _countdown; };

    /**
     * Sets the current countdown count
     */
    void setCountdown(int value) { _countdown = value; };
    
    /**
     * Gets the object list
     */
    std::vector<std::shared_ptr<Object>> getObjects(){
        return _objects; 
    };

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
    
    void setCameraMove(){
        _movePhaseScene.resetCameraPos();
    }

#pragma mark -
#pragma mark Collision Handling
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
