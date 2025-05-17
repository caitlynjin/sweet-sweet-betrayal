//
//  VictoryScene.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/13/25.
//

#include <stdio.h>

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "VictoryScene.h"
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

bool VictoryScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<SoundController> &sound, const std::shared_ptr<NetworkController> networkController) {
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
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("end");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _restartButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("end.restart"));
    _quitButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("end.quit"));
    
    // Program the buttons
    _restartButton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::RESTART;
            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::RESET_LEVEL));
        }
    });
    
    _quitButton->addListener([this](const std::string& name, bool down) {
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
void VictoryScene::dispose() {
    reset();
    removeAllChildren();
    _background = nullptr;
    _restartButton->clearListeners();
    _quitButton->clearListeners();
    _restartButton = nullptr;
    _quitButton = nullptr;
    _assets = nullptr;
    _sound = nullptr;
    _networkController = nullptr;
    _network = nullptr;
    _input.dispose();

    
    _active = false;
    Scene2::dispose();
    
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
void VictoryScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _restartButton->activate();
            _quitButton->activate();
            _sound->playMusic("victory", true);
        } else {
            _restartButton->deactivate();
            _quitButton->deactivate();
            // If any were pressed, reset them
            _restartButton->setDown(false);
            _quitButton->setDown(false);
        }
    }
}

void VictoryScene::reset(){
    _choice = Choice::NONE;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the beginning of the core application loop,
 * which is concluded with a call to {@link #postUpdate}.
 *
 * This method should be used to process any events that happen early in
 * the application loop, such as user input or events created by the
 * {@link schedule} method. In particular, no new user input will be
 * recorded between the time this method is called and {@link #postUpdate}
 * is invoked.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #postUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void VictoryScene::preUpdate(float dt){
    _networkController->preUpdate(dt);
}

/**
 * The method called to provide a deterministic application loop.
 *
 * This method provides an application loop that runs at a guaranteed fixed
 * timestep. This method is (logically) invoked every {@link getFixedStep}
 * microseconds. By that we mean if the method {@link draw} is called at
 * time T, then this method is guaranteed to have been called exactly
 * floor(T/s) times this session, where s is the fixed time step.
 *
 * This method is always invoked in-between a call to {@link #preUpdate}
 * and {@link #postUpdate}. However, to guarantee determinism, it is
 * possible that this method is called multiple times between those two
 * calls. Depending on the value of {@link #getFixedStep}, it can also
 * (periodically) be called zero times, particularly if {@link #getFPS}
 * is much faster.
 *
 * As such, this method should only be used for portions of the application
 * that must be deterministic, such as the physics simulation. It should
 * not be used to process user input (as no user input is recorded between
 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
 *
 * The time passed to this method is NOT the same as the one passed to
 * {@link #preUpdate}. It will always be exactly the same value.
 *
 * @param step  The number of fixed seconds for this step
 */
void VictoryScene::fixedUpdate(float step){
    _networkController->fixedUpdate(step);
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the end of the core application loop, which was
 * begun with a call to {@link #preUpdate}.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * This method should not be used to process user input, as no new input
 * will have been recorded since {@link #preUpdate} was called.
 *
 * Note that the time passed as a parameter is the time measured from the
 * last call to {@link #fixedUpdate}. That is because this method is used
 * to interpolate object position for animation.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void VictoryScene::postUpdate(float remain){
    _networkController->postUpdate(remain);
}
