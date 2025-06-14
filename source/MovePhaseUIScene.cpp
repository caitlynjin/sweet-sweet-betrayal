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
        _greenIcon = nullptr;
        _yellowIcon = nullptr;
        _treasureIcon = nullptr;
//        _pauseButton = nullptr;
//        for (auto score : _scoreImages){
//            score = nullptr;
//        }
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
bool MovePhaseUIScene::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<ScoreController>& scoreController, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> soundController, string local) {
    _networkController = networkController;
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }

    _assets = assets;
    _sound = soundController;

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
//    for (int i = 0; i < TOTAL_GEMS; i++){
//        std::shared_ptr<scene2::PolygonNode> scoreNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(EMPTY_IMAGE));
//        scoreNode->SceneNode::setAnchor(Vec2::ANCHOR_CENTER);
//        scoreNode->setPosition(_size.width * .15 + (i*distance),_size.height * .9);
//        scoreNode->setScale(0.1f);
//        scoreNode->setVisible(true);
//        _scoreImages.push_back(scoreNode);
//        // TODO: May cause issues
//        addChild(scoreNode);
//    }

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
    jumpNode->setScale(0.75f);
    _jumpbutton = scene2::Button::alloc(jumpNode);
    _jumpbutton->setAnchor(Vec2::ANCHOR_CENTER);
    _jumpbutton->setPosition(_size.width * 0.88f, _size.height * 0.22f);
    _jumpbutton->setVisible(false);
    _jumpbutton->setColor(Color4 (_jumpbutton->getColor().r, _jumpbutton->getColor().g, _jumpbutton->getColor().b, 184));
    _jumpbutton->addListener([this](const std::string &name, bool down) {
        if (down && _isActive) {
            _didjump = true;
        }
        else{
            _didjump = false;
        }
    });
    addChild(_jumpbutton);

    std::shared_ptr<scene2::PolygonNode> glideNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(GLIDE_BUTTON));
    glideNode->setScale(0.75f);
    _glidebutton = scene2::Button::alloc(glideNode);
    _glidebutton->setAnchor(Vec2::ANCHOR_CENTER);
    _glidebutton->setPosition(_size.width * 0.88f, _size.height * 0.22f);
    _glidebutton->setVisible(false);
    _glidebutton->setColor(Color4 (_glidebutton->getColor().r, _glidebutton->getColor().g, _glidebutton->getColor().b, 184));
    _glidebutton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _didglide = true;
        }
        else{
            _didglide = false;
        }
    });
    addChild(_glidebutton);
    
    std::shared_ptr<scene2::PolygonNode> giveupNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(GIVEUP_BUTTON));
    giveupNode->setScale(1.0f);
    _giveupbutton = scene2::Button::alloc(giveupNode);
    _giveupbutton->setAnchor(Vec2::ANCHOR_CENTER);
    _giveupbutton->setPosition(_size.width * 0.90f, _size.height * 0.85f);
    _giveupbutton->setVisible(false);
    _giveupbutton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _giveUp= true;
        }
        else{
            _giveUp = false;
        }
    });
    addChild(_giveupbutton);

