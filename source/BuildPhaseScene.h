//
//  BuildPhaseScene.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/17/25.
//

#ifndef __SSB_BUILD_PHASE_SCENE_H__
#define __SSB_BUILD_PHASE_SCENE_H__
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
 * This class is the build phase scene.
 */
class BuildPhaseScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    /** The offset from the world */
    Vec2 _offset;
    /** The initial position of the camera */
    Vec2 _cameraInitialPos;
    /** Whether we are in level editor mode */
    bool _isLevelEditor = false;


public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    BuildPhaseScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~BuildPhaseScene() { dispose(); }

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
    bool init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<Camera> camera);

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

#pragma mark -
#pragma mark Helpers
    /**
     * Set whether the elements of this scene are visible or not
     */
    void setVisible(bool value);

    /** Sets whether or not we are in level editor mode.
    * By default, we are not.
    */
    void setLevelEditor(bool value);

    /**
     * Resets the camera position to the initial state.
     */
    void resetCameraPos();

    /**
     * Converts from screen to Box2D coordinates.
     *
     * @return the Box2D position
     *
     * @param screenPos    The screen position
     */
    Vec2 convertScreenToBox2d(const Vec2 &screenPos);

    /**
 * Converts from screen to Box2D coordinates.
 *
 * @return the Box2D position
 *
 * @param screenPos    The screen position
 * @param systemScale The scale to differentiate mobile from desktop
 */
    Vec2 convertScreenToBox2d(const Vec2& screenPos, float systemScale);


};

#endif /* __SSB_BUILD_PHASE_SCENE_H__ */
