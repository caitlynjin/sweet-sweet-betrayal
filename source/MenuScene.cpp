//
//  MainMenu.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 3/5/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MenuScene.h"
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
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> &sound) {
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("menu");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    
    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("menu.background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }
    
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.center.buttons.create"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.center.buttons.join"));
    _backbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.back"));
    _text = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("menu.center.menu-title"));
    _textBasePos = _text->getPosition();
    
    // Program the buttons
    _hostbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::HOST;
            _sound->playSound("button_click");
        }
    });
    _joinbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::JOIN;
            _sound->playSound("button_click");
        }
    });
    _backbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::BACK;
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
void MenuScene::dispose() {
    reset();
    removeAllChildren();
    _background = nullptr;
    _active = false;
    
    _assets = nullptr;

    _sound = nullptr;
    _input.dispose();

    _hostbutton->clearListeners();
    _joinbutton->clearListeners();
    _backbutton->clearListeners();
    
    _hostbutton = nullptr;
    _joinbutton = nullptr;
    _backbutton = nullptr;
    _text = nullptr;
    
    Scene2::dispose();
    
}

/**
 * Resets all properties of the scene.
 */
void MenuScene::reset(){
    _choice = Choice::NONE;
    _textFloatTimer = 0.0f;
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
void MenuScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            if (_sound->getCurrentTrackPlaying() != "main_menu") {
                _sound->playMusic("main_menu", true);
            }
            
            _choice = NONE;
            _hostbutton->activate();
            _joinbutton->activate();
            _backbutton->activate();
        } else {
            _hostbutton->deactivate();
            _joinbutton->deactivate();
            _backbutton->deactivate();
            // If any were pressed, reset them
            _hostbutton->setDown(false);
            _joinbutton->setDown(false);
            _backbutton->setDown(false);
        }
    }
}

void MenuScene::update(float timestep) {
    _textFloatTimer += timestep;

    float floatAmplitude = 4.0f;
    float floatSpeed = 7.0f;

    float offsetY = floatAmplitude * std::sin(floatSpeed * _textFloatTimer);

    if (_text != nullptr) {
        _text->setPosition(_textBasePos.x, _textBasePos.y + offsetY);
    }
}
