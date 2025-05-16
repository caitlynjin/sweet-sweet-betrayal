//
//  NPHostScene.h
//  Networked Physics
//
//  This class represents the scene for the host when creating a game. Normally
//  this class would be combined with the class for the client scene (as both
//  initialize the network controller).  But we have separated to make the code
//  a little clearer for this lab.
//
//  Author: Walker White
//  Version: 1/10/24
//
#ifndef __NL_HOST_SCENE_H__
#define __NL_HOST_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include <cugl/netcode/CUNetcodeConfig.h>
#include <cugl/physics2/distrib/CUNetEventController.h>
#include "SoundController.h"
#include "NetworkController.h"


#define PING_TEST_COUNT 5

using namespace cugl::physics2::distrib;
//using namespace cugl;

/**
 * This class provides the interface to make a new game.
 *
 * Most games have a since "matching" scene whose purpose is to initialize the
 * network controller.  We have separate the host from the client to make the
 * code a little more clear.
 */
class HostScene : public cugl::scene2::Scene2 {

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** The menu button for starting a game */
    std::shared_ptr<cugl::scene2::Button> _startgame;
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;
    /** The game id label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _gameid;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _player;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    
    std::vector<std::shared_ptr<cugl::scene2::SceneNode>> _tips;
    int _currentTipIndex = 0;
    float _tipSwitchTimer = 0.0f;
    float _tipInterval = 3.0f;
    
//    /** The player icons for showing players joined */
//    std::shared_ptr<cugl::scene2::PolygonNode> _player1Icon;
//    std::shared_ptr<cugl::scene2::PolygonNode> _player2Icon;
//    std::shared_ptr<cugl::scene2::PolygonNode> _player3Icon;
//    std::shared_ptr<cugl::scene2::PolygonNode> _player4Icon;
    
//    /** The texture to use when the player icon gets filled */
//    std::shared_ptr<graphics::Texture> _filledIcon;
//    std::shared_ptr<graphics::Texture> _emptyIcon;

    /** Whether the startGame button had been pressed. */
    bool _startGameClicked = false;
    /** Whether the back button had been pressed. */
    bool _backClicked = false;
    
    /** The network configuration */
    cugl::netcode::NetcodeConfig _config;

    /** The network controller */
    std::shared_ptr<NetworkController> _networkController;
    /** The network */
    std::shared_ptr<cugl::physics2::distrib::NetEventController> _network;

    /** The sound controller */
    std::shared_ptr<SoundController> _sound;
    
    cugl::Timestamp _pingTimer;
    
    Uint64 _totalPing;
    
    int _sendCount;
    
    int _receiveCount;
    
    bool _hostMessageSent = false;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new host scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    HostScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~HostScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Resets all properties of the scene and network related properties.
     */
    void reset() override;
    
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
     * Returns whether the back button is pressed
     */
    bool getBackClicked() { return _backClicked; };

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
     * This method prompts the network controller to start the game.
     */
    void startGame();
    
};

#endif /* __NL_HOST_SCENE_H__ */
