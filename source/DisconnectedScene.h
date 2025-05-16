//
//  DisconnectedScene.h
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 29/4/25.
//

#ifndef __DISCONNECTED_SCENE_H__
#define __DISCONNECTED_SCENE_H__

#include <cugl/cugl.h>
#include "Constants.h"
#include "SSBInput.h"
#include "SoundController.h"
#include <cugl/graphics/loaders/CUTextureLoader.h>
#include <cugl/scene2/CUScene2Loader.h>
#include <cugl/core/assets/CUWidgetLoader.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;
using namespace Constants;

/**
 * This class presents the Disconnected screen to the player
 */
class DisconnectedScene : public cugl::scene2::Scene2 {
    
public:
    /**
     * The scene choice.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to quit */
        QUIT
    };
    
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    std::shared_ptr<SoundController> _sound;
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    /** The menu button for cancelling */
    std::shared_ptr<cugl::scene2::Button> _quitbutton;
    
    /** The player menu choice */
    Choice _choice;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new Disconnected scene with the default values.
     */
    DisconnectedScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~DisconnectedScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Resets all properties of the scene.
     */
    void reset() override;
    
    /**
     * Initializes the controller contents.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param sound     The sound controller
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> &sound);
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }
};

#endif /* __DISCONNECTED_SCENE_H__ */
