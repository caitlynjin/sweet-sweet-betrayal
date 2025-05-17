//
//  PauseScene.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 5/13/25.
//
#ifndef __SSB_PAUSE_SCENE_H__
#define __SSB_PAUSE_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "PlayerModel.h"
#include "Platform.h"
#include "WindObstacle.h"
#include "Treasure.h"
#include "SoundController.h"

using namespace cugl;
using namespace Constants;

/**
 * This class presents the menu to the player.
 *
 * There is no need for an input controller, as all input is managed by
 * listeners on the scene graph.  We only need getters so that the main
 * application can retrieve the state and communicate it to other scenes.
 */
class PauseScene : public cugl::scene2::Scene2 {
public:
    /**
     * The menu choice.
     *
     * This state allows the top level application to know what the user
     * chose.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to disconnect a game */
        DISCONNECT,
        /** User wants to resume a game */
        RESUME
    };

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<SoundController> _sound;

    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    /** Reference to the music slider */
    std::shared_ptr<scene2::Slider> _musicSlider;
    /** Reference to the sfx slider */
    std::shared_ptr<scene2::Slider> _sfxSlider;
    /** The menu button for disconnecting a game */
    std::shared_ptr<cugl::scene2::Button> _disconnectButton;
    /** The menu button for resuming a game */
    std::shared_ptr<cugl::scene2::Button> _resumeButton;
    /** The player menu choice */
    Choice _choice;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  menu scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    PauseScene() : cugl::scene2::Scene2() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~PauseScene() { dispose(); }
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
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
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

    void update(float timestep) override;

    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }

};

#endif /* __SSB_PAUSE_SCENE_H__ */
