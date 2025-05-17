#include "SSBApp.h"
#include "SSBInput.h"
#include "Constants.h"
#include "ArtAssetMapHelper.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace cugl::audio;

#pragma mark -
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
void SSBApp::onStartup()
{
    _assets = AssetManager::alloc();
    _batch = SpriteBatch::alloc();

    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif

    Input::activate<Keyboard>();
    Input::activate<TextInput>();

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->loadDirectory("json/loading.json");

    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets, "json/assets.json");
    _loading.setSpriteBatch(_batch);

    // Queue up the other assets
    _loading.start();
    _status = LOAD;
    AudioEngine::start();

    NetworkLayer::start(NetworkLayer::Log::INFO);
    Application::onStartup(); // YOU MUST END with call to parent

    setDeterministic(true);
}

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
void SSBApp::onShutdown()
{
    _loading.dispose();
    _gameController.dispose();
    _startscreen.dispose();
    _settingscreen.dispose();
    _creditsscreen.dispose();
    _helpscreen.dispose();
    _mainmenu.dispose();
    _hostgame.dispose();
    _levelSelect.dispose();
    _joingame.dispose();
    _victory.dispose();
    _transition.dispose();

    // Is this correct way of diposing networkController?
    _networkController->dispose();
    _networkController = nullptr;
    _assets = nullptr;
    _batch = nullptr;

    // Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif

    AudioEngine::stop();
    Application::onShutdown(); // YOU MUST END with call to parent
}

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
void SSBApp::onSuspend()
{
    AudioEngine::get()->pause();
}

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
void SSBApp::onResume()
{
    AudioEngine::get()->resume();
}

