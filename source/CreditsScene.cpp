//
// Created by chvel on 5/15/2025.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "CreditsScene.h"
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

bool CreditsScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> sound){
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("credits");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("credits.credits-background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }
    _choice = Choice::NONE;

    _backButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("credits.back"));

    _backButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::BACK;
            _sound->playSound("button_click");
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

void CreditsScene::update(float timestep) {
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void CreditsScene::dispose() {
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
void CreditsScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _backButton->activate();
        } else {
            _backButton->deactivate();
            _backButton->setDown(false);
        }
    }
}