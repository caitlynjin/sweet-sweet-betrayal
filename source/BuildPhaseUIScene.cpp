//
//  BuildPhaseUIScene.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/17/25.
//

#include "BuildPhaseUIScene.h"
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
#define SCENE_WIDTH 1306
#define SCENE_HEIGHT 576

/** The image for the ready button */
#define READY_BUTTON "ready_button"
/** The image for the left button */
#define LEFT_BUTTON "left_button"
/** The image for the right button */
#define RIGHT_BUTTON "right_button"
/** The texture for the inventory */
#define INVENTORY "inventory"

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
BuildPhaseUIScene::BuildPhaseUIScene() : Scene2() {}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void BuildPhaseUIScene::dispose() {
    if (_active)
    {
        _readyButton = nullptr;
        _rightButton = nullptr;
        _leftButton = nullptr;
        _trashButton = nullptr;
        _pauseButton = nullptr;
        _timer = nullptr;
        _redIcon = nullptr;
        _blueIcon = nullptr;
        _greenIcon = nullptr;
        _yellowIcon = nullptr;
        _topFrame = nullptr;
        _bottomFrame = nullptr;
        _leftFrame = nullptr;
        _timerFrame = nullptr;
        for (std::shared_ptr<cugl::scene2::PolygonNode> checkmark : _checkmarkList){
            checkmark = nullptr;
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
bool BuildPhaseUIScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> soundController, string local) {
    _networkController = networkController;
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, 0)))
    {
        return false;
    }
    
    Size dimen = getSize();

    _assets = assets;
    _gridManager = gridManager;
    _sound = soundController;
    _startTime = Application::get()->getEllapsedMicros();
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("buildmode");
    scene->setContentSize(dimen);
    scene->doLayout();

    _rightButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("buildmode.bottom.right_button"));
    _rightButton->setVisible(true);
    _rightButton->activate();
    _rightButton->addListener([this](const std::string &name, bool down) {
        _rightpressed = down;
    });

    _leftButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("buildmode.bottom.left_button"));
    _leftButton->setVisible(true);
    _leftButton->activate();
    _leftButton->addListener([this](const std::string &name, bool down) {
        _leftpressed = down;
    });

    _readyButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("buildmode.bottom.ready_button"));
    _leftButton->setVisible(true);
    _readyButton->activate();
    _readyButton->addListener([this](const std::string &name, bool down) {
        if (down && !_isReady) {
            setIsReady(true);
            _gridManager->posToObjMap.clear();  // Disables movement of placed objects
            _sound->playSound("button_click");
        }
    });

    _trashButton =std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("buildmode.bottom.trash"));
    _trashButton->setVisible(true);

    std::shared_ptr<scene2::PolygonNode> pauseNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(PAUSE));
    pauseNode->setScale(1.0f);
    _pauseButton = scene2::Button::alloc(pauseNode);
    _pauseButton->setAnchor(Vec2::ANCHOR_CENTER);
    _pauseButton->setPosition(_size.width * 0.1f, _size.height * 0.85f);
    _pauseButton->activate();
    _pauseButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _isPaused = true;
            _sound->playSound("button_click");
        }
    });
    
    _timerFrame = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.top.timer.timer"));
    _timerFrame->setVisible(true);
    _timer = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("buildmode.top.timer.text.text"));
    _timer->setVisible(true);

    _redIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.top.icons.player1.red-icon"));
    _blueIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.top.icons.player2.blue-icon"));
    _greenIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.top.icons.player3.green-icon"));
    _yellowIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.top.icons.player4.yellow-icon"));

    if (local == "playerRed"){
        _red = "buildmode.top.icons.player1.red-icon";
        _redPos = 1;
    }
    else if (local == "playerBlue"){
        _blue = "buildmode.top.icons.player1.blue-icon";
        _bluePos = 1;
    }
    else if (local == "playerGreen"){
        _green = "buildmode.top.icons.player1.green-icon";
        _greenPos = 1;
    }
    else if (local == "playerYellow"){
        _yellow = "buildmode.top.icons.player1.yellow-icon";
        _yellowPos = 1;
    }
    _icons--;

    _local = local;

    _redCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
            _assets->get<scene2::SceneNode>("buildmode.top.icons.player1.checkmark"));
    _blueCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
            _assets->get<scene2::SceneNode>("buildmode.top.icons.player2.checkmark"));
    _greenCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
            _assets->get<scene2::SceneNode>("buildmode.top.icons.player3.checkmark"));
    _yellowCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
            _assets->get<scene2::SceneNode>("buildmode.top.icons.player4.checkmark"));

    _redCheck->setVisible(false);
    _blueCheck->setVisible(false);
    _greenCheck->setVisible(false);
    _yellowCheck->setVisible(false);
    
    _topFrame = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.top.top-frame"));
    _topFrame->setVisible(true);

    _leftFrame = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.left-frame"));
    _leftFrame->setVisible(true);

    _bottomFrame = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.bottom.bottom-frame"));
    _bottomFrame->setVisible(true);

    addChild(scene);
    addChild(_pauseButton);
    return true;
}

