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
bool ColorSelectScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> networkController, const std::shared_ptr<SoundController>& sound) {
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
    _networkController = networkController;
    _network = networkController->getNetwork();
    _networkController->setOnColorTaken([this](ColorType newColor, int oldColor){
        updateColorTaken(newColor, oldColor);
    });
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("colorselect");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _backbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.back"));
    _readybutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.ready"));
    _isReady = false;
    
    _redbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.color-buttons.red"));
    _redNormal   = std::dynamic_pointer_cast<scene2::PolygonNode>(_redbutton->getChildByName("red-button"));
    _redSelected = std::dynamic_pointer_cast<scene2::PolygonNode>(_redbutton->getChildByName("red-selected"));
    _redTaken = std::dynamic_pointer_cast<scene2::PolygonNode>(_redbutton->getChildByName("red-taken"));
    _redNormal->setVisible(true);
    _redSelected->setVisible(false);
    _redTaken->setVisible(false);
    
    _bluebutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.color-buttons.blue"));
    _blueNormal   = std::dynamic_pointer_cast<scene2::PolygonNode>(_bluebutton->getChildByName("blue-button"));
    _blueSelected = std::dynamic_pointer_cast<scene2::PolygonNode>(_bluebutton->getChildByName("blue-selected"));
    _blueTaken = std::dynamic_pointer_cast<scene2::PolygonNode>(_bluebutton->getChildByName("blue-taken"));
    _blueNormal->setVisible(true);
    _blueSelected->setVisible(false);
    _blueTaken->setVisible(false);
    
    _yellowbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.color-buttons.yellow"));
    _yellowNormal   = std::dynamic_pointer_cast<scene2::PolygonNode>(_yellowbutton->getChildByName("yellow-button"));
    _yellowSelected = std::dynamic_pointer_cast<scene2::PolygonNode>(_yellowbutton->getChildByName("yellow-selected"));
    _yellowTaken = std::dynamic_pointer_cast<scene2::PolygonNode>(_yellowbutton->getChildByName("yellow-taken"));
    _yellowNormal->setVisible(true);
    _yellowSelected->setVisible(false);
    _yellowTaken->setVisible(false);
    
    _greenbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("colorselect.color-buttons.green"));
    _greenNormal   = std::dynamic_pointer_cast<scene2::PolygonNode>(_greenbutton->getChildByName("green-button"));
    _greenSelected = std::dynamic_pointer_cast<scene2::PolygonNode>(_greenbutton->getChildByName("green-selected"));
    _greenTaken = std::dynamic_pointer_cast<scene2::PolygonNode>(_greenbutton->getChildByName("green-taken"));
    _greenNormal->setVisible(true);
    _greenSelected->setVisible(false);
    _greenTaken->setVisible(false);
    
    _backbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::BACK;
            _sound->playSound("button_click");

        }
    });
    _readybutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            CULog("Pressed ready");
            _choice = Choice::READY;
            _networkController->flushConnection();
            _networkController->setLocalColor(_myColor);
            CULog("Sent out color ready message");
            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::COLOR_READY));
            _isReady = true;
            _sound->playSound("button_click");
        }
    });
    _redbutton->addListener([this](const std::string& name, bool down) {
        if (!down && !_taken[int(ColorType::RED)]) {
            _myColor = ColorType::RED;
            _network->pushOutEvent(ColorEvent::allocColorEvent(_network->getShortUID(), _myColor, _prevTakenIndex));
            updateSelectedColor(_myColor);
            _sound->playSound("redSelect");
        }
    });
    _bluebutton->addListener([this](const std::string& name, bool down) {
        if (!down && !_taken[int(ColorType::BLUE)]) {
            _myColor = ColorType::BLUE;
            _network->pushOutEvent(ColorEvent::allocColorEvent(_network->getShortUID(), _myColor, _prevTakenIndex));
            updateSelectedColor(_myColor);
            _sound->playSound("blueSelect");
        }
    });
    _yellowbutton->addListener([this](const std::string& name, bool down) {
        if (!down && !_taken[int(ColorType::YELLOW)]) {
            _myColor = ColorType::YELLOW;
            _network->pushOutEvent(ColorEvent::allocColorEvent(_network->getShortUID(), _myColor, _prevTakenIndex));
            updateSelectedColor(_myColor);
            _sound->playSound("yellowSelect");
        }
    });
    _greenbutton->addListener([this](const std::string& name, bool down) {
        if (!down && !_taken[int(ColorType::GREEN)]) {
            _myColor = ColorType::GREEN;
            _network->pushOutEvent(ColorEvent::allocColorEvent(_network->getShortUID(), _myColor, _prevTakenIndex));
            updateSelectedColor(_myColor);
            _sound->playSound("greenSelect");
        }
    });
    
    _redBasePos    = _redbutton->getPosition();
    _blueBasePos   = _bluebutton->getPosition();
    _yellowBasePos = _yellowbutton->getPosition();
    _greenBasePos  = _greenbutton->getPosition();

    _bounceTimer = 0.0f;
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
    _taken = {false,false,false,false};
    _myColor = ColorType::RED;
    resetButtons();
    _isReady = false;
    setReadyEnabled(false);
    _initialPlayerCount = 0;
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
            // Deactivate ready button at first because no colors have been selected yet
            _isReady = false;
            setReadyEnabled(false);
            _redbutton->activate();
            _bluebutton->activate();
            _yellowbutton->activate();
            _greenbutton->activate();
        } else {
            _backbutton->deactivate();
            _readybutton->deactivate();
            _redbutton->deactivate();
            _bluebutton->deactivate();
            _yellowbutton->deactivate();
            _greenbutton->deactivate();
            // If any were pressed, reset them
            _backbutton->setDown(false);
            _readybutton->setDown(false);
            _redbutton->setDown(false);
            _bluebutton->setDown(false);
            _yellowbutton->setDown(false);
            _greenbutton->setDown(false);
            _bounceTimer    = 0.0f;
            _prevTakenIndex = -1;
            _redbutton  ->setPosition(_redBasePos);
            _bluebutton ->setPosition(_blueBasePos);
            _yellowbutton->setPosition(_yellowBasePos);
            _greenbutton->setPosition(_greenBasePos);
        }
    };
}

