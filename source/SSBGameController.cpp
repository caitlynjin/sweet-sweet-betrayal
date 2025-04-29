//
//  SSBGameController.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/16/25.
//

#include "SSBGameController.h"
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
#pragma mark Level Geography

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 2048
#define SCENE_HEIGHT 1152

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0 / 16.0

/** The number pixels in a Box2D unit */
#define BOX2D_UNIT 64.0f

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH (SCENE_WIDTH / BOX2D_UNIT) * 2
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT (SCENE_HEIGHT / BOX2D_UNIT) * 1

#define FIXED_TIMESTEP_S 0.02f

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
SSBGameController::SSBGameController() : Scene2(),
    _world(nullptr)
{
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool SSBGameController::init(const std::shared_ptr<AssetManager> &assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound)
{
    return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), networkController, sound);
}


/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 * @param gravity   The gravitational force on this Box2d world
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool SSBGameController::init(const std::shared_ptr<AssetManager> &assets,
                     const Rect &rect, const Vec2 &gravity, const std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound)
{
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }

    _assets = assets;
    _networkController = networkController;
    _network = networkController->getNetwork();
    _sound = sound;

    // Networked physics world
    _world = physics2::distrib::NetWorld::alloc(rect,gravity);
    _world->activateCollisionCallbacks(true);
    _world->beforeSolve = [this](b2Contact *contact, const b2Manifold *oldManifold) {
        _movePhaseController->beforeSolve(contact, oldManifold);
    };
    _world->onBeginContact = [this](b2Contact *contact)
    {
        _movePhaseController->beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact *contact)
    {
        _movePhaseController->endContact(contact);
    };
    _world->update(FIXED_TIMESTEP_S);

    // Start in building mode
    _buildingMode = true;

    // Start up the input handler
    _input = std::make_shared<PlatformInput>();
    _input->init(getBounds());

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = _size.width == SCENE_WIDTH ? _size.width / rect.size.width : _size.height / rect.size.height;
    _scale *= getSystemScale();
    Vec2 offset = Vec2((_size.width - SCENE_WIDTH) / 2.0f, (_size.height - SCENE_HEIGHT) / 2.0f);
    _offset = offset;

    // Initialize background
    _backgroundScene.init();
    _background = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(BACKGROUND_TEXTURE));
    _background->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _background->setPosition(Vec2(0,0));
    _background->setScale(2.1f);
    _backgroundScene.addChild(_background);

    _gridManager = GridManager::alloc(false, DEFAULT_WIDTH, _scale, offset, assets);

    _movePhaseController = std::make_shared<MovePhaseController>();
    _buildPhaseController = std::make_shared<BuildPhaseController>();

    // Initialize movement phase controller
    _movePhaseController->init(assets, _world, _input, _gridManager, _networkController, _sound);
    _camera = _movePhaseController->getCamera();
    _objectController = _movePhaseController->getObjectController();
    
    _objects = _movePhaseController->getObjects();

    // Initialize build phase controller
    _buildPhaseController->init(assets, _input, _gridManager, _objectController, _networkController, _camera, _movePhaseController->getLocalPlayer());


    _active = true;
    Application::get()->setClearColor(Color4f::CORNFLOWER);

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void SSBGameController::dispose()
{
    _world = nullptr;
    _gridManager->getGridNode() = nullptr;

    _input->dispose();
    _backgroundScene.dispose();
    _buildPhaseController->dispose();
    _movePhaseController->dispose();
    Scene2::dispose();
}

#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void SSBGameController::reset()
{
    // Clear the world
    _world->clear();
    
    // Reset all controllers
    _networkController->reset();
    _buildPhaseController->reset();
    _movePhaseController->reset();
        
    _hasVictory = false;
}

