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

#define SCENE_WIDTH 1024
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
bool LevelSelectScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<NetworkController> networkController, const std::shared_ptr<SoundController> sound) {
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
    std::shared_ptr<scene2::SceneNode> levelScene = _assets->get<scene2::SceneNode>("level-select");
    levelScene->setContentSize(dimen);
    levelScene->doLayout(); // Repositions the HUD
    
    
//    std::shared_ptr<scene2::SceneNode> menuScene = _assets->get<scene2::SceneNode>("menu");
//    levelScene->setContentSize(dimen);
//    levelScene->doLayout(); // Repositions the HUD
    
    _choice = Choice::NONE;
    
    // General level layout
    _level1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.x1"));
    _level2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.x2"));
    _level3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.x3"));
    _hostText = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.hosttext"));
    _backbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.back"));
    
    
    // modal buttons and assets
    
    // Program the buttons
    _level1->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::LEVEL1;
            _sound->playSound("button_click");
            selectLevel(1);
        }
    });
    _level2->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::LEVEL2;
            _sound->playSound("button_click");
            selectLevel(2);
        }
    });
    _level3->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::LEVEL3;
            _sound->playSound("button_click");
            selectLevel(3);
        }
    });
    _backbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::BACK;
            _sound->playSound("button_click");
        }
    });
    
    
    _modalDarkener = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.black-background"));
    _modalFrame = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.modal"));
    _levelImage = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.placeholder-1"));
    _levelName = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("level-select.playground-text"));
    _closeButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.exit"));
    _playButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level-select.play"));
    
    _closeButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _sound->playSound("button_click");
            setModalVisible(false);
        }
    });
    
    _playButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _sound->playSound("button_click");
        }
    });
    
    
    setModalVisible(false);

    
    addChild(levelScene);
    setActive(false);
    return true;

}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelSelectScene::dispose() {
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
void LevelSelectScene::reset(){
    _choice = Choice::NONE;
}

void LevelSelectScene::update(float dt){
    
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
        if (value) {
            _choice = NONE;
            _level1->activate();
            _level2->activate();
            _level3->activate();
            _backbutton->activate();
//            _closeButton->activate();
//            _playButton->activate();
        } else {
            _level1->deactivate();
            _level2->deactivate();
            _level3->deactivate();
            _backbutton->deactivate();
            _closeButton->deactivate();
            _playButton->deactivate();
            // If any were pressed, reset them
            _level1->setDown(false);
            _level2->setDown(false);
            _level3->setDown(false);
            _backbutton->setDown(false);
            _closeButton->setDown(false);
            _playButton->setDown(false);
        }
    }
}

/**
 Shows the level modal for the appropriate level number
 */
void LevelSelectScene::selectLevel(int levelNum){
    if (levelNum == 1){
        _levelImage->setTexture(_assets->get<Texture>("placeholder-1"));
        _levelName->setTexture(_assets->get<Texture>("playground-text"));
    }
    
    if (levelNum == 2){
        _levelImage->setTexture(_assets->get<Texture>("gorges-img"));
        _levelName->setTexture(_assets->get<Texture>("gorges-text"));
    }
    
    if (levelNum == 3){
        _levelImage->setTexture(_assets->get<Texture>("winds-img"));
        _levelName->setTexture(_assets->get<Texture>("winds-text"));
    }
    
    setModalVisible(true);
}

/**
    Sets the visibility of the modal.
 */
void LevelSelectScene::setModalVisible(bool visible){
    
    if (visible){
        _closeButton->activate();
        _playButton->activate();
        
        _level1->deactivate();
        _level2->deactivate();
        _level3->deactivate();
        _backbutton->deactivate();
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
        _backbutton->activate();
    }
    
    _modalDarkener->setVisible(visible);
    _modalFrame->setVisible(visible);
    _levelImage->setVisible(visible);
    _levelName->setVisible(visible);
    _closeButton->setVisible(visible);
    _playButton->setVisible(visible);
}

