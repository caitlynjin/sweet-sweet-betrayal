//
//  LevelSelect.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 5/1/25.
//

#include <stdio.h>
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "LevelSelectScene.h"
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

#define LEVEL1_IMG    "level"

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
bool LevelSelectScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<NetworkController> networkController, const std::shared_ptr<SoundController> &sound 
) {
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
    
    _networkController = networkController;
    _network = networkController->getNetwork();
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> levelScene = _assets->get<scene2::SceneNode>("level-select");
    levelScene->setContentSize(dimen);
    levelScene->doLayout(); // Repositions the HUD
    
    
//    std::shared_ptr<scene2::SceneNode> menuScene = _assets->get<scene2::SceneNode>("menu");
//    levelScene->setContentSize(dimen);
//    levelScene->doLayout(); // Repositions the HUD
    
    _choice = Choice::NONE;
    
    // General level layout
    _level1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.map.x1"));
    _level2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.map.x2"));
    _level3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.map.x3"));
    _hostText = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.hosttext"));
    _backbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.back"));
    
//    _level2->setColor(Color4::GRAY);
//    _level3->setColor(Color4::GRAY);
    
    // modal buttons and assets
    
    // Program the buttons
    _level1->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _levelView = 1;
            _sound->playSound("button_click");
            _levelPressed = true;
        }
    });
    _level2->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _levelView = 2;
            _sound->playSound("button_click");
            _levelPressed = true;
        }
    });
    _level3->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _levelView = 3;
            _sound->playSound("button_click");
            _levelPressed = true;
        }
    });
    _backbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::BACK;
            _sound->playSound("button_click");
        }
    });
    
    
    _modalDarkener = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.black-background"));
    if (_modalDarkener) {
        _modalDarkener->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _modalDarkener->getContentSize();
        float scale = dimen.height / tex.height;
        _modalDarkener->setScale(scale, scale);
        _modalDarkener->setPosition(dimen.width/2, dimen.height/2);
    }
    
    _modalFrame = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.modal.modal"));
    _lvl1Img = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>("level-select.modal.placeholder-1"));
    _lvl2Img = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>("level-select.modal.gorgeous-gorges"));
    _lvl3Img = std::dynamic_pointer_cast<scene2::PolygonNode>(
        _assets->get<scene2::SceneNode>("level-select.modal.wild-winds"));
    _lvl1Name = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.modal.playground-text"));
    _lvl2Name = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.modal.gorges-text"));
    _lvl3Name = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.modal.winds-text"));
    
    _lvl1Name->setVisible(false);
    _lvl2Name->setVisible(false);
    _lvl3Name->setVisible(false);
    _lvl1Img->setVisible(false);
    _lvl2Img->setVisible(false);
    _lvl3Img->setVisible(false);
    
    _closeButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.modal.exit"));
    _playButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.modal.play"));
    
    _closeButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::NONE;
            _sound->playSound("button_click");
            _lvl1Name->setVisible(false);
            _lvl2Name->setVisible(false);
            _lvl3Name->setVisible(false);
            _lvl1Img->setVisible(false);
            _lvl2Img->setVisible(false);
            _lvl3Img->setVisible(false);
            _levelView = 0;
            _closePressed = true;
        }
    });
    
    _playButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _sound->playSound("button_click");
            
            if (_levelView == 1){
                _choice = LEVEL1;
            }
            if (_levelView == 2){
                _choice = LEVEL2;
            }
            if (_levelView == 3){
                _choice = LEVEL3;
            }
            
            _playPressed = true;
        }
    });
    
    
    setModalVisible(false);
    setModalActive(false);

    
    addChild(levelScene);
    setActive(false);
    return true;

}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelSelectScene::dispose() {
    reset();
    removeAllChildren();
    
    _background = nullptr;
    _active = false;
    _networkController = nullptr;
    _network = nullptr;
    
    _assets = nullptr;
    
    _level1->clearListeners();
    _level2->clearListeners();
    _level3->clearListeners();
    _backbutton->clearListeners();
    
    _playButton->clearListeners();
    _closeButton->clearListeners();

   _sound = nullptr;
    _input.dispose();
   _background = nullptr;
   _level1 = nullptr;
   _level2 = nullptr;
   _level3 = nullptr;
   _backbutton = nullptr;
   _hostText = nullptr;
   
   _modalDarkener = nullptr;
   _modalFrame = nullptr;
    _lvl1Img = nullptr;
    _lvl2Img = nullptr;
    _lvl3Img = nullptr;
    _lvl1Name = nullptr;
    _lvl2Name = nullptr;
    _lvl3Name = nullptr;
   _playButton = nullptr;
   _closeButton = nullptr;
    
    Scene2::dispose();
    
}

/**
 * Resets all properties of the scene.
 */
void LevelSelectScene::reset(){
    _choice = Choice::NONE;
    _levelView = 0;
    
    _playPressed = false;
    _decreaseAlpha = true;
    _levelPressed = false;
    _closePressed = false;
    _showModal = false;
    setModalVisible(false);
    
    if (_network->isHost()){
        setModalActive(false);
    }
}

// Update level event to store whether to display the modal

