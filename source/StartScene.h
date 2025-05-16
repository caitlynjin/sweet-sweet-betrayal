//
//  StartScene.h
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 25/3/25.
//

#ifndef __START_SCENE_H__
#define __START_SCENE_H__
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
 * This class presents the start screen to the player.
 */
class StartScene : public cugl::scene2::Scene2 {
public:
    /**
     * The start screen choice.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to start a game */
        START,
        LEVEL_EDITOR,
        SETTING
    };

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<SoundController> _sound;
    
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    
    std::shared_ptr<cugl::scene2::Button> _startbutton;
    std::shared_ptr<cugl::scene2::Button> _settingsbutton;
    std::shared_ptr<cugl::scene2::Button> _leveleditorbutton;
    std::shared_ptr<cugl::scene2::PolygonNode> _leftglider;
    std::shared_ptr<cugl::scene2::PolygonNode> _rightglider;
    
    /** Glider image variables */
    float _gliderFloatTimer = 0.0f;
    cugl::Vec2 _leftBasePos;
    cugl::Vec2 _rightBasePos;
    
    /** The player start screen choice */
    Choice _choice;

public:
#pragma mark -
#pragma mark Constructors
    StartScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~StartScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController>& sound
    );
    
    void update(float timestep) override;
    
    virtual void setActive(bool value) override;
    
    Choice getChoice() const { return _choice; }

};

#endif