//    std::shared_ptr<scene2::PolygonNode> pauseNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(_networkController->getNetwork()->getNumPlayers() == 1 ? PAUSE : HOME));
//    pauseNode->setScale(1.0f);
//    _pauseButton = scene2::Button::alloc(pauseNode);
//    _pauseButton->setAnchor(Vec2::ANCHOR_CENTER);
//    _pauseButton->setPosition(_size.width * 0.1f, _size.height * 0.85f);
//    if (_networkController->getNetwork()->getNumPlayers() == 1) {
//        _pauseButton->activate();
//    } else {
//        _pauseButton->setVisible(false);
//    }
//    _pauseButton->addListener([this](const std::string &name, bool down) {
//        if (!down) {
//            _isPaused = true;
//            _sound->playSound("button_click");
//        }
//    });
//    addChild(_pauseButton);

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

    _blueIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BLUE_ICON));
    _blueIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _blueIcon->setScale(0.05f);
    _blueIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2), _size.height * 0.9f);
    _blueIcon->setVisible(false);

    _greenIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(GREEN_ICON));
    _greenIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _greenIcon->setScale(0.05f);
    _greenIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2), _size.height * 0.9f);
    _greenIcon->setVisible(false);

    _yellowIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(YELLOW_ICON));
    _yellowIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _yellowIcon->setScale(0.05f);
    _yellowIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2), _size.height * 0.9f);
    _yellowIcon->setVisible(false);

    if (local == "playerRed"){
        addChild(_blueIcon);
        addChild(_greenIcon);
        addChild(_yellowIcon);
        addChild(_redIcon);
    }
    else if (local == "playerBlue"){
        addChild(_redIcon);
        addChild(_greenIcon);
        addChild(_yellowIcon);
        addChild(_blueIcon);
    }
    else if (local == "playerGreen"){
        addChild(_redIcon);
        addChild(_blueIcon);
        addChild(_yellowIcon);
        addChild(_greenIcon);
    }
    else if (local == "playerYellow"){
        addChild(_redIcon);
        addChild(_blueIcon);
        addChild(_greenIcon);
        addChild(_yellowIcon);
    }

    _treasureIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TREASURE_ICON));
    _treasureIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _treasureIcon->setScale(0.025f);
    _treasureIcon->setPosition(_redIcon->getPosition().x, _redIcon->getPosition().y + 10);
    _treasureIcon->setVisible(false);
    addChild(_treasureIcon);
    
    _scoreboardOverlay = scene2::PolygonNode::allocWithPoly(Rect(0, 0, _size.width, _size.height));
    _scoreboardOverlay->setColor(Color4(0, 0, 0, 192)); // 192/255 ~75% opacity black
    _scoreboardOverlay->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _scoreboardOverlay->setPosition(0, 0);
    _scoreboardOverlay->setVisible(false);
    
    addChild(_scoreboardOverlay);
    
    // Add scoreboard node
//    _scoreboardNode = scene2::Label::allocWithText("Round Over!", _assets->get<Font>(INFO_FONT));
//    _scoreboardNode->setAnchor(Vec2::ANCHOR_CENTER);
//    _scoreboardNode->setPosition(_size.width * .5,_size.height * .7);
//    _scoreboardNode->setForeground(INFO_COLOR);
//    _scoreboardNode->setVisible(false);
//    addChild(_scoreboardNode);
    
    // KEEP THIS FOR REFERENCE, WILL NEED ONCE SCORING UI IS IN
    // Add player score nodes
//    std::shared_ptr<scene2::Label> playerScore = scene2::Label::allocWithText("Player 1 has 0 points", _assets->get<Font>(INFO_FONT));
//    playerScore->setAnchor(Vec2::ANCHOR_CENTER);
//    playerScore->setPosition(_size.width * .5,_size.height * .5);
//    playerScore->setForeground(INFO_COLOR);
//    playerScore->setVisible(false);
//    addChild(playerScore);
//    _playerScores.push_back(playerScore);

    _playerList = _networkController->getPlayerList();
    _scoreController = scoreController;
    

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
    scoreBoardInitialized = false;
    _didjump = false;
    _didglide = false;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void MovePhaseUIScene::preUpdate(float dt) {
    if (!scoreBoardInitialized) {
        auto playerList = _networkController->getPlayerList();
        CULog("Calling initScoreboardNodes with %d players", (int)playerList.size());
        _scoreController->initScoreboardNodes(this, Vec2::ANCHOR_CENTER, _networkController->getPlayerList(), _size.width, _size.height);
        scoreBoardInitialized = true;
    }
}

/**
 * Sets whether mode is in building or play mode.
 *
 * @param value whether the level is in building mode.
 */
