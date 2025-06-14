//
//  DisconnectedScene.cpp
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 29/4/25.
//

#include <cugl/cugl.h>
#include "DisconnectedScene.h"
#include "Constants.h"
#include "SoundController.h"
#include <cugl/graphics/loaders/CUTextureLoader.h>
#include <cugl/scene2/CUScene2Loader.h>
#include <cugl/core/assets/CUWidgetLoader.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;
using namespace std;

#define SCENE_WIDTH 1306
#define SCENE_HEIGHT 576

#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  Disconnected with the default values.
     */
bool DisconnectedScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> &sound){
    
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
    
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("disconnected");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    
    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("disconnected.background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }
    _blackBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("waiting-host.black-background"));
    if (_blackBackground) {
        _blackBackground->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _blackBackground->getContentSize();
        float scale = dimen.height / tex.height;
        _blackBackground->setScale(scale, scale);
        _blackBackground->setPosition(dimen.width/2, dimen.height/2);
    }
    
    _quitbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("disconnected.quit"));
    _quitbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::QUIT;
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void DisconnectedScene::dispose(){
    reset();
    removeAllChildren();
    _background = nullptr;
    _active = false;
    _assets = nullptr;
    _sound = nullptr;
    _input.dispose();
    
    _quitbutton->clearListeners();
    _quitbutton = nullptr;
    Scene2::dispose();
    
}

/**
 * Resets all properties of the scene.
 */
void DisconnectedScene::reset(){
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
void DisconnectedScene::setActive(bool value){
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _quitbutton->activate();
        } else {
            _quitbutton->deactivate();
            _quitbutton->setDown(false);
        }
    }
}
