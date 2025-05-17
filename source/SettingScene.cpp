//
// Created by chvel on 5/14/2025.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "SettingScene.h"
#include "SSBGameController.h"
#include "Constants.h"
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

bool SettingScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound) {
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("setting");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;

    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("setting.black-background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }

    _musicSlider = std::dynamic_pointer_cast<scene2::Slider>(_assets->get<scene2::SceneNode>("setting.modal.musicslider"));
    _sfxSlider = std::dynamic_pointer_cast<scene2::Slider>(_assets->get<scene2::SceneNode>("setting.modal.sfxslider"));
    _exitButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("setting.modal.exit"));
    _helpButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("setting.modal.help"));
    _creditsButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("setting.modal.credits"));

    _musicSlider->setBounds(Rect(40, _musicSlider->getBounds().getMinY(), _musicSlider->getBounds().getMaxX() - 40, 0));
    _sfxSlider->setBounds(Rect(40, _sfxSlider->getBounds().getMinY(), _sfxSlider->getBounds().getMaxX() - 40, 0));

    _musicSlider->addListener([this](const std::string& name, float value) {
        _sound->setMusicVolume(value / 100, true);
    });
    _sfxSlider->addListener([this](const std::string& name, float value){
        _sound->setSFXVolume(value / 100, true);
    });
    _exitButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::EXIT;
            _sound->playSound("button_click");
        }
    });
    _helpButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::HELP;
            _sound->playSound("button_click");
        }
    });
    _creditsButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::CREDITS;
            _sound->playSound("button_click");
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

void SettingScene::update(float timestep) {

}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void SettingScene::dispose() {
    if (_active) {
        removeAllChildren();
        _background = nullptr;
        _active = false;
        Scene2::dispose();
    }
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
void SettingScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _musicSlider->activate();
            _sfxSlider->activate();
            _exitButton->activate();
            _helpButton->activate();
            _creditsButton->activate();
        } else {
            _musicSlider->deactivate();
            _sfxSlider->deactivate();
            _exitButton->deactivate();
            _creditsButton->deactivate();
            _helpButton->deactivate();
            _exitButton->setDown(false);
            _helpButton->setDown(false);
            _creditsButton->setDown(false);
        }
    }
}