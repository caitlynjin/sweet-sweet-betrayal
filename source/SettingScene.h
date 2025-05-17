//
// Created by chvel on 5/14/2025.
//

#ifndef SWEETSWEETBETRAYAL_SETTINGSCENE_H
#define SWEETSWEETBETRAYAL_SETTINGSCENE_H
#include <cugl/cugl.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SoundController.h"

using namespace cugl;
using namespace Constants;

/**
 * This class presents the settings screen to the player.
 */
class SettingScene : public cugl::scene2::Scene2{
public:
    /**
     * The start screen choice.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User has pressed the exit button */
        EXIT,
        /** User has pressed the help button */
        HELP,
        /** User has pressed the credits button */
        CREDITS
    };

    /** The player start screen choice */
    Choice _choice;
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<SoundController> _sound;

    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;

    std::shared_ptr<cugl::scene2::Slider> _musicSlider;
    std::shared_ptr<cugl::scene2::Slider> _sfxSlider;
    std::shared_ptr<cugl::scene2::Button> _exitButton;
    std::shared_ptr<cugl::scene2::Button> _helpButton;
    std::shared_ptr<cugl::scene2::Button> _creditsButton;


public:
#pragma mark -
#pragma mark Constructors
    SettingScene() : cugl::scene2::Scene2() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~SettingScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController>& sound);

    void update(float timestep) override;

    virtual void setActive(bool value) override;

    Choice getChoice() const { return _choice; }

};
#endif //SWEETSWEETBETRAYAL_SETTINGSCENE_H
