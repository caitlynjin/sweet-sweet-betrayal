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

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"

bool StartScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound) {
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("start");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _startbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("start.start"));
    _leveleditorbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("start.settings"));
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
            //_choice = Choice::LEVEL_EDITOR;
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

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void StartScene::dispose() {
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
