#ifndef __PF_APP_H__
#define __PF_APP_H__
#include <cugl/cugl.h>
#include "SSBGameController.h"
#include "SSBLoadingScene.h"
#include "StartScene.h"
#include "SettingScene.h"
#include "HelpScene.h"
#include "CreditsScene.h"
#include "MenuScene.h"
#include "TransitionScene.h"
#include "SSBInput.h"
#include "NPClientScene.h"
#include "NPHostScene.h"
#include "NetworkController.h"
#include "VictoryScene.h"
#include "PauseScene.h"
#include "SoundController.h"
#include "LevelEditorController.h"
#include "ColorSelectScene.h"
#include "WaitingHostScene.h"
#include "LevelSelectScene.h"
#include "DisconnectedScene.h"
#include <cugl/physics2/distrib/CUNetEventController.h>
#include "Constants.h"


using namespace cugl::physics2::distrib;

/**
 * This class represents the application root for the platform demo.
 */
class SSBApp : public cugl::Application {
    enum Status {
        LOAD, START, MENU, HOST, CLIENT, LEVEL_SELECT, GAME, LEVEL_EDITOR, VICTORY, COLOR_SELECT, WAITING_HOST, DISCONNECTED, SETTING, CREDITS, HELP
    };
protected:
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    StartScene _startscreen;

    SettingScene _settingscreen;

    CreditsScene _creditsscreen;

    HelpScene _helpscreen;
    
    /**neworking scenes*/
    MenuScene _mainmenu;
    
    ClientScene _joingame;
        
    HostScene _hostgame;
    
    VictoryScene _victory;

    PauseScene _pause;


    TransitionScene _transition;
    
    LevelSelectScene _levelSelect;
    
    bool _doTransition = false;

    ColorSelectScene _colorselect;
    
    WaitingHostScene _waitinghost;
    
    DisconnectedScene _disconnectedscreen;

    
    /***/
    PlatformInput _input;
    
    /** The controller for the loading screen */
    SSBLoadingScene _loading;

    /** The controller for handling gameplay */
    SSBGameController _gameController;

    /** The controller for handling the level editor */
    LevelEditorController _levelEditorController;

    /** The controller for handling networking */
    std::shared_ptr<NetworkController> _networkController;

    /** The network */
    std::shared_ptr<cugl::physics2::distrib::NetEventController> _network;

    /** The controller for handling audio logic */
    std::shared_ptr<SoundController> _sound;
    
    /** Whether or not we have finished loading all assets */
    bool _loaded;
    
    Status _status;
    
    int _expectedPlayers;
    
public:
#pragma mark Constructors
    /**
     * Creates, but does not initialized a new application.
     *
     * This constructor is called by main.cpp.  You will notice that, like
     * most of the classes in CUGL, we do not do any initialization in the
     * constructor.  That is the purpose of the init() method.  Separation
     * of initialization from the constructor allows main.cpp to perform
     * advanced configuration of the application before it starts.
     */
    SSBApp() : cugl::Application(), _loaded(false) {}
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits.
     * It simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~SSBApp() { }
    
    
#pragma mark Application State

    /**
     * The method called after OpenGL is initialized, but before running the application.
     *
     * This is the method in which all user-defined program intialization should
     * take place.  You should not create a new init() method.
     *
     * When overriding this method, you should call the parent method as the
     * very last line.  This ensures that the state will transition to FOREGROUND,
     * causing the application to run.
     */
    virtual void onStartup() override;
    
    /**
     * The method called when the application is ready to quit.
     *
     * This is the method to dispose of all resources allocated by this
     * application.  As a rule of thumb, everything created in onStartup()
     * should be deleted here.
     *
     * When overriding this method, you should call the parent method as the
     * very last line.  This ensures that the state will transition to NONE,
     * causing the application to be deleted.
     */
    virtual void onShutdown() override;
    
    /**
     * The method called when the application is suspended and put in the background.
     *
     * When this method is called, you should store any state that you do not
     * want to be lost.  There is no guarantee that an application will return
     * from the background; it may be terminated instead.
     *
     * If you are using audio, it is critical that you pause it on suspension.
     * Otherwise, the audio thread may persist while the application is in
     * the background.
     */
    virtual void onSuspend() override;
    
