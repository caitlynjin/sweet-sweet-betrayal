//
//  WaitingHostScene.cpp
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 28/4/25.
//

#include <cugl/cugl.h>
#include "WaitingHostScene.h"
#include "Constants.h"
#include "SoundController.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace std;

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  waiting host scene with the default values.
     */
bool WaitingHostScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound){
    
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
    
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("waiting-host");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    
    _cancelbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("waiting-host.cancel"));
    _cancelbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::CANCEL;
        }
    });
    
    _gemanimation = 
    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void WaitingHostScene::dispose(){
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
void WaitingHostScene::reset(){
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
void WaitingHostScene::setActive(bool value){
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _cancelbutton->activate();
        } else {
            _cancelbutton->deactivate();
            _cancelbutton->setDown(false);
        }
    }
}
