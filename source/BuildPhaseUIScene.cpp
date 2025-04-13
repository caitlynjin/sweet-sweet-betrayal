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
bool BuildPhaseUIScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<GridManager> gridManager) {
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

    std::shared_ptr<scene2::PolygonNode> rightNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    rightNode->setScale(0.8f);
    _rightButton = scene2::Button::alloc(rightNode);
    _rightButton->setAnchor(Vec2::ANCHOR_CENTER);
    _rightButton->setPosition(_size.width * 0.6f, _size.height * 0.1f);
    _rightButton->activate();
    _rightButton->addListener([this](const std::string &name, bool down) {
        _rightpressed = down;
    });

    std::shared_ptr<scene2::PolygonNode> leftNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_BUTTON));
    leftNode->setScale(0.8f);
    _leftButton = scene2::Button::alloc(leftNode);
    _leftButton->setAnchor(Vec2::ANCHOR_CENTER);
    _leftButton->setPosition(_size.width * 0.4f, _size.height * 0.1f);
    _leftButton->activate();
    _leftButton->addListener([this](const std::string &name, bool down) {
        _leftpressed = down;
    });

    std::shared_ptr<scene2::PolygonNode> readyNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    readyNode->setScale(0.8f);
    _readyButton = scene2::Button::alloc(readyNode);
    _readyButton->setAnchor(Vec2::ANCHOR_CENTER);
    _readyButton->setPosition(_size.width * 0.91f, _size.height * 0.1f);
    
    _readyButton->activate();
    _readyButton->addListener([this](const std::string &name, bool down) {
        if (down && !_isReady) {
            setIsReady(true);
            _gridManager->posToObjMap.clear();  // Disables movement of placed objects
        }
    });

    std::shared_ptr<scene2::PolygonNode> trashNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TRASH));
    trashNode->setScale(0.8f);
    _trashButton = scene2::Button::alloc(trashNode);
    _trashButton->setAnchor(Vec2::ANCHOR_CENTER);
    _trashButton->setPosition(_size.width * 0.1f, _size.height * 0.85f);

    _timer = scene2::Label::allocWithText(std::to_string(BUILD_TIME), _assets->get<Font>(MESSAGE_FONT));
    _timer->setAnchor(Vec2::ANCHOR_CENTER);
    _timer->setPosition(_size.width * 0.1f, _size.height * 0.15f);

    addChild(_rightButton);
    addChild(_readyButton);
    addChild(_leftButton);
    addChild(_trashButton);
    addChild(_timer);

    return true;
}

/**
 * Initializes the grid layout on the screen for build mode.
 */
void BuildPhaseUIScene::initInventory(std::vector<Item> inventoryItems, std::vector<std::string> assetNames)
{
    // Set the background
    _inventoryBackground = scene2::PolygonNode::alloc();
    _inventoryBackground->setPosition(Vec2(_size.width*0.88, _size.height*0.2));
    _inventoryBackground->setContentSize(Size(_size.width*0.18, _size.height*0.8));
    _inventoryBackground->setColor(Color4(131,111,108));
    _inventoryBackground->setVisible(true);
    addChild(_inventoryBackground);

    float yOffset = 0;
    for (size_t itemNo = 0; itemNo < inventoryItems.size(); itemNo++)
    {
        std::shared_ptr<scene2::PolygonNode> itemNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(assetNames[itemNo]));
        std::shared_ptr<scene2::Button> itemButton = scene2::Button::alloc(itemNode);
        itemButton->setAnchor(Vec2::ANCHOR_TOP_RIGHT);
        itemButton->setPosition(_size.width - 10, _size.height - 100 - yOffset);
        itemButton->setName(itemToString(inventoryItems[itemNo]));
        itemButton->setVisible(true);
        itemButton->activate();
        _inventoryButtons.push_back(itemButton);
        addChild(itemButton);
        yOffset += 80;
    }

    // Set the darkened overlay
    _inventoryOverlay = scene2::PolygonNode::alloc();
    _inventoryOverlay->setPosition(Vec2(_size.width * 0.88, _size.height * 0.2));
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
