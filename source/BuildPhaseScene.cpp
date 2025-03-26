//
//  BuildPhaseScene.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/17/25.
//

#include "BuildPhaseScene.h"
#include "Constants.h"
#include "Platform.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "SSBDudeModel.h"
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
#pragma mark Scene Constants

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0 / 16.0

/** The number pixels in a Box2D unit */
#define BOX2D_UNIT 64.0f

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH (SCENE_WIDTH / BOX2D_UNIT) * 2
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT (SCENE_HEIGHT / BOX2D_UNIT)

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
BuildPhaseScene::BuildPhaseScene() : Scene2() {}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void BuildPhaseScene::dispose() {

};

/**
 * Initializes the scene contents
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool BuildPhaseScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<Camera> camera) {
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::initWithHint(Size(SCENE_WIDTH, SCENE_HEIGHT)))
    {
        return false;
    }

    _assets = assets;
    _camera = camera;
    _cameraInitialPos = getCamera()->getPosition();

    _scale = _size.width == SCENE_WIDTH ? _size.width / DEFAULT_WIDTH : _size.height / DEFAULT_HEIGHT;
    _offset = Vec2((_size.width * SCENE_WIDTH / 1024 - SCENE_WIDTH) * 0.8f, (_size.height * SCENE_HEIGHT / 576 - SCENE_HEIGHT) * 0.8f);

    return true;
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void BuildPhaseScene::reset() {
    _camera->setPosition(_cameraInitialPos);
    _camera->update();
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void BuildPhaseScene::preUpdate(float dt) {
    _camera->update();
}


#pragma mark -
#pragma mark Helpers
/**
 * Set whether the elements of this scene are visible or not
 */
void BuildPhaseScene::setVisible(bool value) {
    
}

/** Sets whether or not we are in level editor mode.
    * By default, we are not.
    */
void BuildPhaseScene::setLevelEditor(bool value) {
    _isLevelEditor = value;
}
/**
 * Resets the camera position to the initial state.
 */
void BuildPhaseScene::resetCameraPos() {
    _camera->setPosition(_cameraInitialPos);
}

/**
 * Converts from screen to Box2D coordinates.
 *
 * @return the Box2D position
 *
 * @param screenPos    The screen position
 */
Vec2 BuildPhaseScene::convertScreenToBox2d(const Vec2 &screenPos)
{
    Vec2 adjusted = screenPos - _offset;

    // Adjust for camera position
    Vec2 worldPos = adjusted + (_camera->getPosition() - _cameraInitialPos);

    float xBox2D = worldPos.x / _scale;
    float yBox2D = worldPos.y / _scale;

    // Converts to the specific grid position
    int xGrid = xBox2D;
    int yGrid = yBox2D;

    return Vec2(xGrid, yGrid);
}

/**
 * Converts from screen to Box2D coordinates.
 *
 * @return the Box2D position
 *
 * @param screenPos    The screen position
 * @param systemScale The scale to differentiate mobile from desktop
 */
Vec2 BuildPhaseScene::convertScreenToBox2d(const Vec2& screenPos, float systemScale)
{
    Vec2 adjusted = screenPos * systemScale - _offset;

    // Adjust for camera position
    Vec2 worldPos = (adjusted + (_camera->getPosition() * systemScale * systemScale - _cameraInitialPos));

    float xBox2D = worldPos.x / _scale;
    float yBox2D = worldPos.y / _scale;

    // Converts to the specific grid position
    int xGrid = xBox2D / systemScale;
    int yGrid = yBox2D / systemScale;

    return Vec2(xGrid, yGrid);
}