    /**
     * The method called when the application resumes and put in the foreground.
     *
     * If you saved any state before going into the background, now is the time
     * to restore it. This guarantees that the application looks the same as
     * when it was suspended.
     *
     * If you are using audio, you should use this method to resume any audio
     * paused before app suspension.
     */
    virtual void onResume()  override;
    
    
#pragma mark Application Loop
    /**
     * The method called to update the application data.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should contain any code that is not an OpenGL call.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    virtual void update(float dt) override;
    
    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * This method is used instead of {@link #update} if {@link #setDeterministic}
     * is set to true. It marks the beginning of the core application loop,
     * which is concluded with a call to {@link #postUpdate}.
     *
     * This method should be used to process any events that happen early in
     * the application loop, such as user input or events created by the
     * {@link schedule} method. In particular, no new user input will be
     * recorded between the time this method is called and {@link #postUpdate}
     * is invoked.
     *
     * Note that the time passed as a parameter is the time measured from the
     * start of the previous frame to the start of the current frame. It is
     * measured before any input or callbacks are processed. It agrees with
     * the value sent to {@link #postUpdate} this animation frame.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    virtual void preUpdate(float dt) override;
    
    /**
     * The method called to provide a deterministic application loop.
     *
     * This method provides an application loop that runs at a guaranteed fixed
     * timestep. This method is (logically) invoked every {@link getFixedStep}
     * microseconds. By that we mean if the method {@link draw} is called at
     * time T, then this method is guaranteed to have been called exactly
     * floor(T/s) times this session, where s is the fixed time step.
     *
     * This method is always invoked in-between a call to {@link #preUpdate}
     * and {@link #postUpdate}. However, to guarantee determinism, it is
     * possible that this method is called multiple times between those two
     * calls. Depending on the value of {@link #getFixedStep}, it can also
     * (periodically) be called zero times, particularly if {@link #getFPS}
     * is much faster.
     *
     * As such, this method should only be used for portions of the application
     * that must be deterministic, such as the physics simulation. It should
     * not be used to process user input (as no user input is recorded between
     * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
     */
    virtual void fixedUpdate() override;

    /**
     * The method called to indicate the end of a deterministic loop.
     *
     * This method is used instead of {@link #update} if {@link #setDeterministic}
     * is set to true. It marks the end of the core application loop, which was
     * begun with a call to {@link #preUpdate}.
     *
     * This method is the final portion of the update loop called before any
     * drawing occurs. As such, it should be used to implement any final
     * animation in response to the simulation provided by {@link #fixedUpdate}.
     * In particular, it should be used to interpolate any visual differences
     * between the the simulation timestep and the FPS.
     *
     * This method should not be used to process user input, as no new input
     * will have been recorded since {@link #preUpdate} was called.
     *
     * Note that the time passed as a parameter is the time measured from the
     * start of the previous frame to the start of the current frame. It is
     * measured before any input or callbacks are processed. It agrees with
     * the value sent to {@link #preUpdate} this animation frame.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    virtual void postUpdate(float dt) override;
    
    /**
     Resets the entire state of the application. Disposes of all scenes and the network and re-initializes them.
     */
    void resetApplication();
    
    /**
     Resets the entire state of the level controllers. Used when a party is still connected and wants to play another game.
     */
    void resetLevel();
    
    /**
     Disposes all scenes necessary to create a clean slate. 
     */
    void disposeScenes();
    
    
    void setTransition(bool value);
    
    
    /**
     * Inidividualized update method for the menu scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the menu scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateMenuScene(float timestep);
    
    void updateStartScene(float timestep);

    void updateSettingScene(float timestep);

    void updateCreditsScene(float timestep);

    void updateHelpScene(float timestep);
    
    /**
     * Inidividualized update method for the host scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the host scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateHostScene(float timestep);
    /**
     * Inidividualized update method for the client scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the client scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateClientScene(float timestep);
    
    void updateColorSelectScene(float timestep);
    
    void updateWaitingHostScene(float timestep);

    void updatePauseScene(float timestep);

    void updateDisconnectedScene(float timestep);
    
    void updateLevelSelectScene(float timestep);
    
    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    virtual void draw() override;
    
    
    /**
     Resets all properties of the scene prior to joining a game.
     */
    void resetScenes();
    
    /**
    * Sets up the asset, item, and JSON maps in Constants.
    * Had to this here because updating the static variables from within Constants does not work.
    */
    void populateMaps();
    
   // std::map<string, string> jsonTypeToAsset = {};
    //std::map<string, Item> jsonTypeToItemType = {};
    //std::map<Item, string> itemToAssetNameMap = {};
};
#endif /* __PF_APP_H__ */
