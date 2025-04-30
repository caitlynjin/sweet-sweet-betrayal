//
//  NLHostScene.cpp
//  Network Lab
//
//  This class represents the scene for the host when creating a game. Normally
//  this class would be combined with the class for the client scene (as both
//  initialize the network controller).  But we have separated to make the code
//  a little clearer for this lab.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "NPHostScene.h"
#include "Message.h"
#include "MessageEvent.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::netcode;
using namespace std;

#pragma mark -
#pragma mark Level Layout

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576


/**
 * Converts a hexadecimal string to a decimal string
 *
 * This function assumes that the string is 4 hexadecimal characters
 * or less, and therefore it converts to a decimal string of five
 * characters or less (as is the case with the lobby server). We
 * pad the decimal string with leading 0s to bring it to 5 characters
 * exactly.
 *
 * @param hex the hexadecimal string to convert
 *
 * @return the decimal equivalent to hex
 */
static std::string hex2dec(const std::string hex) {
    Uint32 value = strtool::stou32(hex,0,16);
    std::string result = strtool::to_string(value);
    if (result.size() < 5) {
        size_t diff = 5-result.size();
        std::string alt(5,'0');
        for(size_t ii = 0; ii < result.size(); ii++) {
            alt[diff+ii] = result[ii];
        }
        result = alt;
    }
    return result;
}

#pragma mark -
#pragma mark Provided Methods
/**
 * Initializes the controller contents, and starts the game
 *
 * In previous labs, this method "started" the scene.  But in this
 * case, we only use to initialize the scene user interface.  We
 * do not activate the user interface yet, as an active user
 * interface will still receive input EVEN WHEN IT IS HIDDEN.
 *
 * That is why we have the method {@link #setActive}.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool HostScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
           return false;
    }

    if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT))) {
       return false;
    }
    Size dimen = getSize();


    _networkController = networkController;
    _network = networkController->getNetwork();
    _sound = sound;
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("host");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host.start"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host.back"));
    _gameid = scene2::Label::allocWithText("0", _assets->get<Font>("yeasty flavorsRegular66.53518676757812"));
    _gameid->setAnchor(Vec2::ANCHOR_CENTER);
    _gameid->setPosition(_size.width * .50 + 10,_size.height * .65);
    _gameid->setContentWidth(_size.width * .3);
    _gameid->setForeground(Color4::WHITE);
    _gameid->setVisible(true);
    scene->addChild(_gameid);
    
    
    _player1Icon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.player-count.redglider"));
    _filledIcon = _player1Icon->getTexture();
    
    _player2Icon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.player-count.greyglider"));    
    _emptyIcon = _player2Icon->getTexture();
    
    _player3Icon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.player-count.greyglider_2"));
    
    _player4Icon = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("host.player-count.greyglider_3"));
    
    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _backClicked = true;
            _sound->playSound("button_click");
        }
    });

    _startgame->addListener([this](const std::string& name, bool down) {
        if (!down) {
            startGame();
            _sound->playSound("button_click");
        }
    });
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    _sendCount = 0;
    _receiveCount = 0;
    _totalPing = 0;

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void HostScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

/**
 * Resets all properties of the scene and network related properties.
 */
void HostScene::reset(){
    _startGameClicked = false;
    _backClicked = false;
    _hostMessageSent = false;
    
    _sendCount = 0;
    _receiveCount = 0;
    _totalPing = 0;
    
    _player2Icon->setTexture(_emptyIcon);
    _player3Icon->setTexture(_emptyIcon);
    _player4Icon->setTexture(_emptyIcon);
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
void HostScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        
        /**
         * If value is true, you need to activate the _backout button, and set the clicked variable to false. You need to also call the network controller to start a connection as a host. If the value is false, and reset all buttons and textfields to their original state.
         */
#pragma mark BEGIN SOLUTION
        if (value) {
            _backout->activate();
            _startgame->activate();
            _network->disconnect();
            _network->connectAsHost();
            _backClicked = false;
        } else {
            _gameid->setText("");
            _startgame->deactivate();
            updateText(_startgame, "INACTIVE");
            _backout->deactivate();
            // If any were pressed, reset them
            _startgame->setDown(false);
            _backout->setDown(false);
        }
#pragma mark END SOLUTION
    }
}


/**
 * Updates the text in the given button.
 *
 * Techincally a button does not contain text. A button is simply a scene graph
 * node with one child for the up state and another for the down state. So to
 * change the text in one of our buttons, we have to descend the scene graph.
 * This method simplifies this process for you.
 *
 * @param button    The button to modify
 * @param text      The new text value
 */
void HostScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
//    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("up")->getChildByName("label"));
//    label->setText(text);

}

#pragma mark -
#pragma mark Student Methods
/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void HostScene::update(float timestep) {
    /**
     * Check for the status of `_network` (The NetworkController). If it is CONNECTED, you would need to update the scene nodes so that _gameId displays the id of the room (converted from hex to decimal) and _player displays the number of players. Additionally, you should check whether the `_startgame` button has been pressed and update its text. If it is not pressed yet, then its should display "Start Game" and be activated, otherwise, it should be deactivated and show "Starting".
     */
#pragma mark BEGIN SOLUTION
    if(_network->getStatus() == NetEventController::Status::CONNECTED){
        
        
        if (!_startGameClicked) {
            updateText(_startgame, "Start Game");
            _startgame->activate();
        }
        else {
            
            updateText(_startgame, "Starting");
            _startgame->deactivate();
        }
		_gameid->setText(hex2dec(_network->getRoomID()));
//        _player->setText(std::to_string(_network->getNumPlayers()));
        if (_network->getNumPlayers() >= 2){
            _player2Icon->setTexture(_filledIcon);
        }
        else{
            _player2Icon->setTexture(_emptyIcon);
        }
        
        if (_network->getNumPlayers() >= 3){
            _player3Icon->setTexture(_filledIcon);
        }
        else{
            _player3Icon->setTexture(_emptyIcon);
        }
        
        if (_network->getNumPlayers() == 4){
            _player4Icon->setTexture(_filledIcon);
        }
        else{
            _player4Icon->setTexture(_emptyIcon);
        }
	}
#pragma mark END SOLUTION
}

/**
 * This method prompts the network controller to start the game.
 */
void HostScene::startGame(){
    //Call the network controller to start the game and set the _startGameClicked to true.
#pragma mark BEGIN SOLUTION
    _network->startGame();
//    _network->ga
    _startGameClicked = true;
#pragma mark END SOLUTION
}