#pragma mark -
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
void SSBApp::update(float dt)
{
}

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
void SSBApp::preUpdate(float dt)
{
    // Handle transition logic
    if (_doTransition){
        if (_transition.getFadingOutDone()){
            setTransition(false);
        }
        if (_transition.getFadingInDone()){
            _doTransition = false;
            _transition.reset();
            _transition.setActive(false);
        }
    }
    
    //    CULog("Status: %d", static_cast<int>(_status));
    if (_status == LOAD && _loading.isActive())
    {
        _loading.update(0.01f);
        if (_loading.isComplete()){
            _loading.setActive(false);
        }
    }
    else if (_status == LOAD)
    {

        _networkController = NetworkController::alloc(_assets);
        _network = _networkController->getNetwork();
        _network->attachEventType<MessageEvent>();
        _network->attachEventType<ColorEvent>();
        _sound = SoundController::alloc(_assets);

        populateMaps();
        _loading.dispose();
        _startscreen.init(_assets, _sound);
        _startscreen.setActive(true);

        _startscreen.setSpriteBatch(_batch);
        _settingscreen.init(_assets, _sound);
        _settingscreen.setSpriteBatch(_batch);
        _creditsscreen.init(_assets, _sound);
        _creditsscreen.setSpriteBatch(_batch);
        _helpscreen.init(_assets, _sound);
        _helpscreen.setSpriteBatch(_batch);
        _mainmenu.init(_assets, _sound);
        _mainmenu.setSpriteBatch(_batch);
        _hostgame.init(_assets, _networkController, _sound);
        _hostgame.setSpriteBatch(_batch);
        _levelSelect.init(_assets, _networkController, _sound);
        _levelSelect.setSpriteBatch(_batch);
        _joingame.init(_assets, _networkController, _sound);
        _joingame.setSpriteBatch(_batch);
        _victory.init(_assets, _sound, _networkController);
        _victory.setSpriteBatch(_batch);
        _pause.init(_assets, _sound);
        _pause.setSpriteBatch(_batch);
        _colorselect.init(_assets, _networkController, _sound);
        _colorselect.setSpriteBatch(_batch);
        _waitinghost.init(_assets, _sound);
        _waitinghost.setSpriteBatch(_batch);
        _disconnectedscreen.init(_assets, _sound);
        _disconnectedscreen.setSpriteBatch(_batch);
        
        _transition.init(_assets);
        _transition.setSpriteBatch(_batch);
        _doTransition = true;
        _transition.setActive(true);
        _transition.startFadeIn();

        _status = START;

        _sound->playMusic("main_menu", true);
    }
    else
    {
        _transition.preUpdate(dt);
        switch (_status)
        {
        case START:
            updateStartScene(dt);
            break;
        case SETTING:
            updateSettingScene(dt);
            break;
        case CREDITS:
            updateCreditsScene(dt);
            break;
        case HELP:
            updateHelpScene(dt);
            break;
        case MENU:
            updateMenuScene(dt);
            break;
        case HOST:
            updateHostScene(dt);
            break;
        case CLIENT:
            updateClientScene(dt);
            break;
        case WAITING_HOST:
            updateWaitingHostScene(dt);
            break;
        case COLOR_SELECT:
            updateColorSelectScene(dt);
            break;
        case LEVEL_SELECT:
            updateLevelSelectScene(dt);
            break;
        case GAME:
            _gameController.preUpdate(dt);
            if (_network->getNumPlayers() < _expectedPlayers || _network->getStatus() == NetEventController::Status::NETERROR) {
                CULog("DISCONNECTED: numplayers: %d expected: %d", _network->getNumPlayers(), _expectedPlayers);
                _network->disconnect();
                _networkController->flushConnection();
                
                _gameController.setActive(false);
                _disconnectedscreen.setActive(true);
                _status = DISCONNECTED;
                break;
            }
                
            //TODO: Check for a victory
                if (_gameController.getHasVictory()){
                    setTransition(true);
                    if (_transition.getFadingOutDone()){
                        _gameController.setActive(false);
                        //                    _gameController.reset();
                        _victory.setActive(true);
                        _status = VICTORY;
                    }
                }
            // Check for pressing pause button
                if (_gameController.getIsPaused()) {
                    setTransition(true);
                    if (_transition.getFadingOutDone()){
                        _gameController.setActive(false);
                        // TODO: Need to disable ready button
                        _gameController.setElementsActive(false);
                        _pause.setActive(true);
                        _status = PAUSED;
                    }
                }
            break;
        case LEVEL_EDITOR:
            _levelEditorController.preUpdate(dt);
            break;
        case VICTORY:
            _victory.preUpdate(dt);
            //TODO: Check for restart
                if (_networkController->getResetLevel()){
                    // Reset the level and return back to the game scene
//                    _victory.reset();
                    setTransition(true);
                    if (_transition.getFadingOutDone()){
                        _victory.setActive(false);
                        _gameController.reset();
                        _gameController.setActive(true);
                        _status = GAME;
                    }
                }
            //TODO: Check for quit to main menu
                if (_victory.getChoice() == VictoryScene::Choice::QUIT){
                    setTransition(true);
                    if (_transition.getFadingOutDone()){
                        _victory.setActive(false);
                        _victory.reset();
                        _startscreen.setActive(true);
                        //TODO: resetGame() method
                        resetScenes();
                        _status = START;
                    }
                }
            break;
        case PAUSED:
            updatePauseScene(dt);
            _gameController.preUpdate(dt);
            break;
        case DISCONNECTED:
            updateDisconnectedScene(dt);
            break;
        default:
            break;
        }
    }
}

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
void SSBApp::fixedUpdate()
{
    // Compute time to report to game scene version of fixedUpdate
    float time = getFixedStep() / 1000000.0f;
    if (_status == GAME || _status == PAUSED)
    {
        _gameController.fixedUpdate(time);
    }
    else if (_status == LEVEL_EDITOR) {
        _levelEditorController.fixedUpdate(time);
    }
    else if (_status == VICTORY){
        _victory.fixedUpdate(time);
    }
    if (_network)
    {
        _network->updateNet();
    }

}

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
void SSBApp::postUpdate(float dt)
{
    // Compute time to report to game scene version of postUpdate
    float time = getFixedRemainder() / 1000000.0f;
    if (_status == GAME || _status == PAUSED)
    {
        _gameController.postUpdate(time);
    }
    else if (_status == LEVEL_EDITOR) {
        _levelEditorController.postUpdate(time);
    }
    else if (_status == VICTORY){
        _victory.postUpdate(time);
    }
}
/**
 * Inidividualized update method for the menu scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the menu scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void SSBApp::updateMenuScene(float timestep)
{
    _mainmenu.update(timestep);
    switch (_mainmenu.getChoice())
    {
        case MenuScene::Choice::HOST:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _disconnectedscreen.reset();
                _disconnectedscreen.setActive(false);
                _expectedPlayers = 0;
                _mainmenu.setActive(false);
                _hostgame.setActive(true);
                _status = HOST;
            }
            break;
        case MenuScene::Choice::JOIN:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _disconnectedscreen.reset();
                _disconnectedscreen.setActive(false);
                _expectedPlayers = 0;
                _mainmenu.setActive(false);
                _joingame.setActive(true);
                _status = CLIENT;
            }
            break;
        case MenuScene::Choice::BACK:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _mainmenu.setActive(false);
                _startscreen.setActive(true);
                _status = START;
            }
            break;
        case MenuScene::Choice::NONE:
            // DO NOTHING
            break;
    }
}

void SSBApp::setTransition(bool value){
    if (value){
        if (_doTransition){
            return;
        }
    }
    _transition.setActive(true);
    _doTransition = true;
    if (value){
        _transition.startFadeOut();
    }
    else{
        _transition.startFadeIn();
    }
}

void SSBApp::updateStartScene(float timestep)
{
    _startscreen.update(timestep);
    switch (_startscreen.getChoice())
    {
    case StartScene::Choice::START:
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _disconnectedscreen.setActive(false);
            _expectedPlayers = 0;
            _startscreen.setActive(false);
            _mainmenu.setActive(true);
            _status = MENU;
        }

        break;
    case StartScene::Choice::LEVEL_EDITOR:
            _mainmenu.setActive(false);
            _networkController->setIsHost(true);
           _levelEditorController.init(_assets, _networkController, _sound, true);
           _levelEditorController.setActive(true);
           _levelEditorController.setSpriteBatch(_batch);
           _status = LEVEL_EDITOR;
          _network->markReady();
           break;
    case StartScene::Choice::SETTING:
        _startscreen.setActive(false);
        _settingscreen.setActive(true);
        _status = SETTING;
        break;
    case StartScene::Choice::NONE:
        // DO NOTHING
        break;
    }
}

void SSBApp::updateSettingScene(float timestep){
    _settingscreen.update(timestep);
    switch (_settingscreen.getChoice()){
        case SettingScene::Choice::EXIT:
            _settingscreen.setActive(false);
            _startscreen.setActive(true);
            _status = START;
            break;
        case SettingScene::Choice::HELP:
            _settingscreen.setActive(false);
            _helpscreen.setActive(true);
            _status = HELP;
            break;
        case SettingScene::Choice::CREDITS:
            _settingscreen.setActive(false);
            _creditsscreen.setActive(true);
            _status = CREDITS;
            break;
        case SettingScene::Choice::NONE:
            break;
    }
}

void SSBApp::updateHelpScene(float timestep) {
    _helpscreen.update(timestep);
    switch (_helpscreen.getChoice()){
        case HelpScene::Choice::BACK:
            _helpscreen.setActive(false);
            _settingscreen.setActive(true);
            _status = SETTING;
        case HelpScene::Choice::NONE:
            break;
    }
}

void SSBApp::updateCreditsScene(float timestep){
    _creditsscreen.update(timestep);
    switch (_creditsscreen.getChoice()){
        case CreditsScene::Choice::BACK:
            _creditsscreen.setActive(false);
            _settingscreen.setActive(true);
            _status = SETTING;
            break;
        case CreditsScene::Choice::NONE:
            break;
    }
}

/**
 * Inidividualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void SSBApp::updateHostScene(float timestep)
{
    _hostgame.update(timestep);
    _networkController->update(timestep);
    if (_hostgame.getBackClicked())
    {
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _network->disconnect();
            _networkController->flushConnection();
            _status = MENU;
            _hostgame.setActive(false);
            _mainmenu.setActive(true);
        }
    }
    else if (_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID())
    {
        CULog("HANDSHAKE");
        _networkController->setIsHost(true);
        _gameController.init(_assets, _networkController, _sound);
//        _gameController.setSpriteBatch(_batch);
        _network->markReady();
    }
    else if (_network->getStatus() == NetEventController::Status::INGAME)
    {
        CULog("INGAME");
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _hostgame.setActive(false);
            _colorselect.reset();
            _colorselect.setActive(true);
            _colorselect.setInitialPlayerCount(_network->getNumPlayers());
            _status = COLOR_SELECT;
            _network->pushOutEvent(MessageEvent::allocMessageEvent(Message::HOST_START));
        }
    }
    else if (_network->getStatus() == NetEventController::Status::NETERROR)
    {
        _network->disconnect();
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _hostgame.setActive(false);
            _mainmenu.setActive(true);
            _gameController.dispose();
            _status = MENU;
        }
    }
}

/**
 * Inidividualized update method for the level select scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void SSBApp::updateLevelSelectScene(float timestep){
    _levelSelect.update(timestep);
    _networkController->fixedUpdate(timestep);
    
    // Check if host has pressed play on a specific level
    if (_levelSelect.getChoice() != LevelSelectScene::Choice::NONE){
        int levelChoice = static_cast<int>(_levelSelect.getChoice());
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _gameController.setLevelNum(levelChoice);
            _gameController.finishInit();
            _gameController.setSpriteBatch(_batch);
            _levelSelect.setActive(false);
            _gameController.setActive(true);
            _status = GAME;
        }
    }
    
    return;
}

/**
 * Inidividualized update method for the client scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the client scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void SSBApp::updateClientScene(float timestep)
{
#pragma mark SOLUTION
    _joingame.update(timestep);
    _networkController->update(timestep);
    if (_joingame.getBackClicked())
    {
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _network->disconnect();
            _networkController->flushConnection();
            _status = MENU;
            _joingame.setActive(false);
            _mainmenu.setActive(true);
        }
    }
    else if (_network->getStatus() == NetEventController::Status::CONNECTED){
        _status = WAITING_HOST;
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _joingame.setActive(false);
            _waitinghost.setActive(true);
        }
    }
    else if (_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID())
    {
        _networkController->setIsHost(false);
        _gameController.init(_assets, _networkController, _sound);
//        _gameController.setSpriteBatch(_batch);
        _network->markReady();
    }
    else if (_network->getStatus() == NetEventController::Status::INGAME)
    {
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _joingame.setActive(false);
            _gameController.setActive(true);
            _status = GAME;
        }
    }
    else if (_network->getStatus() == NetEventController::Status::NETERROR)
    {
        _network->disconnect();
        if(_joingame.getShowTransition()){
            setTransition(true);
        }
    }
#pragma mark END SOLUTION
}

void SSBApp::updateColorSelectScene(float timestep){
    _colorselect.update(timestep);
    _networkController->fixedUpdate(timestep);
    
    if (_networkController->getNumColorReady() == _colorselect.getInitialPlayerCount()) {
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _colorselect.setActive(false);
//            _gameController.init(_assets, _networkController, _sound);
//            _gameController.setSpriteBatch(_batch);
//            _gameController.setActive(true);
            CULog("Switch to level select");
            _levelSelect.setActive(true);
            _expectedPlayers = _network->getNumPlayers();
            CULog("Expected players: %d", _expectedPlayers);
            _status = LEVEL_SELECT;
        }
        return;
    }
    
    if (_network->getNumPlayers() < _colorselect.getInitialPlayerCount() || _network->getStatus() == NetEventController::Status::NETERROR) {
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _colorselect.reset();
            _colorselect.setActive(false);
            _network->disconnect();
            _networkController->flushConnection();
            _networkController->resetColorReady();
            if (_networkController->getIsHost()) {
                _hostgame.reset();
                _hostgame.setActive(true);
                _status = HOST;
            } else {
                _joingame.reset();
                _joingame.setActive(true);
                _status = CLIENT;
            }
        }
        return;
    }
    
    if (_colorselect._getReady()){
        _colorselect._setReadyEnabled(false);
    }
    
    switch (_colorselect.getChoice())
    {
        case ColorSelectScene::Choice::BACK:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _colorselect.reset();
                _colorselect.setActive(false);
                _network->disconnect();
                _networkController->flushConnection();
                _networkController->resetColorReady();
                if (_networkController->getIsHost()){
                    _hostgame.reset();
                    _hostgame.setActive(true);
                    _status = HOST;
                } else{
                    _joingame.reset();
                    _joingame.setActive(true);
                    _status = CLIENT;
                }
            }
            break;
        case ColorSelectScene::Choice::NONE:
            // DO NOTHING
            break;
    }
}

void SSBApp::updateWaitingHostScene(float timestep){
    _waitinghost.update(timestep);
    _networkController->update(timestep);
    if (_network->getStatus() == NetEventController::Status::INGAME){
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _waitinghost.setActive(false);
            _colorselect.reset();
            _colorselect.setActive(true);
            _colorselect.setInitialPlayerCount(_network->getNumPlayers());
            _status = COLOR_SELECT;
        }
        return;
    }
    else if (_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID())
    {
        _networkController->setIsHost(false);
        _gameController.init(_assets, _networkController, _sound);
//        _gameController.setSpriteBatch(_batch);
        _network->markReady();
    } else if (_network->getStatus() == NetEventController::Status::NETERROR
     || _network->getNumPlayers() <= 1) {
        setTransition(true);
        if (_transition.getFadingOutDone()){
            _network->disconnect();
            _networkController->flushConnection();
            _waitinghost.setActive(false);
            _joingame.reset();
            _joingame.setActive(true);
            _status = CLIENT;
        }
        return;
    }
    switch (_waitinghost.getChoice())
    {
        case WaitingHostScene::Choice::CANCEL:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _waitinghost.setActive(false);
                _joingame.setActive(true);
                _network->disconnect();
                _status = CLIENT;
            }
            break;
        case WaitingHostScene::Choice::NONE:
            break;
    }
}

void SSBApp::updatePauseScene(float timestep){
    _pause.update(timestep);
    switch (_pause.getChoice())
    {
        case PauseScene::Choice::DISCONNECT:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _expectedPlayers = 0;
                _pause.reset();
                _pause.setActive(false);

                _startscreen.reset();
                _startscreen.setActive(true);
                _status = START;

                _gameController.setIsPaused(false);
            }
            break;
        case PauseScene::Choice::RESUME:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _pause.reset();
                _pause.setActive(false);
                _status = GAME;

                _gameController.setIsPaused(false);
                _gameController.setElementsActive(true);
            }
            break;
        case PauseScene::Choice::NONE:
            break;
    }
}

void SSBApp::updateDisconnectedScene(float timestep){
    _disconnectedscreen.update(timestep);
    switch (_disconnectedscreen.getChoice())
    {
        case DisconnectedScene::Choice::QUIT:
            setTransition(true);
            if (_transition.getFadingOutDone()){
                _expectedPlayers = 0;
                _disconnectedscreen.reset();
                _disconnectedscreen.setActive(false);
                
                _startscreen.reset();
                _startscreen.setActive(true);
                _status = START;
            }
            break;
        case DisconnectedScene::Choice::NONE:
            break;
    }
}

void SSBApp::resetScenes(){
    // Reset network
    _networkController->resetNetwork();
    _networkController->resetColorReady();
    _gameController.reset();
    _network->disconnect();
    _gameController.dispose();
    
    _startscreen.reset();
    _settingscreen.reset();
    _helpscreen.reset();
    _creditsscreen.reset();
    _mainmenu.reset();
    _hostgame.reset();
    _joingame.reset();
    _colorselect.reset();
    _levelSelect.reset();
    _waitinghost.reset();
    _pause.reset();
    _disconnectedscreen.reset();
    _expectedPlayers = 0;
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void SSBApp::draw()
{
    switch (_status)
    {
    case LOAD:
        _loading.render();
        break;
    case START:
        _startscreen.render();
        break;
    case SETTING:
        _settingscreen.render();
        break;
    case CREDITS:
        _creditsscreen.render();
        break;
    case HELP:
        _helpscreen.render();
        break;
    case MENU:
        _mainmenu.render();
        break;
    case HOST:
        _hostgame.render();
        break;
    case CLIENT:
        _joingame.render();
        break;
    case WAITING_HOST:
        _waitinghost.render();
        break;
    case COLOR_SELECT:
        _colorselect.render();
        break;
    case LEVEL_SELECT:
        _levelSelect.render();
        break;
    case GAME:
        _gameController.render();
        break;
    case LEVEL_EDITOR:
        _levelEditorController.render();
        break;
    case VICTORY:
        _victory.render();
        break;
    case PAUSED:
        _gameController.render();
        _pause.render();
        break;
    case DISCONNECTED:
        _disconnectedscreen.render();
        break;
    default:
        break;
    }
    
    if (_doTransition){
        _transition.render();
    }
}

void SSBApp::populateMaps() {
    ArtAssetMapHelper::populateConstantsMaps();
}
