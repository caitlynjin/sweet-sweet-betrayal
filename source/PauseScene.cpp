//
//  PauseScene.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 5/13/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "PauseScene.h"
#include "SSBGameController.h"
#include "Constants.h"
#include "Platform.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "PlayerModel.h"
#include "WindObstacle.h"
#include "SoundController.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576
/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"

#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * In previous labs, this method "started" the scene.  But in this
 * case, we only use to initialize the scene user interface.  We
 * do not activate the user interface yet, as an active user
 * interface will still receive input EVEN WHEN IT IS HIDDEN.
 *
 * That is why we have the method {@link #setActive}.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool PauseScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound) {
    if (assets == nullptr) {
       return false;
   }
    if (sound == nullptr) {
        return false;
    }

   if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT))) {
       return false;
   }

    Size dimen = getSize();

    // Start up the input handler
    _assets = assets;

    _sound = sound;

    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("pause");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _modal = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.modal"));
    _musicLabel = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.music"));
    _musicSlider = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.musicslider.slider"));
    _musicKnob = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.musicslider.knob"));
    _sfxLabel = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.sfx"));
    _sfxSlider = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.sfxslider.slider"));
    _sfxKnob = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.sfxslider.knob"));
    _disconnectButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.buttons.disconnect"));
    _resumeButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.buttons.resume"));

    // Program the buttons
    _disconnectButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::DISCONNECT;
            _sound->playSound("button_click");
        }
    });
    _resumeButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::RESUME;
            _sound->playSound("button_click");
        }
    });

    addChild(scene);
    setActive(false);
    return true;

}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void PauseScene::dispose() {
    if (_active) {
        removeAllChildren();
        _background = nullptr;
        _active = false;
        Scene2::dispose();
    }
}

/**
 * Resets all properties of the scene.
 */
void PauseScene::reset(){
    _choice = Choice::NONE;
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void PauseScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _disconnectButton->activate();
            _resumeButton->activate();
        } else {
            _disconnectButton->deactivate();
            _resumeButton->deactivate();
            // If any were pressed, reset them
            _disconnectButton->setDown(false);
            _resumeButton->setDown(false);
        }
    }
}

void PauseScene::update(float timestep) {

}
