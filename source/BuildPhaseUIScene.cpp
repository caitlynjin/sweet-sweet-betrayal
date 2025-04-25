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
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** The image for the ready button */
#define READY_BUTTON "ready_button"
/** The image for the left button */
#define LEFT_BUTTON "left_button"
/** The image for the right button */
#define RIGHT_BUTTON "right_button"
/** The texture for the inventory */
#define INVENTORY "inventory"

/** Starting build time for timer */
#define BUILD_TIME 30

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
bool BuildPhaseUIScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<GridManager> gridManager, std::shared_ptr<NetworkController> networkController) {
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
    _gridManager = gridManager;

    _startTime = Application::get()->getEllapsedMicros();

    std::shared_ptr<scene2::PolygonNode> rightNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RIGHT_BUTTON));
    _rightButton = scene2::Button::alloc(rightNode);
    _rightButton->setScale(0.2f);
    _rightButton->setAnchor(Vec2::ANCHOR_CENTER);
    _rightButton->setPosition(_size.width * 0.65f, _size.height * 0.15f);
    _rightButton->activate();
    _rightButton->addListener([this](const std::string &name, bool down) {
        _rightpressed = down;
    });

    std::shared_ptr<scene2::PolygonNode> leftNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_BUTTON));
    _leftButton = scene2::Button::alloc(leftNode);
    _leftButton->setScale(0.2f);
    _leftButton->setAnchor(Vec2::ANCHOR_CENTER);
    _leftButton->setPosition(_size.width * 0.35f, _size.height * 0.15f);
    _leftButton->activate();
    _leftButton->addListener([this](const std::string &name, bool down) {
        _leftpressed = down;
    });

    std::shared_ptr<scene2::PolygonNode> readyNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    readyNode->setScale(0.8f);
    _readyButton = scene2::Button::alloc(readyNode);
    _readyButton->setAnchor(Vec2::ANCHOR_CENTER);
    _readyButton->setPosition(_size.width * 0.5f, _size.height * 0.15f);
    
    _readyButton->activate();
    _readyButton->addListener([this](const std::string &name, bool down) {
        if (down && !_isReady) {
            setIsReady(true);
            _gridManager->posToObjMap.clear();  // Disables movement of placed objects
        }
    });

    std::shared_ptr<scene2::PolygonNode> trashNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TRASH_CLOSED));
    trashNode->setScale(0.8f);
    _trashButton = scene2::Button::alloc(trashNode);
    _trashButton->setAnchor(Vec2::ANCHOR_CENTER);
    _trashButton->setPosition(_size.width * 0.1f, _size.height * 0.2f);

    _timer = scene2::Label::allocWithText(std::to_string(BUILD_TIME), _assets->get<Font>(TIMER_FONT));
    _timer->setAnchor(Vec2::ANCHOR_CENTER);
    _timer->setPosition(_size.width * 0.505f, _size.height * 0.9f);
    _timer->setForeground(Color4 (255,62,62));

    _redIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RED_ICON));
    _redIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _redIcon->setScale(0.04f);
    _redIcon->setPosition(_size.width * 0.64f, _size.height * 0.9f);

    _blueIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BLUE_ICON));
    _blueIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _blueIcon->setScale(0.04f);
    _blueIcon->setPosition(_size.width * 0.7f, _size.height * 0.9f);

    _greenIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(GREEN_ICON));
    _greenIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _greenIcon->setScale(0.04f);
    _greenIcon->setPosition(_size.width * 0.76f, _size.height * 0.9f);

    _yellowIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(YELLOW_ICON));
    _yellowIcon->setAnchor(Vec2::ANCHOR_CENTER);
    _yellowIcon->setScale(0.04f);
    _yellowIcon->setPosition(_size.width * 0.82f, _size.height * 0.9f);

    _topFrame = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TOP_FRAME));
    _topFrame->setScale(0.45f, 0.4f);
    _topFrame->setAnchor(Vec2::ANCHOR_CENTER);
    _topFrame->setPosition(_size.width * 0.51f, _size.height * 0.96f);

    _leftFrame = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_FRAME));
    _leftFrame->setScale(0.36f);
    _leftFrame->setAnchor(Vec2::ANCHOR_MIDDLE_LEFT);
    _leftFrame->setPosition(0, _size.height * 0.5f);

    _bottomFrame = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BOTTOM_FRAME));
    _bottomFrame->setScale(0.45f, 0.4f);
    _bottomFrame->setAnchor(Vec2::ANCHOR_CENTER);
    _bottomFrame->setPosition(_size.width * 0.51f, _size.height * 0.04f);

    _timerFrame = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TIMER));
    _timerFrame->setScale(0.5f ,0.45f);
    _timerFrame->setAnchor(Vec2::ANCHOR_CENTER);
    _timerFrame->setPosition(_size.width * 0.5f, _size.height * 0.86f);

    addChild(_rightButton);
    addChild(_readyButton);
    addChild(_leftButton);
    addChild(_trashButton);
    addChild(_timerFrame);
    addChild(_timer);
    addChild(_topFrame);
    addChild(_leftFrame);
    addChild(_bottomFrame);

    return true;
}

/**
 * Initializes the item inventory.
 */
