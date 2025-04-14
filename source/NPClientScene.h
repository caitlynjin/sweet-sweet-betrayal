//
//  NPClientScene.h
//  Networked Physics
//
//  This class represents the scene for the client when joining a game. Normally
//  this class would be combined with the class for the client scene (as both
//  initialize the network controller).  But we have separated to make the code
//  a little clearer for this lab.
//
//  Author: Walker White
//  Version: 1/10/24
//
#ifndef __NL_CLIENT_SCENE_H__
#define __NL_CLIENT_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include <cugl/netcode/CUNetcodeConfig.h>
#include <cugl/physics2/distrib/CUNetEventController.h>
#include "SoundController.h"
#include "NetworkController.h"



using namespace cugl::physics2::distrib;

/**
 * This class provides the interface to join an existing game.
 *
 * Most games have a since "matching" scene whose purpose is to initialize the
 * network controller.  We have separate the host from the client to make the
 * code a little more clear.
 */
class ClientScene : public cugl::scene2::Scene2 {
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
   

    /** The menu button for starting a game */
    std::shared_ptr<cugl::scene2::Button> _startgame;
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _player;
    
    /** The game id entered in by the user represented by an array*/
    std::array<char, 5> _gameID;
    /** Number of index in gameID array  that isn't empty (' ') */
    int _gameIDLength;
    
    /** Gameid textfield no. 1*/
    std::shared_ptr<cugl::scene2::Label> _gameid1;
    /** Gameid textfield no. 2*/
    std::shared_ptr<cugl::scene2::Label> _gameid2;
    /** Gameid textfield no. 3*/
    std::shared_ptr<cugl::scene2::Label> _gameid3;
    /** Gameid textfield no. 4*/
    std::shared_ptr<cugl::scene2::Label> _gameid4;
    /** Gameid textfield no. 5*/
    std::shared_ptr<cugl::scene2::Label> _gameid5;

    /** The numpad button for 0 */
    std::shared_ptr<cugl::scene2::Button> _button0;
    /** The numpad button for 1 */
    std::shared_ptr<cugl::scene2::Button> _button1;
    /** The numpad button for 2 */
    std::shared_ptr<cugl::scene2::Button> _button2;
    /** The numpad button for 3 */
    std::shared_ptr<cugl::scene2::Button> _button3;
    /** The numpad button for 4 */
    std::shared_ptr<cugl::scene2::Button> _button4;
    /** The numpad button for 5 */
    std::shared_ptr<cugl::scene2::Button> _button5;
    /** The numpad button for 6 */
    std::shared_ptr<cugl::scene2::Button> _button6;
    /** The numpad button for 7 */
    std::shared_ptr<cugl::scene2::Button> _button7;
    /** The numpad button for 8 */
    std::shared_ptr<cugl::scene2::Button> _button8;
    /** The numpad button for 9 */
    std::shared_ptr<cugl::scene2::Button> _button9;
    
    /** The delete button */
    std::shared_ptr<cugl::scene2::Button> _deleteButton;
    
    /** The network configuration */
    cugl::netcode::NetcodeConfig _config;
    
    /** The network controller */
    std::shared_ptr<NetworkController> _networkController;
    
    /** The network */
    std::shared_ptr<cugl::physics2::distrib::NetEventController> _network;

    /** The sound controller */
    std::shared_ptr<SoundController> _sound;

    
    /** Whether the back button had been clicked. */
    bool _backClicked = false;

    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new client scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    ClientScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~ClientScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound);

    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;

    /**
     * The method called to update the scene.
     *
     * We need to update this method to constantly talk to the server
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Returns whether the back button has been clicked
     */
    bool getBackClicked() { return _backClicked; }

private:
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
    void updateText(const std::shared_ptr<cugl::scene2::Button>& button, const std::string text);

    /**
     * Reconfigures the start button for this scene
     *
     * This is necessary because what the buttons do depends on the state of the
     * networking.
     */
    void configureStartButton();
    
    /**
     * Adds the num character to the end of the game ID. If the game ID is full (already 6 digits), do nothing.
     */
    void appendGameID(char num);
    
    /**
     * Sets the character in the last non-empty index of game ID to empty. If the game ID is empty, do nothing.
     */
    void deleteGameID();
    
    /**
     * Set the gameid labels to match the characters in the given gameid array.
     */
    void setGameIDLabels(std::array<char, 5> gameid);
};

#endif /* __NL_GAME_SCENE_H__ */
