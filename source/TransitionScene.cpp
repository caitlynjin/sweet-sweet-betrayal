//
//  TransitionScene.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/29/25.
//

#include <stdio.h>

#include "TransitionScene.h"
#include "Constants.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::physics2;
using namespace cugl::audio;
using namespace Constants;

#pragma mark -
#pragma mark Scene Constants

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576



#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
TransitionScene::TransitionScene() : Scene2() {}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void TransitionScene::dispose() {
    if (_active)
    {
        //TODO: Set class variables to nullptr

        Scene2::dispose();
    }
};

/**
 * Initializes the scene contents
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool TransitionScene::init(const std::shared_ptr<AssetManager>& assets) {
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }
    _assets = assets;

    Vec2 offset = Vec2((_size.width - SCENE_WIDTH) / getSystemScale(), (_size.height - SCENE_HEIGHT)) / getSystemScale();
    
    _blackScreen = scene2::PolygonNode::allocWithPoly(Rect(0, 0, _size.width, _size.height));
    _blackScreen->setColor(Color4(0,0,0,192)); // 192/255 ~75% opacity black
    _blackScreen->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _blackScreen->setPosition(0, 0);
    _blackScreen->setVisible(true);
    _blackScreen->setPriority(5);
//    _blackScreen->setPriority(5);
    addChild(_blackScreen);

    

    return true;
}

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void TransitionScene::reset() {
    //TODO: Add reset logic
    _fadingInDone = false;
    _fadingOutDone = false;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void TransitionScene::preUpdate(float dt) {
    //TODO: add transition logic
    Color4 currColor = _blackScreen->getColor();
    switch (_transitionType)
    {
        case TransitionType::FADE_IN:
            currColor = _blackScreen->getColor();
            if (currColor.a > 0.0f){
                currColor.a -= FADE_RATE;
                _blackScreen->setColor(currColor);
            }
            else{
                _fadingInDone = true;
            }
            break;
        case TransitionType::FADE_OUT:
            currColor = _blackScreen->getColor();
            if (currColor.a < 255.0f){
                // Check for ceiling alpha value
                if (currColor.a + FADE_RATE > 255){
                    currColor.a = 255;
                }
                else{
                    currColor.a += FADE_RATE;
                }
                _blackScreen->setColor(currColor);
            }
            else{
                _fadingOutDone = true;
            }
            break;
        case TransitionType::NONE:
            currColor = _blackScreen->getColor();
            break;
            
        default:
            break;
    }
}

void TransitionScene::startFadeIn(){
    _blackScreen->setColor(Color4::BLACK);
    _transitionType = TransitionType::FADE_IN;
    _fadingInDone = false;
    _fadingOutDone = false;
}


void TransitionScene::startFadeOut(){
    Color4 newColor = Color4(0,0,0,0);
    _blackScreen->setColor(newColor);
    _transitionType = TransitionType::FADE_OUT;
    _fadingOutDone = false;
    _fadingInDone = false;
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
void TransitionScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _blackScreen->setVisible(true);
        } else {
            _blackScreen->setVisible(false);
        }
    }
}