void BuildPhaseUIScene::initInventory(std::vector<Item> inventoryItems, std::vector<std::string> assetNames)
{
    // Set the background
    _inventoryBackground = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(INVENTORY));
    _inventoryBackground->setScale(0.9f);
    _inventoryBackground->setAnchor(Vec2::ANCHOR_CENTER);
    _inventoryBackground->setPosition(Vec2(_size.width * 0.92, _size.height*0.5));
    //_inventoryBackground->setColor(Color4(131,111,108));
    _inventoryBackground->setVisible(true);
    addChild(_inventoryBackground);

    setInventoryButtons(inventoryItems, assetNames);

    // Set the darkened overlay
    _inventoryOverlay = scene2::PolygonNode::alloc();
    _inventoryOverlay->setPosition(Vec2(_size.width * 0.88, _size.height * 0.1));
    _inventoryOverlay->setContentSize(Size(_size.width * 0.18, _size.height * 0.8));
    _inventoryOverlay->setColor(Color4(0, 0, 0, 128));
    _inventoryOverlay->setVisible(false);
    addChild(_inventoryOverlay);
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
    _timer->setText(std::to_string(BUILD_TIME - elapsedTime / 1000000));
    if (elapsedTime >= BUILD_TIME * 1000000){
        _isReady = true;
    }
    else if (BUILD_TIME - elapsedTime / 1000000 < 10){
        _timer->setPosition(_size.width * 0.52f, _size.height * 0.9f);
    }
    else if (BUILD_TIME - elapsedTime / 1000000 < 20){
        _timer->setPosition(_size.width * 0.51f, _size.height * 0.9f);
    }
    if (_networkController->getPlayerList().size() > 0 && !_playersCounted){
        // TODO: Finish player ready logic
        for (auto& player : _networkController->getPlayerList()){
            if (player->getName() == "playerRed"){
                _iconList.push_back(_redIcon);
                addChild(_redIcon);
            }
            if (player->getName() == "playerBlue"){
                _iconList.push_back(_blueIcon);
                addChild(_blueIcon);
            }
            if (player->getName() == "playerGreen"){
                _iconList.push_back(_greenIcon);
                addChild(_greenIcon);
            }
            if (player->getName() == "playerYellow"){
                _iconList.push_back(_yellowIcon);
                addChild(_yellowIcon);
            }
        }

        for (std::shared_ptr<cugl::scene2::PolygonNode> icon : _iconList){
            std::shared_ptr<cugl::scene2::PolygonNode> checkmark = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(CHECKMARK));
            checkmark->setAnchor(Vec2::ANCHOR_CENTER);
            checkmark->setScale(0.04f);
            checkmark->setPosition(icon->getPositionX() + 25, icon->getPositionY() - 15);
            _checkmarkMap[icon] = checkmark;
            _checkmarkList.push_back(checkmark);
            checkmark->setVisible(false);
            addChild(checkmark);
        }
        _playersCounted = true;
    }
    for (auto& player : _networkController->getPlayerList()){
        if (player->getName() == "playerRed" && player->getReady()){
            _checkmarkMap[_redIcon]->setVisible(true);
        }
        if (player->getName() == "playerBlue" && player->getReady()){
            _checkmarkMap[_blueIcon]->setVisible(true);
        }
        if (player->getName() == "playerGreen" && player->getReady()){
            _checkmarkMap[_greenIcon]->setVisible(true);
        }
        if (player->getName() == "playerYellow" && player->getReady()){
            _checkmarkMap[_yellowIcon]->setVisible(true);
        }
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
    _timer->setVisible(value);
    _redIcon->setVisible(value);
    _blueIcon->setVisible(value);
    _greenIcon->setVisible(value);
    _yellowIcon->setVisible(value);
    _topFrame->setVisible(value);
    _bottomFrame->setVisible(value);
    _leftFrame->setVisible(value);
    _timerFrame->setVisible(value);
    for (std::shared_ptr<cugl::scene2::PolygonNode> checkmark : _checkmarkList){
        checkmark->setVisible(false);
    }

    if (value){
        _timer->setText(std::to_string(BUILD_TIME));
        _startTime = Application::get()->getEllapsedMicros();
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
    for (auto it = _inventoryButtons.begin(); it != _inventoryButtons.end(); ) {
        std::shared_ptr<scene2::Button> btn = *it;
        btn->dispose();
        it = _inventoryButtons.erase(it);
    }

    float yOffset = 0;
    for (size_t itemNo = 0; itemNo < inventoryItems.size(); itemNo++)
    {
        std::shared_ptr<scene2::PolygonNode> itemNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(assetNames[itemNo]));
        std::shared_ptr<scene2::Button> itemButton = scene2::Button::alloc(itemNode);
        itemButton->setAnchor(Vec2::ANCHOR_CENTER);
        itemButton->setScale(0.2f);
        itemButton->setPosition(_size.width - 75, _size.height - 120 - yOffset);
        itemButton->setName(itemToString(inventoryItems[itemNo]));
        itemButton->setVisible(true);
        itemButton->activate();
        _inventoryButtons.push_back(itemButton);
        addChild(itemButton);
        yOffset += 115;
    }
}