/**
 * Initializes the item inventory.
 */
void BuildPhaseUIScene::initInventory(std::vector<Item> inventoryItems, std::vector<std::string> assetNames)
{
    // Set the background
    _inventoryBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.right.inventory"));
    _inventoryBackground->setVisible(true);

    setInventoryButtons(inventoryItems, assetNames);

    // Set the darkened overlay
    _inventoryOverlay = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("buildmode.right.overlay"));
    _inventoryOverlay->setVisible(false);
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void BuildPhaseUIScene::reset() {
    setVisible(true);
    activateInventory(true);
    
    // Reset UI variables
    _isReady = false;
    _rightpressed = false;
    _leftpressed = false;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void BuildPhaseUIScene::preUpdate(float dt) {

    Uint64 currentTime = Application::get()->getEllapsedMicros();
    Uint64 elapsedTime = currentTime - _startTime;
    auto numSeconds = BUILD_TIME - elapsedTime / 1000000;
    _timer->setText(std::to_string(numSeconds));
    // If we just changed seconds
    if (numSeconds != _previousElapsedTime && numSeconds <= 10) {
        _sound->playSound("timer");
    }
    _previousElapsedTime = numSeconds;
    if (elapsedTime >= BUILD_TIME * 1000000){
        _isReady = true;
    }
    _redIcon->setVisible(false);   _redCheck->setVisible(false);
    _blueIcon->setVisible(false);  _blueCheck->setVisible(false);
    _greenIcon->setVisible(false); _greenCheck->setVisible(false);
    _yellowIcon->setVisible(false);_yellowCheck->setVisible(false);

    for (auto& player : _networkController->getPlayerList()) {
        bool ready = player->getReady();
        std::string name = player->getName();
        if (name=="playerRed") {
            _redIcon->setVisible(true);
            _redCheck->setVisible(ready);
            CULog("red");
        }
        if (name=="playerBlue") {
            _blueIcon->setVisible(true);
            _blueCheck->setVisible(ready);
            CULog("blue");
        }
        if (name=="playerGreen") {
            _greenIcon->setVisible(true);
            _greenCheck->setVisible(ready);
            CULog("green");
        }
        if (name=="playerYellow") {
            _yellowIcon->setVisible(true);
            _yellowCheck->setVisible(ready);
            CULog("yellow");
        }
    }

    if (_networkController->getPlayerList().size() == _networkController->getNetwork()->getNumPlayers() && !_playersCounted){
        _playersCounted = true;
        for (auto& player : _networkController->getPlayerList()){
            if (player->getName() != _local && player->getName() == "playerRed"){
                if (_icons == 3){
                    _red = "buildmode.top.icons.player2.red-icon";
                    _redPos = 2;
                }
                else if (_icons == 2){
                    _red = "buildmode.top.icons.player3.red-icon";
                    _redPos = 3;
                }
                else if (_icons == 1){
                    _red = "buildmode.top.icons.player4.red-icon";
                    _redPos = 4;
                }
                _icons--;
            }
            else if (player->getName() != _local && player->getName() == "playerBlue"){
                if (_icons == 3){
                    _blue = "buildmode.top.icons.player2.blue-icon";
                    _bluePos = 2;
                }
                else if (_icons == 2){
                    _blue = "buildmode.top.icons.player3.blue-icon";
                    _bluePos = 3;
                }
                else if (_icons == 1){
                    _blue = "buildmode.top.icons.player4.blue-icon";
                    _bluePos = 4;
                }
                _icons--;
            }
            else if (player->getName() != _local && player->getName() == "playerGreen"){
                if (_icons == 3){
                    _green = "buildmode.top.icons.player2.green-icon";
                    _greenPos = 2;
                }
                else if (_icons == 2){
                    _green = "buildmode.top.icons.player3.green-icon";
                    _greenPos = 3;
                }
                else if (_icons == 1){
                    _green = "buildmode.top.icons.player4.green-icon";
                    _greenPos = 4;
                }
                _icons--;
            }
            else if (player->getName() != _local && player->getName() == "playerYellow"){
                if (_icons == 3){
                    _yellow = "buildmode.top.icons.player2.yellow-icon";
                    _yellowPos = 2;
                }
                else if (_icons == 2){
                    _yellow = "buildmode.top.icons.player3.yellow-icon";
                    _yellowPos = 3;
                }
                else if (_icons == 1){
                    _yellow = "buildmode.top.icons.player4.yellow-icon";
                    _yellowPos = 4;
                }
                _icons--;
            }
        }
        _redIcon->dispose();
        _blueIcon->dispose();
        _greenIcon->dispose();
        _yellowIcon->dispose();
        _redIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>(_red));
        _blueIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>(_blue));
        _greenIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>(_green));
        _yellowIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>(_yellow));
        _redCheck->dispose();
        _blueCheck->dispose();
        _greenCheck->dispose();
        _yellowCheck->dispose();
        switch (_redPos){
            case 1:
                _redCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player1.checkmark"));
                break;
            case 2:
                _redCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player2.checkmark"));
                break;
            case 3:
                _redCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player3.checkmark"));
                break;
            case 4:
                _redCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player4.checkmark"));
                break;
        }
        switch (_bluePos){
            case 1:
                _blueCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player1.checkmark"));
                break;
            case 2:
                _blueCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player2.checkmark"));
                break;
            case 3:
                _blueCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player3.checkmark"));
                break;
            case 4:
                _blueCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player4.checkmark"));
                break;
        }
        switch (_greenPos){
            case 1:
                _greenCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player1.checkmark"));
                break;
            case 2:
                _greenCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player2.checkmark"));
                break;
            case 3:
                _greenCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player3.checkmark"));
                break;
            case 4:
                _greenCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player4.checkmark"));
                break;
        }
        switch (_yellowPos){
            case 1:
                _yellowCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player1.checkmark"));
                break;
            case 2:
                _yellowCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player2.checkmark"));
                break;
            case 3:
                _yellowCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player3.checkmark"));
                break;
            case 4:
                _yellowCheck   = std::dynamic_pointer_cast<scene2::PolygonNode>(
                        _assets->get<scene2::SceneNode>("buildmode.top.icons.player4.checkmark"));
                break;
        }
        _redCheck->setVisible(false);
        _blueCheck->setVisible(false);
        _greenCheck->setVisible(false);
        _yellowCheck->setVisible(false);
    }
}

