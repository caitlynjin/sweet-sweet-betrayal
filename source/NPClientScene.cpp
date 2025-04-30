//
//  NLClientScene.cpp
//  Network Lab
//
//  This class represents the scene for the client when joining a game. Normally
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
#include <array>

#include "NPClientScene.h"

using namespace cugl;
using namespace cugl::physics2::distrib;

#pragma mark -
#pragma mark Level Layout

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/**
 * Converts a decimal string to a hexadecimal string
 *
 * This function assumes that the string is a decimal number less
 * than 65535, and therefore converts to a hexadecimal string of four
 * characters or less (as is the case with the lobby server). We
 * pad the hexadecimal string with leading 0s to bring it to four
 * characters exactly.
 *
 * @param dec the decimal string to convert
 *
 * @return the hexadecimal equivalent to dec
 */
static std::string dec2hex(const std::string dec) {
    Uint32 value = strtool::stou32(dec);
    if (value >= 655366) {
        value = 0;
    }
    return strtool::to_hexstring(value,4);
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool ClientScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
           return false;
   }

   if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT))) {
       return false;
   }
    Size dimen = getSize();
    
    // Start up the input handler
    _assets = assets;
    _networkController = networkController;
    _network = _networkController->getNetwork();
    _sound = sound;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("client");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.join"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.back"));
    _player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.center.players.field.text"));
    
    _gameID = {' ', ' ', ' ', ' ', ' '};
    _gameIDLength = 0;
    
    _gameid1 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.code-entry.num_1.field.text"));
    _gameid2 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.code-entry.num_2.field.text"));
    _gameid3 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.code-entry.num_3.field.text"));
    _gameid4 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.code-entry.num_4.field.text"));
    _gameid5 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.code-entry.num_5.field.text"));
    
    _button0 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button0"));
    _button1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button1"));
    _button2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button2"));
    _button3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button3"));
    _button4 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button4"));
    _button5 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button5"));
    _button6 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button6"));
    _button7 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button7"));
    _button8 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button8"));
    _button9 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button9"));
    
    _deleteButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.button-del"));
    
    _backout->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _network->disconnect();
            _backClicked = true;
            _gameID = {' ', ' ', ' ', ' ', ' '};
            _gameIDLength = 0;
            
        }
    });

    _startgame->addListener([=,this](const std::string& name, bool down) {
        if (!down) {
            _network->connectAsClient(dec2hex(std::string(_gameID.begin(), _gameID.end())));
        }
    });
    _button0->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('0');}});
    _button1->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('1');}});
    _button2->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('2');}});
    _button3->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('3');}});
    _button4->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('4');}});
    _button5->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('5');}});
    _button6->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('6');}});
    _button7->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('7');}});
    _button8->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('8');}});
    _button9->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('9');}});
    
    _deleteButton->addListener([this](const std::string& name, bool down) {if (!down) { deleteGameID();}});
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void ClientScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

void ClientScene::reset() {
    _backClicked = false;
    _gameID = {' ', ' ', ' ', ' ', ' '};
    _gameIDLength = 0;
    setGameIDLabels(_gameID);
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
void ClientScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        
        /**
         * This is similar to HostScene. if value is true, you need to activate the _backout button, and set the clicked variable to false. However, you should start a connection this time. If the value is false, you should disconnect the network controller, and reset all buttons and textfields to their original state.
         */
#pragma mark BEGIN SOLUTION
        if (value) {
            _backout->activate();
            _player->setText("1");
            
            _button0->activate();
            _button1->activate();
            _button2->activate();
            _button3->activate();
            _button4->activate();
            _button5->activate();
            _button6->activate();
            _button7->activate();
            _button8->activate();
            _button9->activate();
            
            _deleteButton->activate();
            
            setGameIDLabels(_gameID);
            
            configureStartButton();
            _backClicked = false;
            // Don't reset the room id
        } else {
            _startgame->deactivate();
            _backout->deactivate();
            
            _button0->deactivate();
            _button1->deactivate();
            _button2->deactivate();
            _button3->deactivate();
            _button4->deactivate();
            _button5->deactivate();
            _button6->deactivate();
            _button7->deactivate();
            _button8->deactivate();
            _button9->deactivate();
            
            _deleteButton->deactivate();
            
            //_network = nullptr;
            // If any were pressed, reset them
            _startgame->setDown(false);
            _backout->setDown(false);
            
            _gameID = {' ', ' ', ' ', ' ', ' '};
            _gameIDLength = 0;
            setGameIDLabels(_gameID);
            
        }
#pragma mark END SOLUTION
    }
}

/**
 * Checks that the network connection is still active.
 *
 * Even if you are not sending messages all that often, you need to be calling
 * this method regularly. This method is used to determine the current state
 * of the scene.
 *
 * @return true if the network connection is still active.
 */
void ClientScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
//    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("up")->getChildByName("label"));
//    label->setText(text);

}

/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void ClientScene::update(float timestep) {
    // Do this last for button safety
    configureStartButton();
    setGameIDLabels(_gameID);
    if(_network->getStatus() == NetEventController::Status::CONNECTED || _network->getStatus() == NetEventController::Status::HANDSHAKE){
        
        _player->setText(std::to_string(_network->getNumPlayers()));
    }
}

/**
 * Reconfigures the start button for this scene
 *
 * This is necessary because what the buttons do depends on the state of the
 * networking.
 */
void ClientScene::configureStartButton() {
    if (_network->getStatus() == NetEventController::Status::IDLE) {
//        _startgame->setDown(false);
        _startgame->activate();
        updateText(_startgame, "Start Game");
    }
    else if (_network->getStatus() == NetEventController::Status::CONNECTING) {
//        _startgame->setDown(false);
        _startgame->deactivate();
        updateText(_startgame, "Connecting");
    }
    else if (_network->getStatus() == NetEventController::Status::CONNECTED) {
        _startgame->setDown(false);
        _startgame->deactivate();
        updateText(_startgame, "Waiting");
    }
}

/**
 * Adds the num character to the end of the game ID. If the game ID is full (already 6 digits), do nothing.
 */
void ClientScene::appendGameID(char num){
    if (_gameIDLength < 5){
        _gameID[_gameIDLength] = num;
        _gameIDLength += 1;
    }
}

/**
 * Sets the character in the last non-empty index of game ID to empty. If the game ID is empty, do nothing.
 */
void ClientScene::deleteGameID(){
    if (_gameIDLength > 0){
        _gameID[_gameIDLength-1] = ' ';
        _gameIDLength -= 1;
    }
}

/**
 * Set the gameid labels to match the characters in the given gameid array.
 */
void ClientScene::setGameIDLabels(std::array<char, 5> gameid){
    _gameid1->setText(std::string(1, gameid[0]));
    _gameid2->setText(std::string(1, gameid[1]));
    _gameid3->setText(std::string(1, gameid[2]));
    _gameid4->setText(std::string(1, gameid[3]));
    _gameid5->setText(std::string(1, gameid[4]));
}

