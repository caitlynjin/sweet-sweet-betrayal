//
// Created by chvel on 5/16/2025.
//

#ifndef SWEETSWEETBETRAYAL_HELPSCENE_H
#define SWEETSWEETBETRAYAL_HELPSCENE_H

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
 * This class presents the help screen to the player.
 */
class HelpScene : public cugl::scene2::Scene2{
public:
    /**
     * The help screen choice.
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

    std::shared_ptr<scene2::PolygonNode> _obstacles;
    std::shared_ptr<scene2::PolygonNode> _building;
    std::shared_ptr<scene2::PolygonNode> _racing;
    std::shared_ptr<scene2::PolygonNode> _scoring;
    std::shared_ptr<scene2::PolygonNode> _stealing;

    std::vector<std::shared_ptr<scene2::PolygonNode>> _tutorials;
    int _curTutorial = 0;

    std::shared_ptr<scene2::Button> _leftButton;
    std::shared_ptr<scene2::Button> _rightButton;
    std::shared_ptr<scene2::Button> _backButton;

    Choice _choice;

public:
#pragma mark -
#pragma mark Constructors
    HelpScene() : cugl::scene2::Scene2() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~HelpScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound);

    void update(float timestep) override;

    virtual void setActive(bool value) override;

    Choice getChoice() const { return _choice; }
};

#endif //SWEETSWEETBETRAYAL_HELPSCENE_H
