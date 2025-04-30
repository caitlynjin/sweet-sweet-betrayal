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
public:
    /**
     * Enum representing different types of transitions
     */
    enum TransitionType {
        FADE_IN,
        FADE_OUT,
        NONE
    };
    
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** Reference to the progress bar */
    std::shared_ptr<cugl::scene2::PolygonNode> _blackScreen;
    
    /** The current type of transition **/
    TransitionType _transitionType = TransitionType::NONE;

    /** Whether the transition has finished fading in */
    bool _fadingInDone = false;
    
    /** Whether the transition has finished fading out */
    bool _fadingOutDone = false;
    
    /** The rate of fading */
    float const FADE_RATE = 16.0f;
    /** The duration of a fade transition */
    float const FADE_TIME = 100.0f;
    
    /** The current progression of the fade transition */
    float _fadeTime = 0.0f;

    
    

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
    virtual void dispose() override;

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
    virtual void reset() override;

    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);
    
    virtual void setActive(bool value) override;
    
    void startFadeOut();
    
    void startFadeIn();
    
    bool getFadingInDone(){
        return _fadingInDone;
    }
    
    bool getFadingOutDone(){
        return _fadingOutDone;
    }


#pragma mark -
#pragma mark Attribute Functions


};

#endif /* TransitionScene_h */
