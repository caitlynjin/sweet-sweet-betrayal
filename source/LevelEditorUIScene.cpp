//
//  LevelEditorUIScene.cpp
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/28/25.
//

#include "LevelEditorUIScene.h"
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
/** The image for the brush button */
#define BRUSH_BUTTON "brush_button"
/** The image for the eraser button */
#define ERASER_BUTTON "eraser_button"

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
LevelEditorUIScene::LevelEditorUIScene() : Scene2() {}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelEditorUIScene::dispose() {
    if (_active)
    {
        _readyButton = nullptr;
        _rightButton = nullptr;
        _leftButton = nullptr;
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
bool LevelEditorUIScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<GridManager> gridManager) {
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

    std::shared_ptr<scene2::PolygonNode> rightNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RIGHT_BUTTON));
    rightNode->setScale(1.0f);
    _rightButton = scene2::Button::alloc(rightNode);
    //_rightButton->setContentSize(_rightButton->getContentSize() / 2.0f);
    _rightButton->setAnchor(Vec2::ANCHOR_CENTER);
    _rightButton->setPosition(_size.width * 0.7f, _size.height * 0.1f);
    _rightButton->setScale(0.2f);
    _rightButton->activate();
    _rightButton->addListener([this](const std::string& name, bool down) {
        _rightpressed = down;
        });

    std::shared_ptr<scene2::PolygonNode> leftNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_BUTTON));
    leftNode->setScale(1.0f);
    _leftButton = scene2::Button::alloc(leftNode);
    //_leftButton->setContentSize(_leftButton->getContentSize() / 2.0f);
    _leftButton->setAnchor(Vec2::ANCHOR_CENTER);
    _leftButton->setPosition(_size.width * 0.3f, _size.height * 0.1f);
    _leftButton->setScale(0.2f);
    //_leftButton->setContentSize(_leftButton->getContentSize() / 3);
    _leftButton->activate();
    _leftButton->addListener([this](const std::string& name, bool down) {
        _leftpressed = down;
        });

    std::shared_ptr<scene2::PolygonNode> readyNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    readyNode->setScale(1.0f);
    _readyButton = scene2::Button::alloc(readyNode);
    _readyButton->setAnchor(Vec2::ANCHOR_CENTER);
    // Sorry, I used this as the save button. Probably shouldn't have done that, but too late now.
    // Might refactor later. Might not.
    _readyButton->setPosition(_size.width * 0.08f, _size.height * 0.20f);
    _readyButton->setScale(0.5f);
    _readyButton->activate();
    _readyButton->addListener([this](const std::string& name, bool down) {
        // Runs when the button is released, not when it is first pressed
        if (!down && !_isReady) {
            setIsReady(true);
        }
        });
    std::shared_ptr<scene2::PolygonNode> loadNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(READY_BUTTON));
    loadNode->setScale(1.0f);
    _loadButton = scene2::Button::alloc(loadNode);
    _loadButton->setAnchor(Vec2::ANCHOR_CENTER);
    _loadButton->setPosition(_size.width * 0.1f, _size.height * 0.9f);
    _loadButton->setScale(0.5f);
    _loadButton->activate();
    _loadButton->addListener([this](const std::string& name, bool down) {
        // This runs when the button is RELEASED.
        // This avoids issues with calling the loading logic every frame while the button is down.
        if (!down && !_isTimeToLoad) {
            setLoadClicked(true);
        }
        });

    std::shared_ptr<scene2::PolygonNode> paintNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BRUSH_BUTTON));
    paintNode->setScale(0.8f);
    _paintButton = scene2::Button::alloc(paintNode);
    _paintButton->setAnchor(Vec2::ANCHOR_CENTER);
    _paintButton->setPosition(_size.width * 0.06f, _size.height * 0.6f);
    _paintButton->activate();
    _paintButton->addListener([this](const std::string& name, bool down) {
        // If the paint mode was just entered
        if (down && !_paintButtonDown) {
            _inPaintMode = true;
            _inEraserMode = false;
            _paintButtonDown = !_paintButtonDown;
            _eraserButtonDown = false;
            _paintButton->setColor(Color4(255, 0, 0, 255));
            _eraserButton->setColor(Color4(255, 255, 255, 255));
        }
        // If the paint mode was just exited
        else if (down && _paintButtonDown) {
            _inPaintMode = false;
            _paintButtonDown = !_paintButtonDown;
            _paintButton->setColor(Color4(255, 255, 255, 255));
        }
        });

    std::shared_ptr<scene2::PolygonNode> eraserNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(ERASER_BUTTON));
    eraserNode->setScale(0.8f);
    _eraserButton = scene2::Button::alloc(eraserNode);
    _eraserButton->setAnchor(Vec2::ANCHOR_CENTER);
    _eraserButton->setPosition(_size.width * 0.06f, _size.height * 0.4f);

    // TODO: make this actually work
    _eraserButton->activate();
    _eraserButton->addListener([this](const std::string& name, bool down) {
        // If the paint button was just pressed
        if (down && !_eraserButtonDown) {
            _inEraserMode = true;
            _inPaintMode = false;
            _eraserButtonDown = !_eraserButtonDown;
            _paintButtonDown = false;
            _eraserButton->setColor(Color4(255, 0, 0, 255));
            _paintButton->setColor(Color4(255, 255, 255, 255));
        }
        // If the paint button was just released
        else if (down && _eraserButtonDown) {
            _inEraserMode = false;
            _eraserButtonDown = !_eraserButtonDown;
            _eraserButton->setColor(Color4(255, 255, 255, 255));
        }
        });
    _fileSaveText = scene2::TextField::allocWithTextBox(Size(200, 100), "save", _assets->get<Font>("marker"));
    _fileSaveText->setAnchor(Vec2::ANCHOR_CENTER);
    _fileSaveText->setPosition(_size.width * 0.13f, _size.height * 0.03f);

    _fileLoadText = scene2::TextField::allocWithTextBox(Size(200, 100), "load", _assets->get<Font>("marker"));
    _fileLoadText->setAnchor(Vec2::ANCHOR_CENTER);
    _fileLoadText->setPosition(_size.width * 0.13f, _size.height * 0.73f);
    addChild(_rightButton);
    addChild(_readyButton);
    addChild(_leftButton);
    addChild(_fileSaveText);
    addChild(_fileLoadText);
    addChild(_loadButton);
    addChild(_paintButton);
    addChild(_eraserButton);

    return true;
}

