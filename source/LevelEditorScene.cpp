//
//  LevelEditorScene.cpp
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/28/25.
//

#include "LevelEditorScene.h"
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
#pragma mark Scene Constants

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
#define DEFAULT_HEIGHT (SCENE_HEIGHT / BOX2D_UNIT)

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
LevelEditorScene::LevelEditorScene() : Scene2() {}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelEditorScene::dispose() {

};

/**
 * Initializes the scene contents
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool LevelEditorScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<Camera> camera) {
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
#ifdef CU_TOUCH_SCREEN 
    _offset = Vec2((_size.width - SCENE_WIDTH) * 2.0f, 448);
#else
    _offset = Vec2((_size.width * SCENE_WIDTH / 1024 - SCENE_WIDTH) / 2, (_size.height * SCENE_HEIGHT / 576 - SCENE_HEIGHT) / 2);
#endif
    _active = true;
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void LevelEditorScene::reset() {
    _camera->setPosition(_cameraInitialPos);
    _camera->update();
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void LevelEditorScene::preUpdate(float dt) {
    _camera->update();
}


#pragma mark -
#pragma mark Helpers
/**
 * Set whether the elements of this scene are visible or not
 */
void LevelEditorScene::setVisible(bool value) {

}

/**
 * Resets the camera position to the initial state.
 */
void LevelEditorScene::resetCameraPos() {
    _camera->setPosition(_cameraInitialPos);
}

/**
 * Converts from screen to Box2D coordinates.
 *
 * @return the Box2D position
 *
 * @param screenPos    The screen position
 */
Vec2 LevelEditorScene::convertScreenToBox2d(const Vec2& screenPos)
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
Vec2 LevelEditorScene::convertScreenToBox2d(const Vec2& screenPos, float systemScale)
{
    Vec2 adjusted = screenPos * systemScale - _offset;

    // Adjust for camera position
    // systemScale used to be involved here (2 * systemScale instead of 2), but now I'm not so sure.
    // I think it's different now because the MoveScene (1024 x 576) and BuildScene (2048 x 1152) don't both get drawn with different dimensions on top of each other in the level editor.
    // Either way, it doesn't matter much, since this code is for desktop only.
    // And for desktop, this works.
    Vec2 worldPos = (adjusted + (_camera->getPosition() * 2 - _cameraInitialPos));

    float xBox2D = worldPos.x / _scale;
    float yBox2D = worldPos.y / _scale;

    // Converts to the specific grid position
    int xGrid = xBox2D / 2;
    int yGrid = yBox2D / 2;

    return Vec2(xGrid, yGrid);
}
void LevelEditorScene::linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj,
    const std::shared_ptr<scene2::SceneNode>& node, float scale, const std::shared_ptr<scene2::SceneNode>* _worldnode) {

    node->setPosition(obj->getPosition() * scale);
    (*_worldnode)->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=, this](physics2::Obstacle* obs) {
            float leftover = Application::get()->getFixedRemainder() / 1000000.f;
            Vec2 pos = obs->getPosition() + leftover * obs->getLinearVelocity();
            float angle = obs->getAngle() + leftover * obs->getAngularVelocity();
            weak->setPosition(pos * scale);
            weak->setAngle(angle);
            });
    }
}