#pragma mark -
#pragma mark Attribute Functions
/**
 * Sets whether the player has pressed the ready button to indicate they are done with build phase.
 */
void BuildPhaseUIScene::setIsReady(bool isDone) {
    if (isDone){
        _readyButton->setColor(Color4::GRAY);
        _isReady = true;
    }
    else{
        _readyButton->setColor(Color4::WHITE);
        _isReady = false;
    }
}

#pragma mark -
#pragma mark Helpers
/**
 * Set whether the elements of this scene are visible or not
 */
void BuildPhaseUIScene::setVisible(bool value) {
    for (size_t i = 0; i < _inventoryButtons.size(); i++)
    {
        _inventoryButtons[i]->setVisible(value);
    }
    
    _inventoryOverlay->setVisible(value);
    _inventoryBackground->setVisible(value);

    _leftButton->setVisible(value);
    _rightButton->setVisible(value);
    _readyButton->setVisible(value);
    _trashButton->setVisible(value);
    _pauseButton->setVisible(value);
    _timer->setVisible(value);
    _redIcon->setVisible(value);
    _blueIcon->setVisible(value);
    _greenIcon->setVisible(value);
    _yellowIcon->setVisible(value);
    _topFrame->setVisible(value);
    _bottomFrame->setVisible(value);
    _leftFrame->setVisible(value);
    _timerFrame->setVisible(value);
    for (auto& player : _networkController->getPlayerList()){
        if (player->getName() == "playerRed"){
            _redCheck->setVisible(false);
        }
        if (player->getName() == "playerBlue"){
            _blueCheck->setVisible(false);
        }
        if (player->getName() == "playerGreen"){
            _greenCheck->setVisible(false);
        }
        if (player->getName() == "playerYellow"){
            _yellowCheck->setVisible(false);
        }
    }

    if (value){
        _timer->setText(std::to_string(BUILD_TIME));
        _startTime = Application::get()->getEllapsedMicros();
        _previousElapsedTime = BUILD_TIME;
    }
    else{
        _networkController->playersUnready();
    }
}