/**
 * Initializes the grid layout on the screen for build mode.
 */
void LevelEditorUIScene::initInventory(std::vector<Item> inventoryItems, std::vector<std::string> assetNames)
{
    // Set the background
    _inventoryBackground = scene2::PolygonNode::alloc();
    _inventoryBackground->setPosition(Vec2(_size.width * 0.88, _size.height * 0.2));
    _inventoryBackground->setContentSize(Size(_size.width * 0.18, _size.height * 0.8));
    _inventoryBackground->setColor(Color4(131, 111, 108));
    _inventoryBackground->setVisible(true);
    addChild(_inventoryBackground);

    float yOffset = 0;
    float count = 0;
    float page_temp = 0;
    for (size_t itemNo = 0; itemNo < inventoryItems.size(); itemNo++)
    {
        std::shared_ptr<scene2::PolygonNode> itemNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(assetNames[itemNo]));
        std::shared_ptr<scene2::Button> itemButton = scene2::Button::alloc(itemNode);
        itemButton->setAnchor(Vec2::ANCHOR_TOP_RIGHT);
        itemButton->setPosition(_size.width - 10 - page_temp * 60, _size.height - 100 - count * 40);
        itemButton->setScale(itemButton->getScale() / 2);
        itemButton->setName(itemToString(inventoryItems[itemNo]));
        itemButton->setVisible(true);
        itemButton->activate();
        _inventoryButtons.push_back(itemButton);
        addChild(itemButton);
        yOffset += 80;
        count = (count + 1);
        if (count >= 11) {
            count = 0;
            page_temp++;
        }
    }
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void LevelEditorUIScene::reset() {
    setVisible(true);
    activateInventory(true);
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void LevelEditorUIScene::preUpdate(float dt) {

}

#pragma mark -
#pragma mark Attribute Functions
/**
 * Sets whether the player has pressed the ready button to indicate they are done with build phase.
 */
void LevelEditorUIScene::setIsReady(bool isDone) {
    if (isDone) {
        _readyButton->setColor(Color4::GRAY);
        _isReady = true;
    }
    else {
        _readyButton->setColor(Color4::WHITE);
        _isReady = false;
    }
}

#pragma mark -
#pragma mark Helpers
/**
 * Set whether the elements of this scene are visible or not
 */
void LevelEditorUIScene::setVisible(bool value) {
    for (size_t i = 0; i < _inventoryButtons.size(); i++)
    {
        _inventoryButtons[i]->setVisible(value);
    }
    _inventoryBackground->setVisible(value);

    _leftButton->setVisible(value);
    _rightButton->setVisible(value);
    _readyButton->setVisible(value);
}

/** Sets whether or not the load button was pressed. */
void LevelEditorUIScene::setLoadClicked(bool value) {
    _isTimeToLoad = value;
}

/**
 * Whether to activate the inventory.
 */
void LevelEditorUIScene::activateInventory(bool value) {
    for (size_t i = 0; i < _inventoryButtons.size(); i++)
    {
        if (value) {
            _inventoryButtons[i]->activate();
        }
        else {
            _inventoryButtons[i]->deactivate();
        }
    }
}
