//
//  LevelSelect.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 5/1/25.
//

#ifndef LevelSelectScene_h
#define LevelSelectScene_h

#include <cugl/cugl.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "SoundController.h"
#include "NetworkController.h"
#include "LevelEvent.h"
//#include <cmath>

using namespace cugl;
using namespace Constants;

/**
 * This class presents the start screen to the player.
 */
class LevelSelectScene : public cugl::scene2::Scene2 {
public:
    /**
     * The start screen choice.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to start a game */
        LEVEL1,
        LEVEL2,
        LEVEL3,
        BACK
    };

protected:
    
    // CONSTANTS
    float const MAX_ALPHA = 255.0f;
    
    float const MIN_ALPHA = 150.0f;
    
    float const FLASH_RATE = 2.0f;
    
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<SoundController> _sound;
    
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;
    
    std::shared_ptr<NetworkController> _networkController;
    /** The network */
    std::shared_ptr<cugl::physics2::distrib::NetEventController> _network;
    
    std::shared_ptr<cugl::scene2::Button> _level1;
    std::shared_ptr<cugl::scene2::Button> _level2;
    std::shared_ptr<cugl::scene2::Button> _level3;
    std::shared_ptr<cugl::scene2::Button> _backbutton;
    std::shared_ptr<scene2::PolygonNode> _hostText;
    
    std::shared_ptr<scene2::PolygonNode> _modalDarkener;
    std::shared_ptr<scene2::PolygonNode> _modalFrame;
    std::shared_ptr<scene2::PolygonNode> _levelImage;
    std::shared_ptr<scene2::PolygonNode> _levelName;
    std::shared_ptr<cugl::scene2::Button> _playButton;
    std::shared_ptr<cugl::scene2::Button> _closeButton;
    
    int _levelView = 0;
    
    bool _playPressed = false;
    bool _decreaseAlpha = true;
    bool _levelPressed = false;
    bool _closePressed = false;
    
    // Booleans for client-side
    bool _showModal = false;

    
    /** Glider image variables */
    cugl::Vec2 _leftBasePos;
    cugl::Vec2 _rightBasePos;
    
    /** The player start screen choice */
    Choice _choice;

public:
#pragma mark -
#pragma mark Constructors
    LevelSelectScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LevelSelectScene() { dispose(); }
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<NetworkController> networkController, const std::shared_ptr<SoundController> &sound);
    
    
    virtual void update(float timestep) override;
    
    /**
     * Resets all properties of the scene.
     */
    void reset() override;
    
    virtual void setActive(bool value) override;
    
    Choice getChoice() const { return _choice; }
    
    /**
     Shows the level modal for the appropriate level number
     */
    void selectLevel(int levelNum);
    
    /**
        Sets the visibility of the modal.
     */
    void setModalVisible(bool visibile);
    
    /**
        Sets the active state  of the modal.
     */
    void setModalActive(bool value);
    
    /** Visually updates the alpha values of the level select buttons */
    void animateButtons();

};

#endif /* LevelSelectScene_h */
