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

#define SCENE_WIDTH 1306
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
bool PauseScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> &sound) {
    if (assets == nullptr) {
       return false;
   }
    if (sound == nullptr) {
        return false;
    }

   if (!Scene2::initWithHint(Size(SCENE_WIDTH, 0))) {
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

    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("pause.black-background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }

    _choice = Choice::NONE;
    _musicSlider = std::dynamic_pointer_cast<scene2::Slider>(_assets->get<scene2::SceneNode>("pause.modal.musicslider"));
    _sfxSlider = std::dynamic_pointer_cast<scene2::Slider>(_assets->get<scene2::SceneNode>("pause.modal.sfxslider"));
    _disconnectButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.modal.disconnect"));
    _resumeButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pause.modal.resume"));

    // Program the buttons
    _musicSlider->setBounds(Rect(35, _musicSlider->getBounds().getMinY(), _musicSlider->getBounds().getMaxX() - 35, 0));
    _sfxSlider->setBounds(Rect(35, _sfxSlider->getBounds().getMinY(), _sfxSlider->getBounds().getMaxX() - 35, 0));

    _musicSlider->addListener([this](const std::string& name, float value) {
        _sound->setMusicVolume(value / 100, true);
    });
    _sfxSlider->addListener([this](const std::string& name, float value){
        _sound->setSFXVolume(value / 100, true);
        _sound->playSound("button_click");
    });

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
            _musicSlider->activate();
            _sfxSlider->activate();
            _disconnectButton->activate();
            _resumeButton->activate();
        } else {
            _musicSlider->deactivate();
            _sfxSlider->deactivate();
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