void LevelSelectScene::update(float dt){
    // Host handles level selection
    if (_network->isHost()){
        // If a level has been selected, show the pop-up modal
        if (_levelView != 0){
            selectLevel(_levelView);
        }

        // If the modal is currently active and level view has been reset, deactivate the modal
        if (_playButton->isActive() && _levelView == 0){
            setModalVisible(false);
            setModalActive(false);
        }
        
        // If host presses a level, show the pop-up for all clients
        if (_levelPressed){
            _network->pushOutEvent(LevelEvent::allocLevelEvent(_levelView, true, false));
            _levelPressed = false;
        }
        
        // If host pressed close modal, close the modal for all clients
        if (_closePressed){
            _network->pushOutEvent(LevelEvent::allocLevelEvent(_levelView, false, false));
            _closePressed = false;
        }
        
        // If host, send out level selected when play button pressed
        if (_playPressed){
            _network->pushOutEvent(LevelEvent::allocLevelEvent(_levelView, true, true));
            _playPressed = false;
        }
    }
    
    // Client checks for level selected by host
//    if (!_network->isHost() && (_networkController->getLevelSelected() != 0)){
//        _levelView = _networkController->getLevelSelected();
//        _choice = static_cast<Choice>(_levelView);
//    }
    
    // CLIENT-SIDE UPDATE
    if (!_network->isHost()){
        tuple<int, bool, bool> levelData = _networkController->getLevelSelectData();
        _levelView = get<0>(levelData);
        bool showModal = get<1>(levelData);
        bool hostPressedPlay = get<2>(levelData);
        
        CULog("Level View: %d", _levelView);
        CULog("Show modal %d", showModal);
        CULog("Host pressed play: %d", hostPressedPlay);
        
        // Update visibility state of the modal
        if (_levelView != 0){
            selectLevel(_levelView);
        }
        
        if (!showModal){
            setModalVisible(false);
        }

        
        // Check if host pressed play
        if (showModal && hostPressedPlay){
            _playButton->setDown(true);
            _choice = static_cast<Choice>(_levelView);
        }
    }
    
    
    // Update buttons
//    animateButtons();
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
void LevelSelectScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        _levelView = 0;
        if (value) {
            reset();
            _choice = NONE;
            if (_networkController->getIsHost()){
                _level1->activate();
                _level2->activate();
                _level3->activate();
//                _closeButton->activate();
//                _playButton->activate();
            }
            setModalVisible(false);
            setModalActive(false);
    
//            _backbutton->activate();

        } else {
            _level1->deactivate();
            _level2->deactivate();
            _level3->deactivate();
//            _backbutton->deactivate();
            setModalVisible(false);
            _closeButton->deactivate();
            _playButton->deactivate();
//            setModalActive(false);
            // If any were pressed, reset them
            _level1->setDown(false);
            _level2->setDown(false);
            _level3->setDown(false);
//            _backbutton->setDown(false);
            _closeButton->setDown(false);
            _playButton->setDown(false);
        }
    }
}

/**
 Shows the level modal for the appropriate level number
 */
void LevelSelectScene::selectLevel(int levelNum){
    _lvl1Img->setVisible(false);
    _lvl2Img->setVisible(false);
    _lvl3Img->setVisible(false);
    if (levelNum == 1){
        _lvl1Img->setVisible(true);
        _lvl2Name->setVisible(false);
        _lvl3Name->setVisible(false);
        _lvl1Name->setVisible(true);
    }
    
    if (levelNum == 2){
        _lvl2Img->setVisible(true);
        _lvl1Name->setVisible(false);
        _lvl3Name->setVisible(false);
        _lvl2Name->setVisible(true);
    }
    
    if (levelNum == 3){
        _lvl3Img->setVisible(true);
        _lvl1Name->setVisible(false);
        _lvl2Name->setVisible(false);
        _lvl3Name->setVisible(true);
    }
    
    setModalVisible(true);
    if (_network->isHost()){
        setModalActive(true);
    }
}

/**
    Sets the visibility of the modal.
 */
void LevelSelectScene::setModalVisible(bool visible){
    
    _modalDarkener->setVisible(visible);
    _modalFrame->setVisible(visible);
    _closeButton->setVisible(visible);
    _playButton->setVisible(visible);
    
    if (!visible){
        _choice = Choice::NONE;
        _lvl1Name->setVisible(false);
        _lvl2Name->setVisible(false);
        _lvl3Name->setVisible(false);
        _lvl1Img->setVisible(false);
        _lvl2Img->setVisible(false);
        _lvl3Img->setVisible(false);
        _levelView = 0;
        _closePressed = true;
    }
}

void LevelSelectScene::setModalActive(bool value){
    if (value){
        _closeButton->activate();
        _playButton->activate();
        
        _level1->deactivate();
        _level2->deactivate();
        _level3->deactivate();
//        _backbutton->deactivate();
        
        _level1->setDown(false);
        _level2->setDown(false);
        _level3->setDown(false);
        
    }
    else {
        _closeButton->deactivate();
        _playButton->deactivate();
        _closeButton->setDown(false);
        _playButton->setDown(false);
        
        _level1->activate();
        _level2->activate();
        _level3->activate();
//        _backbutton->activate();
    }
}


void LevelSelectScene::animateButtons(){
    Color4 currColor = _level1->getColor();
    float alpha = currColor.a;
    
    if (_decreaseAlpha){
        alpha -= FLASH_RATE;
        alpha = alpha < MIN_ALPHA ? MIN_ALPHA : alpha;
        if (alpha <= MIN_ALPHA){
            _decreaseAlpha = false;
        }
    }
    else{
        alpha += FLASH_RATE;
        alpha = alpha > 255 ? 255 : alpha;
        if (alpha >= MAX_ALPHA){
            _decreaseAlpha = true;
        }
    }
    
    currColor.a = alpha;
    _level1->setColor(currColor);
    _level2->setColor(currColor);
    _level3->setColor(currColor);
}