void MovePhaseUIScene::disableUI(bool value) {
    _progressBar->setVisible(!value);

    for (auto& player : _networkController->getPlayerList()){
        string playerTag = player->getName();
        if (playerTag == "playerRed"){
            _redIcon->setVisible(!value);
        }
        else if (playerTag == "playerBlue"){
            _blueIcon->setVisible(!value);
        }
        else if (playerTag == "playerGreen"){
            _greenIcon->setVisible(!value);
        }
        else if (playerTag == "playerYellow"){
            _yellowIcon->setVisible(!value);
        }
    }

    if (value){
        if (!_isActive) {
            return;
        }

        _jumpbutton->deactivate();
        _glidebutton->deactivate();
        _giveupbutton->deactivate();
        _giveupbutton->setVisible(!value);
        _giveUpCountDown = 2000;
    }
    else{
        if (!_isActive) {
            return;
        }

        _jumpbutton->activate();
        _glidebutton->activate();
    }
    _jumpbutton->setVisible(!value);
    _glidebutton->setVisible(!value);
}

void MovePhaseUIScene::setActive(bool value) {
    _isActive = value;

    if (value) {
        _jumpbutton->activate();
        _glidebutton->activate();
        _giveupbutton->activate();
        if (_networkController->getNetwork()->getNumPlayers() == 1) {
//            _pauseButton->activate();
        }
    } else {
        _jumpbutton->deactivate();
        _glidebutton->deactivate();
        _giveupbutton->deactivate();
//        _pauseButton->deactivate();
    }
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
    if (!_isActive) {
        return;
    }

    _jumpbutton->deactivate();
    _jumpbutton->setVisible(false);
    _glidebutton->activate();
    _glidebutton->setVisible(true);
}

/**
 * Set the jump button active.
 */
void MovePhaseUIScene::setJumpButtonActive() {
    if (!_isActive) {
        return;
    }

    _jumpbutton->activate();
    _jumpbutton->setVisible(true);
    _glidebutton->deactivate();
    _glidebutton->setVisible(false);
}

/**
 * Set the score image at this gem index to full.
 */
//void MovePhaseUIScene::setScoreImageFull(int index) {
//    _scoreImages.at(index)->setTexture(_assets->get<Texture>(FULL_IMAGE));
//}

/**
 * Updates round counter
 *
 * @param cur       The current round number
 * @param total     The total number of rounds
 */
void MovePhaseUIScene::updateRound(int cur, int total) {
//    _roundsnode->setText("Round: " + std::to_string(cur) + "/" + std::to_string(total));
}

void MovePhaseUIScene::setPlayerIcon(float pos, float width, string tag) {
    float cur = (pos * _progressBar->getWidth()) / width;
    if (tag == "playerRed"){
        _redIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2) + cur, _size.height * 0.9f);
    }
    else if (tag == "playerBlue"){
        _blueIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2) + cur, _size.height * 0.9f);
    }
    else if (tag == "playerGreen"){
        _greenIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2) + cur, _size.height * 0.9f);
    }
    else if (tag == "playerYellow"){
        _yellowIcon->setPosition(_size.width * 0.5f - (_progressBar->getWidth()/2) + cur, _size.height * 0.9f);
    }
}

void MovePhaseUIScene::setTreasureIcon(bool has, string tag) {
    if (tag == "playerRed" && has) {
        _treasureIcon->setPosition(_redIcon->getPosition().x, _redIcon->getPosition().y + 10);
    }
    else if (tag == "playerBlue" && has){
        _treasureIcon->setPosition(_blueIcon->getPosition().x, _blueIcon->getPosition().y + 10);
    }
    else if (tag == "playerGreen" && has){
        _treasureIcon->setPosition(_greenIcon->getPosition().x, _greenIcon->getPosition().y + 10);
    }
    else if (tag == "playerYellow" && has){
        _treasureIcon->setPosition(_yellowIcon->getPosition().x, _yellowIcon->getPosition().y + 10);
    }
    _treasureIcon->setVisible(has);
}

void MovePhaseUIScene::removePlayerIcon(string tag) {
    if (tag == "playerRed"){
        _redIcon->setVisible(false);
    }
    else if (tag == "playerBlue"){
        _blueIcon->setVisible(false);
    }
    else if (tag == "playerGreen"){
        _greenIcon->setVisible(false);
    }
    else if (tag == "playerYellow"){
        _yellowIcon->setVisible(false);
    }
}



