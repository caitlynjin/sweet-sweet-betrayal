//
//  MovePhaseUIScene.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/18/25.
//

#include "MovePhaseUIScene.h"
#include "Constants.h"
#include "Platform.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "PlayerModel.h"
#include "WindObstacle.h"
#include "LevelModel.h"
#include "ObjectController.h"

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

/** The color of the info labels */
#define INFO_COLOR      Color4::WHITE
/** The font for Round and Gem info */
#define INFO_FONT    "marker"

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
MovePhaseUIScene::MovePhaseUIScene() : Scene2() {}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MovePhaseUIScene::dispose() {
    if (_active)
    {
        _winnode = nullptr;
        _losenode = nullptr;
//        _roundsnode = nullptr;
        _leftnode = nullptr;
        _rightnode = nullptr;
        _progressBar = nullptr;
        _redIcon = nullptr;
        _blueIcon = nullptr;
        _treasureIcon = nullptr;
        for (auto score : _scoreImages){
            score = nullptr;
        }
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
bool MovePhaseUIScene::init(const std::shared_ptr<AssetManager>& assets, int players) {
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }

    _assets = assets;

    _winnode = scene2::Label::allocWithText(WIN_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(_size.width / 2.0f, _size.height / 2.0f);
    _winnode->setForeground(WIN_COLOR);
    addChild(_winnode);

    _losenode = scene2::Label::allocWithText(LOSE_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _losenode->setAnchor(Vec2::ANCHOR_CENTER);
    _losenode->setPosition(_size.width / 2.0f, _size.height / 2.0f);
    _losenode->setForeground(LOSE_COLOR);
    addChild(_losenode);

//    _roundsnode = scene2::Label::allocWithText("Round: 1/" + std::to_string(_totalRounds), _assets->get<Font>(INFO_FONT));
//    _roundsnode->setAnchor(Vec2::ANCHOR_CENTER);
//    _roundsnode->setPosition(_size.width * .75,_size.height * .9);
//    _roundsnode->setContentWidth(_size.width * .3);
//    _roundsnode->setForeground(INFO_COLOR);
//    _roundsnode->setVisible(true);
//    addChild(_roundsnode);

    float distance = _size.width * .05;
    for (int i = 0; i < TOTAL_GEMS; i++){
        std::shared_ptr<scene2::PolygonNode> scoreNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(EMPTY_IMAGE));
        scoreNode->SceneNode::setAnchor(Vec2::ANCHOR_CENTER);
        scoreNode->setPosition(_size.width * .15 + (i*distance),_size.height * .9);
        scoreNode->setScale(0.1f);
        scoreNode->setVisible(true);
        _scoreImages.push_back(scoreNode);
        // TODO: May cause issues
        addChild(scoreNode);
    }

    _leftnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_IMAGE));
    _leftnode->SceneNode::setAnchor(Vec2::ANCHOR_MIDDLE_RIGHT);
    _leftnode->setScale(0.35f);
    _leftnode->setVisible(false);
    addChild(_leftnode);

    _rightnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RIGHT_IMAGE));
    _rightnode->SceneNode::setAnchor(Vec2::ANCHOR_MIDDLE_LEFT);
    _rightnode->setScale(0.35f);
    _rightnode->setVisible(false);
    addChild(_rightnode);

    std::shared_ptr<scene2::PolygonNode> jumpNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(JUMP_BUTTON));
    _jumpbutton = scene2::Button::alloc(jumpNode);
    _jumpbutton->setAnchor(Vec2::ANCHOR_CENTER);
    _jumpbutton->setPosition(_size.width * 0.85f, _size.height * 0.25f);
    _jumpbutton->setVisible(false);
    _jumpbutton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _didjump = true;
        }
        else{
            _didjump = false;
        }
    });
    addChild(_jumpbutton);

    std::shared_ptr<scene2::PolygonNode> glideNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(GLIDE_BUTTON));
    _glidebutton = scene2::Button::alloc(glideNode);
    _glidebutton->setAnchor(Vec2::ANCHOR_CENTER);
    _glidebutton->setPosition(_size.width * 0.85f, _size.height * 0.25f);
    _glidebutton->setVisible(false);
    _glidebutton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _didglide = true;
        }
        else{
            _didglide = false;
        }
    });
    addChild(_glidebutton);

    _progressBar = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(PROGRESS_BAR));
    _progressBar->setAnchor(Vec2::ANCHOR_CENTER);
    _progressBar->setScale(0.4f);
    _progressBar->setPosition(_size.width * 0.5f, _size.height * 0.9f);
    _progressBar->setVisible(false);
    addChild(_progressBar);

    _redIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RED_ICON));
    _redIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _redIcon->setScale(0.05f);
    _redIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2), _size.height * 0.9f);
    _redIcon->setVisible(false);
    addChild(_redIcon);

    _blueIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BLUE_ICON));
    _blueIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _blueIcon->setScale(0.05f);
    _blueIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2), _size.height * 0.9f);
    _blueIcon->setVisible(false);
    addChild(_blueIcon);

    _treasureIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TREASURE_ICON));
    _treasureIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _treasureIcon->setScale(0.025f);
    _treasureIcon->setPosition(_redIcon->getPosition());
    _treasureIcon->setVisible(false);
    addChild(_treasureIcon);

    _numPlayers = players;

    return true;
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void MovePhaseUIScene::reset() {
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void MovePhaseUIScene::preUpdate(float dt) {

}

/**
 * Sets whether mode is in building or play mode.
 *
 * @param value whether the level is in building mode.
 */
void MovePhaseUIScene::setBuildingMode(bool value) {
//    _roundsnode->setVisible(value);
    _progressBar->setVisible(!value);
    _redIcon->setVisible(!value);
    if (_numPlayers > 1){
        _blueIcon->setVisible(!value);
    }

    if (value){
        _jumpbutton->deactivate();
        _glidebutton->deactivate();
        _glidebutton->setVisible(false);
    }
    else{
        _jumpbutton->activate();
    }
    _jumpbutton->setVisible(!value);
}

#pragma mark -
#pragma mark Attribute Functions
/**
 * Sets the total number of rounds
 */
void MovePhaseUIScene::setTotalRounds(int value) {
    _totalRounds = value;
};


#pragma mark -
#pragma mark Helpers
/**
 * Set whether the elements of this scene are visible or not
 */
void MovePhaseUIScene::setVisible(bool value) {
//    _roundsnode->setVisible(value);
}

/**
 * Set whether the win node is visible or not
 */
void MovePhaseUIScene::setWinVisible(bool value) {
    _winnode->setVisible(value);
}

/**
 * Set whether the lose node is visible or not
 */
void MovePhaseUIScene::setLoseVisible(bool value) {
    _losenode->setVisible(value);
}

/**
 * Sets the left joystick to be visible.
 */
void MovePhaseUIScene::setLeftVisible() {
    _leftnode->setVisible(true);
    _rightnode->setVisible(false);
}

/**
 * Sets the right joystick to be visible.
 */
void MovePhaseUIScene::setRightVisible() {
    _leftnode->setVisible(false);
    _rightnode->setVisible(true);
}

/**
 * Sets both joysticks to hidden.
 */
void MovePhaseUIScene::setJoystickHidden() {
    _leftnode->setVisible(false);
    _rightnode->setVisible(false);
}

/**
 * Set joystick positions.
 */
void MovePhaseUIScene::setJoystickPosition(Vec2 pos) {
    _leftnode->setPosition(pos);
    _rightnode->setPosition(pos);
}

/**
 * Get whether the glide button is held down.
 */
bool MovePhaseUIScene::isGlideDown() {
    return _glidebutton->isDown();
}

/**
 * Get whether the jump button is held down.
 */
bool MovePhaseUIScene::isJumpDown() {
    return _jumpbutton->isDown();
}

/**
 * Set the glide button active.
 */
void MovePhaseUIScene::setGlideButtonActive() {
    _jumpbutton->deactivate();
    _jumpbutton->setVisible(false);
    _glidebutton->activate();
    _glidebutton->setVisible(true);
}

/**
 * Set the jump button active.
 */
void MovePhaseUIScene::setJumpButtonActive() {
    _jumpbutton->activate();
    _jumpbutton->setVisible(true);
    _glidebutton->deactivate();
    _glidebutton->setVisible(false);
}

/**
 * Set the score image at this gem index to full.
 */
void MovePhaseUIScene::setScoreImageFull(int index) {
    _scoreImages.at(index)->setTexture(_assets->get<Texture>(FULL_IMAGE));
}

/**
 * Updates round counter
 *
 * @param cur       The current round number
 * @param total     The total number of rounds
 */
void MovePhaseUIScene::updateRound(int cur, int total) {
//    _roundsnode->setText("Round: " + std::to_string(cur) + "/" + std::to_string(total));
}

void MovePhaseUIScene::setRedIcon(float pos, float width) {
    float cur = (pos * _progressBar->getWidth()) / width;
    _redIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2) + cur, _size.height * 0.9f);
}

void MovePhaseUIScene::setBlueIcon(float pos, float width) {
    float cur = (pos * _progressBar->getWidth()) / width;
    _blueIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2) + cur, _size.height * 0.9f);
}

void MovePhaseUIScene::setTreasureIcon(bool has, int color) {
    if (color == 0 && has) {
        _treasureIcon->setPosition(_redIcon->getPosition());
    }
    _treasureIcon->setVisible(has);
}



