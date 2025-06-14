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

#define SCENE_WIDTH 1306
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
bool ClientScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController>& sound) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
           return false;
   }

   if (!Scene2::initWithHint(Size(SCENE_WIDTH, 0))) {
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
    
    _background = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.roomcode-background"));
    if (_background) {
        _background->setAnchor(Vec2::ANCHOR_CENTER);
        Size tex = _background->getContentSize();
        float scale = dimen.height / tex.height;
        _background->setScale(scale, scale);
        _background->setPosition(dimen.width/2, dimen.height/2);
    }
    
    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.left.join"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.back"));
    
    _gameID = {' ', ' ', ' ', ' ', ' '};
    _gameIDLength = 0;
    
    _gameid1 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.left.code-entry.num_1.text"));
    _gameid2 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.left.code-entry.num_2.text"));
    _gameid3 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.left.code-entry.num_3.text"));
    _gameid4 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.left.code-entry.num_4.text"));
    _gameid5 = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client.left.code-entry.num_5.text"));
    
    _button0 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button0"));
    _button1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button1"));
    _button2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button2"));
    _button3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button3"));
    _button4 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button4"));
    _button5 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button5"));
    _button6 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button6"));
    _button7 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button7"));
    _button8 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button8"));
    _button9 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button9"));
    _deleteButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client.numpad.numbers.button_del"));
    
    _codeNotFound = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("client.left.code-not-found"));
    _codeNotFound->setVisible(false);
    
    _backout->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _network->disconnect();
            _backClicked = true;
            _gameID = {' ', ' ', ' ', ' ', ' '};
            _gameIDLength = 0;
            _sound->playSound("button_click");
            
        }
    });

    _startgame->addListener([=,this](const std::string& name, bool down) {
        if (!down) {
            _isJoining = true;
            _codeNotFound->setVisible(false);
            _network->connectAsClient(dec2hex(std::string(_gameID.begin(), _gameID.end())));
            _sound->playSound("button_click");
        }
    });
    _button0->addListener([this](const std::string& name, bool down) {if (!down) {
        CULog("0 Pressed");
        appendGameID('0'); _sound->playSound("numpad_0");
    }});
    _button1->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('1'); _sound->playSound("numpad_1"); }});
    _button2->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('2'); _sound->playSound("numpad_2");
    }});
    _button3->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('3'); _sound->playSound("numpad_3");
    }});
    _button4->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('4'); _sound->playSound("numpad_4");
    }});
    _button5->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('5'); _sound->playSound("numpad_5");
    }});
    _button6->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('6'); _sound->playSound("numpad_6");
    }});
    _button7->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('7'); _sound->playSound("numpad_7");
    }});
    _button8->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('8'); _sound->playSound("numpad_8");
    }});
    _button9->addListener([this](const std::string& name, bool down) {if (!down) { appendGameID('9'); _sound->playSound("numpad_9");
    }});
    
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
    reset();
    removeAllChildren();
    _active = false;
    
    _assets = nullptr;
    
    _startgame->clearListeners();
    _backout->clearListeners();
    _startgame = nullptr;
    _backout = nullptr;
//    _player = nullptr;
    
    _gameid1 = nullptr;
    _gameid2 = nullptr;
    _gameid3 = nullptr;
    _gameid4 = nullptr;
    _gameid5 = nullptr;

    _button0->clearListeners();
    _button1->clearListeners();
    _button2->clearListeners();
    _button3->clearListeners();
    _button4->clearListeners();
    _button5->clearListeners();
    _button6->clearListeners();
    _button7->clearListeners();
    _button8->clearListeners();
    _button9->clearListeners();
    _deleteButton->clearListeners();
    
    _button0 = nullptr;
//    _button0->removeListener(0);
    _button1 = nullptr;
    _button2 = nullptr;
    _button3 = nullptr;
    _button4 = nullptr;
    _button5 = nullptr;
    _button6 = nullptr;
    _button7 = nullptr;
    _button8 = nullptr;
    _button9 = nullptr;
    _deleteButton = nullptr;

    _networkController = nullptr;
    _network = nullptr;
    _sound = nullptr;
    
    Scene2::dispose();
    
}

void ClientScene::reset() {
    _backClicked = false;
    _gameID = {' ', ' ', ' ', ' ', ' '};
    _gameIDLength = 0;
    setGameIDLabels(_gameID);
    _codeNotFound->setVisible(false);
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
        if (value) {
            _sound->playMusic("waiting_scene", true);
        }
        /**
         * This is similar to HostScene. if value is true, you need to activate the _backout button, and set the clicked variable to false. However, you should start a connection this time. If the value is false, you should disconnect the network controller, and reset all buttons and textfields to their original state.
         */
#pragma mark BEGIN SOLUTION
        if (value) {
            _backout->activate();
            
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
            _codeNotFound->setVisible(false);
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
    if (_isJoining) {
        switch (_network->getStatus()) {
          case NetEventController::Status::CONNECTED:
            _isJoining = false;
            _showTransition = true;
            break;
          case NetEventController::Status::NETERROR:
            _isJoining = false;
            _codeNotFound->setVisible(true);
            _showTransition = false;
            break;
          default:
            break;
        }
      }
    
    // Do this last for button safety
    configureStartButton();
    setGameIDLabels(_gameID);
    if(_network->getStatus() == NetEventController::Status::CONNECTED || _network->getStatus() == NetEventController::Status::HANDSHAKE){
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