/**
 * Whether to activate the inventory.
 */
void BuildPhaseUIScene::activateInventory(bool value) {
    for (size_t i = 0; i < _inventoryButtons.size(); i++)
    {
        if (value) {
            _inventoryButtons[i]->activate();
        } else {
            _inventoryButtons[i]->deactivate();
        }
    }
    _inventoryOverlay->setVisible(!value);
}

/**
 * Set the inventory buttons for each item.
 */
void BuildPhaseUIScene::setInventoryButtons(std::vector<Item> inventoryItems, std::vector<std::string> assetNames) {
    // Reset buttons
    for (auto btn : _inventoryButtons) { btn->dispose(); }
    _inventoryButtons.clear();
    auto scene = _assets->get<scene2::SceneNode>("buildmode");
    scene->setContentSize(getSize());
    scene->doLayout();
    auto rightPanel = std::dynamic_pointer_cast<scene2::SceneNode>(_assets->get<scene2::SceneNode>("buildmode.right"));

    for (size_t i = 0; i < inventoryItems.size() && i < 4; ++i) {
        std::string slotName = "item" + std::to_string(i+1);
        auto slotNode = std::dynamic_pointer_cast<scene2::SceneNode>(
                             rightPanel->getChildByName(slotName));
        if (!slotNode) continue;
        
        auto imgNode = std::dynamic_pointer_cast<scene2::PolygonNode>(
                           slotNode->getChildByName(slotName));
        if (!imgNode) continue;
        
        Size slotSize = imgNode->getContentSize();
        Vec2  slotAnchor = imgNode->getAnchor();
        auto icon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(assetNames[i]));
        icon->setContentSize(slotSize);
        icon->setAnchor(Vec2::ANCHOR_CENTER);
        icon->setPosition(slotSize * 0.5f);

        auto button = scene2::Button::alloc(icon);
        button->setName(itemToString(inventoryItems[i]));
        button->activate();
        slotNode->addChild(button);

        _inventoryButtons.push_back(button);
    }
}
