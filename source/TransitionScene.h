//
//  TransitionScene.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/29/25.
//

#ifndef TransitionScene_h
#define TransitionScene_h

#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;

/**
 * This class is the build phase UI scene.
 */
class TransitionScene : public scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** Reference to the progress bar */
    std::shared_ptr<cugl::scene2::PolygonNode> _fade;

    /** Whether the transition has finished fading out */
    bool _fadeOutDone = false;
    /** Whether the transition has finished fading in */
    bool _fadeInDone = false;

    
    

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    TransitionScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~TransitionScene() { dispose(); }

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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);


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


};

#endif /* TransitionScene_h */
