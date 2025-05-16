//
//  StartScene.cpp
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 25/3/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "StartScene.h"
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

/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"

bool StartScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController>& sound
) {
    if (assets == nullptr) {
       return false;
   }
    if (sound == nullptr) {
        return false;
    }

   if (!Scene2::initWithHint(SCENE_WIDTH, 0)) {
       return false;
   }
        
    // Start up the input handler
    _assets = assets;

    _sound = sound;
    
    // Acquire the scene built by the asset loader and resize it the scene
    Size dimen = getSize();
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("start");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("start.background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }
    _choice = Choice::NONE;
    _startbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("start.center.start"));
    _leveleditorbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("start.center.settings"));
    _leftglider = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("start.leftgliders"));
    _rightglider = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("start.rightgliders"));
    _leftBasePos = _leftglider->getPosition();
    _rightBasePos = _rightglider->getPosition();
//    _leveleditorbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("start.button_container.help"));
    
    // Program the buttons
    _startbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::START;
            _sound->playSound("button_click");
        }
    });
    
    _leveleditorbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::SETTING;
            _sound->playSound("button_click");
        }
    });
    addChild(scene);
    setActive(false);
    return true;

}

void StartScene::update(float timestep) {
    _gliderFloatTimer += timestep;

    float floatAmplitude = 5.5f;
    float floatSpeed = 6.0f;

    float offsetY = floatAmplitude * std::sin(floatSpeed * _gliderFloatTimer);

    if (_leftglider != nullptr) {
        _leftglider->setPosition(_leftBasePos.x, _leftBasePos.y + offsetY);
    }
    if (_rightglider != nullptr) {
        _rightglider->setPosition(_rightBasePos.x, _rightBasePos.y + offsetY);
    }
}

void StartScene::reset(){
    _choice = Choice::NONE;
    _gliderFloatTimer = 0.0f;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void StartScene::dispose() {
    reset();
    removeAllChildren();
    _background = nullptr;
    _active = false;
    
    _assets = nullptr;

    _sound = nullptr;
    _input.dispose();
    
    _startbutton->clearListeners();
//    _settingsbutton->clearListeners();
    _leveleditorbutton->clearListeners();
    _startbutton = nullptr;
    _settingsbutton = nullptr;
    _leveleditorbutton = nullptr;
    
    
    _leftglider = nullptr;
    _rightglider = nullptr;
    
    Scene2::dispose();
    
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
void StartScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _startbutton->activate();
            _leveleditorbutton->activate();
        } else {
            _startbutton->deactivate();
            _leveleditorbutton->deactivate();
            // If any were pressed, reset them
            _startbutton->setDown(false);
            _leveleditorbutton->setDown(false);
        }
    }
}
