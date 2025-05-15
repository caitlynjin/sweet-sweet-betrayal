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
#include <cugl/graphics/loaders/CUTextureLoader.h>
#include <cugl/scene2/CUScene2Loader.h>
#include <cugl/core/assets/CUWidgetLoader.h>

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;
using namespace std;

#define SCENE_WIDTH 1306
#define SCENE_HEIGHT 576
#define DURATION 1.0f
#define ACT_KEY  "current"

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

   if (!Scene2::initWithHint(Size(SCENE_WIDTH, 0))) {
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
    
    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("waiting-host.background"));
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
    
    _backbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("waiting-host.back"));
    _backbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::CANCEL;
        }
    });
    _waitSpriteNode = std::dynamic_pointer_cast<scene2::SpriteNode>(_assets->get<scene2::SceneNode>("waiting-host.fourgems"));
    _timeline = ActionTimeline::alloc();
    
    std::vector<int> forward;
    for (int ii = 0; ii < 20; ii++) {
       forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _waitAnimateSprite = AnimateSprite::alloc(forward);
    _waitAction = _waitAnimateSprite->attach<scene2::SpriteNode>(_waitSpriteNode);
    
    _tips.push_back(_assets->get<scene2::SceneNode>("waiting-host.tip1"));
    _tips.push_back(_assets->get<scene2::SceneNode>("waiting-host.tip2"));
    _tips.push_back(_assets->get<scene2::SceneNode>("waiting-host.tip3"));
    _tips.push_back(_assets->get<scene2::SceneNode>("waiting-host.tip4"));
    for (int i = 0; i < _tips.size(); ++i) {
        _tips[i]->setVisible(i == 0);
    }
    
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
            _backbutton->activate();
        } else {
            _backbutton->deactivate();
            _backbutton->setDown(false);
        }
    }
}

/**
 * Performs a film strip action
 *
 * @param key   The action key
 * @param action The film strip action
 * @param slide  The associated movement slide
 */
void WaitingHostScene::doStrip(std::string key, cugl::ActionFunction action, float duration = DURATION) {
    if (_timeline->isActive(key)) {
        // NO OP
    } else {
        _timeline->add(key, action, duration);
    }
}

/** Updates the scene */
void WaitingHostScene::update(float dt){
    doStrip(ACT_KEY, _waitAction, DURATION);
    _timeline->update(dt);
    _tipSwitchTimer += dt;
    if (_tipSwitchTimer >= _tipInterval) {
        _tipSwitchTimer = 0.0f;
        _tips[_currentTipIndex]->setVisible(false);
        _currentTipIndex = (_currentTipIndex + 1) % _tips.size();
        _tips[_currentTipIndex]->setVisible(true);
    }
}