void ColorSelectScene::update(float dt) {
    _bounceTimer += dt;
    const float iconAmplitude = 5.0f;
    const float iconSpeed     = 4.0f;
    float raw  = iconAmplitude * std::sin(_bounceTimer * iconSpeed);
    float dy   = -raw;

    auto applyButtonDiff = [&](std::shared_ptr<cugl::scene2::Button> btn, const cugl::Vec2& basePos, int idx){
        if (idx == _prevTakenIndex) {
            btn->setPosition(basePos.x, basePos.y + dy);
        } else {
            btn->setPosition(basePos.x, basePos.y);
        }
    };

    applyButtonDiff(_redbutton,    _redBasePos,    (int)ColorType::RED);
    applyButtonDiff(_bluebutton,   _blueBasePos,   (int)ColorType::BLUE);
    applyButtonDiff(_yellowbutton, _yellowBasePos, (int)ColorType::YELLOW);
    applyButtonDiff(_greenbutton,  _greenBasePos,  (int)ColorType::GREEN);

    Scene2::update(dt);
}

/** Visually and logically disable a color button */
void ColorSelectScene::updateColorTaken(ColorType newColor, int oldColorInt) {
    clearTaken(oldColorInt);
    _taken[(int)newColor] = true;
    switch(newColor) {
        case ColorType::RED:
            _redTaken->setVisible(true);
            _redSelected->setVisible(false);
            _redNormal->setVisible(false);
            _redbutton->deactivate();
            break;
        case ColorType::BLUE:
            _blueTaken->setVisible(true);
            _blueSelected->setVisible(false);
            _blueNormal->setVisible(false);
            _bluebutton->deactivate();
            break;
        case ColorType::GREEN:
            _greenTaken->setVisible(true);
            _greenSelected->setVisible(false);
            _greenNormal->setVisible(false);
            _greenbutton->deactivate();
            break;
        case ColorType::YELLOW:
            _yellowTaken->setVisible(true);
            _yellowSelected->setVisible(false);
            _yellowNormal->setVisible(false);
            _yellowbutton->deactivate();
            break;
  }
}

/** Visually select a color button */
void ColorSelectScene::updateSelectedColor(ColorType c) {
    if (_prevTakenIndex >= 0 && _prevTakenIndex != int(c)) {
        _taken[_prevTakenIndex] = false;
    }
    _taken[int(c)] = true;
    _redNormal->setVisible(true); _redSelected->setVisible(false);
    _blueNormal->  setVisible(true); _blueSelected->  setVisible(false);
    _yellowNormal->setVisible(true); _yellowSelected->setVisible(false);
    _greenNormal-> setVisible(true); _greenSelected-> setVisible(false);
    switch(c) {
        case ColorType::RED:
            _redSelected->setVisible(true);
            _redNormal->setVisible(false);
            break;
        case ColorType::BLUE:
            _blueSelected->setVisible(true);
            _blueNormal->setVisible(false);
            break;
        case ColorType::GREEN:
            _greenSelected->setVisible(true);
            _greenNormal->setVisible(false);
            break;
        case ColorType::YELLOW:
            _yellowSelected->setVisible(true);
            _yellowNormal->setVisible(false);
            break;
    }
    _prevTakenIndex = int(c);
    if (!_readybutton->isActive()){
        setReadyEnabled(true);
    }
}

void ColorSelectScene::clearTaken(int oldColorInt){
    if (oldColorInt >= 0){
        _taken[oldColorInt] = false;
        switch(oldColorInt) {
            case (int)ColorType::RED:
                _redTaken->setVisible(false);
                _redNormal->setVisible(true);
                _redbutton->activate();
                break;
            case (int)ColorType::BLUE:
                _blueTaken->setVisible(false);
                _blueNormal->setVisible(true);
                _bluebutton->activate();
                break;
            case (int)ColorType::GREEN:
                _greenTaken->setVisible(false);
                _greenNormal->setVisible(true);
                _greenbutton->activate();
                break;
            case (int)ColorType::YELLOW:
                _yellowTaken->setVisible(false);
                _yellowNormal->setVisible(true);
                _yellowbutton->activate();
                break;
        }
    }
}

void ColorSelectScene::resetButtons(){
    _redNormal->setVisible(true); _redSelected->setVisible(false); _redTaken->setVisible(false);
    _blueNormal->  setVisible(true); _blueSelected->  setVisible(false); _blueTaken->setVisible(false);
    _yellowNormal->setVisible(true); _yellowSelected->setVisible(false); _yellowTaken->setVisible(false);
    _greenNormal-> setVisible(true); _greenSelected-> setVisible(false); _greenTaken->setVisible(false);
}

/** Visually change the ready button depending on the input */
void ColorSelectScene::setReadyEnabled(bool enable) {
    if (enable){
        _readybutton->activate();
    } else{
        _readybutton->deactivate();
    }
    Color4 tint = enable ? Color4::WHITE : Color4(161,161,161,255);
    _readybutton->setColor(tint);
}
