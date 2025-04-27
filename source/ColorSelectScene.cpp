//
//  ColorSelectScene.cpp
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 27/4/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "ColorSelectScene.h"
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
 * Initialize this scene with assets and sound controller.
 *
 * @param assets  Loaded asset manager
 * @param sound   Sound controller
 * @return true if successful
 */
bool ColorSelectScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController>& sound) {
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("colorselect");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _backbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.back"));
    
    _backbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::BACK;
        }
    });
    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void ColorSelectScene::dispose() {
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
void ColorSelectScene::reset(){
    _choice = Choice::NONE;
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void ColorSelectScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _backbutton->activate();
        } else {
            _backbutton->deactivate();
            // If any were pressed, reset them
            _backbutton->setDown(false);
        }
    };
}