#pragma mark -
#pragma mark Physics Handling
/**
 * The method called to update the game mode.
 *
 * This is the nondeterministic version of a physics simulation. It is
 * provided for comparison purposes only.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void SSBGameController::update(float timestep)
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
void SSBGameController::preUpdate(float dt)
{
    // Check for reset
    if (_networkController->getResetLevel()){
        reset();
    }
    
    // Overall game logic
    _networkController->preUpdate(dt);
    _input->update(dt);
    

//    if (_networkController->getIsHost() && _networkController->getTreasure() != nullptr){
//        _networkController->getTreasure()->updateAnimation(dt);
//    }

    // Update logic for Build Phase
    if (_buildingMode){
        _buildPhaseController->preUpdate(dt);
        // Check if can switch to movement phase
        if (_networkController->canSwitchToMove()){
            // Exit build mode and switch to movement phase
            setBuildingMode(!_buildingMode);
        }
        
    }
    
    // Update logic for Movement Phase
    if (!_buildingMode){
        _movePhaseController->preUpdate(dt);
        // Check if can switch to build phase, therefore starting a new round
        if (_networkController->canSwitchToBuild()){
//            //TODO: Segment into switchToBuild()
//            if (_scoreCountdown == -1){
//                _scoreCountdown = SCOREBOARD_COUNT;
//                _movePhaseController->scoreboardActive(true);
//            }
//            if (_scoreCountdown == 0){
//                _movePhaseController->scoreboardActive(false);
//                _movePhaseController->resetRound();
//                setBuildingMode(!_buildingMode);
//                _networkController->resetRound();
////                _movePhaseController->resetRound();
//                _scoreCountdown = -1;
//                // Check for win condition
//                if (_networkController->checkWinCondition()){
//                    _hasVictory = true;
//                }
            if (_beforeScoreBoard == 0) {
                //TODO: Segment into switchToBuild()
                if (_scoreCountdown == -1){
                    _scoreCountdown = SCOREBOARD_COUNT;
                    _movePhaseController->scoreboardActive(true);
                }
                if (_scoreCountdown == 0){
                    _movePhaseController->scoreboardActive(false);
                    _movePhaseController->resetRound();
                    setBuildingMode(!_buildingMode);
                    _networkController->resetRound();
    //                _movePhaseController->resetRound();
                    _scoreCountdown = -1;
                    // Check for win condition
                    if (_networkController->checkWinCondition()){
                        _hasVictory = true;
                    }
                    _beforeScoreBoard = 15;
                }
            } else {
                _beforeScoreBoard --;
            }
            
        }
    }
    
    // Update any timers, if active
    // TODO: segment into updateTimers method if more timers needed in future
    if (_scoreCountdown > 0){
        _scoreCountdown -= 1;
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
 *
 * The time passed to this method is NOT the same as the one passed to
 * {@link #preUpdate}. It will always be exactly the same value.
 *
 * @param step  The number of fixed seconds for this step
 */
void SSBGameController::fixedUpdate(float step)
{
    // Turn the physics engine crank.
    _world->update(FIXED_TIMESTEP_S);

    // Update all controllers
    _networkController->fixedUpdate(step);
    
    
    // Update all game objects
//    for (auto it = _objects.begin(); it != _objects.end(); ++it) {
//        (*it)->update(step);
//    }
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
 * last call to {@link #fixedUpdate}. That is because this method is used
 * to interpolate object position for animation.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void SSBGameController::postUpdate(float remain)
{
    // Since items may be deleted, garbage collect
    _world->garbageCollect();

    // Update all controllers
    _networkController->fixedUpdate(remain);
    
    if (!_buildingMode){
        _movePhaseController->postUpdate(remain);
    }
    

    // Reset the game if we win or lose.
    // TODO: handle within u
    int countdown = _movePhaseController->getCountdown();
    if (countdown > 0)
    {
        _movePhaseController->setCountdown(countdown - 1);
    }
    else if (countdown == 0)
    {
        reset();
    }
}

void SSBGameController::setSpriteBatch(const shared_ptr<SpriteBatch> &batch) {
    _backgroundScene.setSpriteBatch(batch);
    Scene2::setSpriteBatch(batch);
    _movePhaseController->setSpriteBatch(batch);
    _buildPhaseController->setSpriteBatch(batch);
}

void SSBGameController::render() {
    _backgroundScene.render();
    Scene2::render();
    _movePhaseController->render();
    _buildPhaseController->render();
}

/**
 * Sets whether mode is in building or play mode.
 *
 * @param value whether the level is in building mode.
 */
void SSBGameController::setBuildingMode(bool value) {
    _buildingMode = value;
    
    _buildPhaseController->processModeChange(value);

    _gridManager->getGridNode()->setVisible(value);
    _camera->setPosition(_initialCameraPos);

    _movePhaseController->processModeChange(value);
    
    std::vector<std::shared_ptr<PlayerModel>> players = _networkController->getPlayerList();
    for (auto player : players){
        player->setImmobile(value);
    }
}

#pragma mark -
#pragma mark Helpers

