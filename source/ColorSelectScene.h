//
//  ColorSelectScene.h
//  SweetSweetBetrayal
//
//  Created by Grace Sawatyanon on 27/4/25.
//

#ifndef __COLOR_SELECT_SCENE_H__
#define __COLOR_SELECT_SCENE_H__

#include <cugl/cugl.h>
#include "SoundController.h"
#include "SSBInput.h"
#include <array>
#include "Message.h"
#include "NetworkController.h"

class ColorSelectScene : public cugl::scene2::Scene2 {
public:
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to return to the previous scene */
        BACK,
        RED,
        BLUE,
        YELLOW,
        GREEN,
        /** User is ready to start the game */
        READY
    };
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    std::shared_ptr<SoundController> _sound;
    /** The network controller */
    std::shared_ptr<NetworkController> _networkController;
    /** The network */
    std::shared_ptr<cugl::physics2::distrib::NetEventController> _network;
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    /** The menu button for returning to the previous scene  */
    std::shared_ptr<cugl::scene2::Button> _backbutton;
    /** The button for starting a game */
    std::shared_ptr<cugl::scene2::Button> _readybutton;
    /** The button for choosing red  */
    std::shared_ptr<cugl::scene2::Button> _redbutton;
    /** The button for choosing blue  */
    std::shared_ptr<cugl::scene2::Button> _bluebutton;
    /** The button for choosing yellow  */
    std::shared_ptr<cugl::scene2::Button> _yellowbutton;
    /** The button for choosing green  */
    std::shared_ptr<cugl::scene2::Button> _greenbutton;
    std::shared_ptr<scene2::PolygonNode> _redNormal,   _redSelected,   _redTaken;
    std::shared_ptr<scene2::PolygonNode> _blueNormal,  _blueSelected,   _blueTaken;
    std::shared_ptr<scene2::PolygonNode> _yellowNormal,_yellowSelected,   _yellowTaken;
    std::shared_ptr<scene2::PolygonNode> _greenNormal, _greenSelected,   _greenTaken;
    
    Choice _choice;
    int _initialPlayerCount = 0;
    /** Tracks which colors are taken, indexed by (int)ColorType enum*/
    std::array<bool,4> _taken{false,false,false,false};

    ColorType _myColor = ColorType::RED;
    int  _prevTakenIndex = -1;
    bool _isReady;
    
public:
#pragma mark -
#pragma mark Constructors
    
    ColorSelectScene() :cugl::scene2::Scene2() {}
    
    ~ColorSelectScene() { dispose(); }
    
    void dispose() override;
    
    void reset() override;
    
    /**
     * Initialize this scene with assets and sound controller.
     *
     * @param assets  Loaded asset manager
     * @param networkController the Network Controller
     * @param sound   Sound controller
     * @return true if successful
     */
    bool init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<NetworkController> networkController, const std::shared_ptr<SoundController>& sound);
    
    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    Choice getChoice() const { return _choice; }
    
    void setInitialPlayerCount(int count){ _initialPlayerCount = count;}
    
    int getInitialPlayerCount() const { return _initialPlayerCount; }
    
    /** Update the screen after a color is taken */
    void _updateColorTaken(ColorType newColor, int oldColorInt);
    /** Update the screen after the player selects a color button */
    void _updateSelectedColor(ColorType c);
    /** Update the screen to clear the previously taken color */
    void _clearTaken(int oldColorInt);
    /** Reset the buttons */
    void _resetButtons();
    /** Visually change the ready button depending on the input */
    void _setReadyEnabled(bool enable);
    /** Returns isReady */
    bool _getReady(){ return _isReady; }
};

#endif // __COLOR_SELECT_SCENE_H__
