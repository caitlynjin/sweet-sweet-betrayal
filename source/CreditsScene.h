//
// Created by chvel on 5/15/2025.
//

#ifndef SWEETSWEETBETRAYAL_CREDITSSCENE_H
#define SWEETSWEETBETRAYAL_CREDITSSCENE_H

#include <cugl/cugl.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SoundController.h"
//#include <cmath>

using namespace cugl;
using namespace Constants;

/**
 * This class presents the credits screen to the player.
 */
class CreditsScene : public cugl::scene2::Scene2{
public:
    /**
     * The credits screen choice.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to start a game */
        BACK
    };

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<SoundController> _sound;

    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;

    std::shared_ptr<scene2::Button> _backButton;

    Choice _choice;

public:
#pragma mark -
#pragma mark Constructors
    CreditsScene() : cugl::scene2::Scene2() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~CreditsScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound);

    void update(float timestep) override;

    virtual void setActive(bool value) override;

    Choice getChoice() const { return _choice; }
};

#endif //SWEETSWEETBETRAYAL_CREDITSSCENE_H
