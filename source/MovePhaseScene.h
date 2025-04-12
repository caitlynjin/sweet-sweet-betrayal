//
//  MovePhaseScene.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/18/25.
//

#ifndef __SSB_MOVE_PHASE_SCENE_H__
#define __SSB_MOVE_PHASE_SCENE_H__
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

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;

/**
 * This class is the build phase scene.
 */
class MovePhaseScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::distrib::NetWorld> _world;
    std::shared_ptr<ObjectController> _objectController;
    std::shared_ptr<GridManager> _gridManager;
    /** The network controller */
    std::shared_ptr<NetworkController> _networkController;
    /** The network  */
    std::shared_ptr<NetEventController> _network;
    /** A list of all objects to be updated during each animation frame. */
    std::vector<std::shared_ptr<Object>>* _objects;

    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _debugnode;
    /** Reference to the goalDoor (for collision detection) */
    std::shared_ptr<physics2::BoxObstacle>    _goalDoor;
    /** Reference to the local player */
    std::shared_ptr<PlayerModel> _localPlayer;
    /** Reference to the treasure */
    std::shared_ptr<Treasure> _treasure;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    /** The offset from the world */
    Vec2 _offset;
    /** The initial position of the camera */
    Vec2 _initialCameraPos;


public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    MovePhaseScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MovePhaseScene() { dispose(); }

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
    bool init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::distrib::NetWorld>& world, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController, std::vector<std::shared_ptr<Object>>* objects);

    /**
     * Lays out the game geography.
     *
     * Pay close attention to how we attach physics objects to a scene graph.
     * The simplest way is to make a subclass, like we do for the Dude.  However,
     * for simple objects you can just use a callback function to lightly couple
     * them.  This is what we do with the crates.
     *
     * This method is really, really long.  In practice, you would replace this
     * with your serialization loader, which would process a level file.
     */
    void populate();

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
     * Gets the scale of the world
     */
    float getScale() { return _scale; };

    /**
     * Gets the offset of the grid for the world
     */
    Vec2 getOffset() { return _offset; };

    /**
     * Gets the camera for the move phase scene
     */
    std::shared_ptr<Camera> getCamera() { return _camera; };

    /**
     * Gets the object controller
     */
    std::shared_ptr<ObjectController> getObjectController() { return _objectController; };

    /**
     * Gets the local player
     */
    std::shared_ptr<PlayerModel> getLocalPlayer() { return _localPlayer; };

    /**
     * Gets the treasure
     */
    std::shared_ptr<Treasure> getTreasure() { return _treasure; };

    /**
     * Gets the goal door
     */
    std::shared_ptr<physics2::BoxObstacle> getGoalDoor() { return _goalDoor; };

#pragma mark -
#pragma mark Helpers
    /**
     * Set whether the debug node is visible
     */
    void setDebugVisible(bool value);

    /**
     * Resets the camera position to the initial state.
     */
    void resetCameraPos();

    /**
     * Resets the player properties at the end of a round.
     */
    void resetPlayerProperties();

    /**
     * Set the next position for the treasure based on the current gem count.
     */
    void setNextTreasure(int count);

    /**
     * Converts from screen to Box2D coordinates.
     *
     * @return the Box2D position
     *
     * @param screenPos    The screen position
     */
    Vec2 convertScreenToBox2d(const Vec2 &screenPos);

    /**
     * This method links a scene node to the obstacle.
     *
     * This method adds a listener so that the sceneNode will move along with the obstacle.
     */
    void linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj, const std::shared_ptr<scene2::SceneNode>& node);

};

#endif /* __SSB_MOVE_PHASE_SCENE_H__ */
