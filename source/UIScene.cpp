//
// Created by chvel on 3/5/2025.
//

#include "UIScene.h"
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

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576
/** The image for the ready button */
#define READY_BUTTON "ready_button"
/** The image for the left button */
#define LEFT_BUTTON "left_button"
/** The color of the info labels */
#define INFO_COLOR      Color4::WHITE
/** The font for Round and Gem info */
#define INFO_FONT    "marker"

UIScene::UIScene() : Scene2() {

}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool UIScene::init(const std::shared_ptr<AssetManager>& assets)
{
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        CULog("%d %d", SCENE_WIDTH, SCENE_HEIGHT);
        return false;
    }

    // Start in building mode
    _readypressed = false;
    _rightpressed = false;
    _leftpressed = false;

    _assets = assets;

    _roundsnode = scene2::Label::allocWithText("Round: 1/" + std::to_string(TOTAL_ROUNDS), _assets->get<Font>(INFO_FONT));
    _roundsnode->setAnchor(Vec2::ANCHOR_CENTER);
    _roundsnode->setPosition(_size.width * .75,_size.height * .9);
    _roundsnode->setContentWidth(_size.width * .3);
    _roundsnode->setForeground(INFO_COLOR);
    _roundsnode->setVisible(true);

    std::shared_ptr<scene2::PolygonNode> rightNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    rightNode->setScale(0.8f);
    _rightButton = scene2::Button::alloc(rightNode);
    _rightButton->setAnchor(Vec2::ANCHOR_CENTER);
    _rightButton->setPosition(_size.width * 0.6f, _size.height * 0.1f);
    _rightButton->activate();
    _rightButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _rightpressed = true;
        }
        else{
            _rightpressed = false;
        }
    });

    std::shared_ptr<scene2::PolygonNode> leftNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_BUTTON));
    leftNode->setScale(0.8f);
    _leftButton = scene2::Button::alloc(leftNode);
    _leftButton->setAnchor(Vec2::ANCHOR_CENTER);
    _leftButton->setPosition(_size.width * 0.4f, _size.height * 0.1f);
    _leftButton->activate();
    _leftButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _leftpressed = true;
        }
        else{
            _leftpressed = false;
        }
    });

    std::shared_ptr<scene2::PolygonNode> readyNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    readyNode->setScale(0.8f);
    _readyButton = scene2::Button::alloc(readyNode);
    _readyButton->setAnchor(Vec2::ANCHOR_CENTER);
    _readyButton->setPosition(_size.width * 0.91f, _size.height * 0.1f);
    _readyButton->activate();
    _readyButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            if (!_isReady){
                _readypressed = true;
                setReadyDone(true);
            }
//            _readyButton->setVisible(false);
//            _rightButton->setVisible(false);
//            _leftButton->setVisible(false);
        }
        else{
            _readypressed = false;
        }
    });

    addChild(_roundsnode);
    addChild(_rightButton);
    addChild(_readyButton);
    addChild(_leftButton);

    return true;
}

void UIScene::dispose(){
    if (_active)
    {
        _roundsnode = nullptr;
        _readyButton = nullptr;
        _rightButton = nullptr;
        _leftButton = nullptr;
        Scene2::dispose();
    }
}

/**
     * The method called to update the game mode.
     *
     * This is the nondeterministic version of a physics simulation. It is
     * provided for comparison purposes only.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
void UIScene::update(float timestep){

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
void UIScene::preUpdate(float dt){

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
void UIScene::fixedUpdate(float step){

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
void UIScene::postUpdate(float remain){

}


/**
 * Resets the status of the game so that we can play again.
 */
void UIScene::reset(){
    _readyButton->setVisible(true);
    _rightButton->setVisible(true);
    _leftButton->setVisible(true);
}

/**
* @return true if the ready button was pressed
*/
bool UIScene::getReadyPressed() {
    return _readypressed;
}

/**
* @return true if the right button was pressed
*/
bool UIScene::getRightPressed() {
    return _rightpressed;
}

/**
* @return true if the left button was pressed
*/
bool UIScene::getLeftPressed() {
    return _leftpressed;
}

/**
* Makes the buttons in the building mode visible
*/
void UIScene::visibleButtons(bool isVisible) {
    _readyButton->setVisible(isVisible);
    _rightButton->setVisible(isVisible);
    _leftButton->setVisible(isVisible);
}

/**
 * Updates round counter
 *
 * @param cur       The current round number
 * @param total     The total number of rounds
 */
void UIScene::updateRound(int cur, int total) {
    _roundsnode->setText("Round: " + std::to_string(cur) + "/" + std::to_string(total));
}

void UIScene::setReadyDone(bool isDone){
    if (isDone){
        _readyButton->setColor(Color4::GRAY);
        _isReady = true;
    }
    else{
        _readyButton->setColor(Color4::WHITE);
        _isReady = false;
    }
}


