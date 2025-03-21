//
//  ObjectController.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 3/13/25.
//

#include "SSBGameController.h"
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


float SPIKE_POS[] = {5.5f, 1.5f};

//TODO: should be handle entworking here later?

/** Create ObjectController that handles the creation of all objects, including platform, spike, windobstacle, moving platform...**/

ObjectController::ObjectController(const std::shared_ptr<AssetManager>& assets,
                                   const std::shared_ptr<cugl::physics2::distrib::NetWorld>& world,
                                   float scale,
                                   const std::shared_ptr<scene2::SceneNode> world_node,
                                   const std::shared_ptr<scene2::SceneNode> debug_node,
                                   std::vector<std::shared_ptr<Object>>* gameObjects) {
    // Constructor body (initialize additional members if needed)
    _assets = assets;
    _world = world;
    _scale = scale;
    _worldnode = world_node;
    _debugnode = debug_node;
    _gameObjects = gameObjects;
};
std::shared_ptr<Object> ObjectController::createPlatform(std::shared_ptr<Platform> plat) {
    std::shared_ptr<Texture> image;
    if (plat->getJsonType() == "tile") {
        image = _assets->get<Texture>(TILE_TEXTURE);
    } else if (plat->getJsonType() == "platform") {
        image = _assets->get<Texture>(PLATFORM_TILE_TEXTURE);
    } else {
        image = _assets->get<Texture>(LOG_TEXTURE);
    }

    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Poly2 poly(Rect(plat->getPosition().x, plat->getPosition().y, plat->getSize().width - blendingOffset, plat->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Set the physics attributes
    plat->getObstacle()->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    plat->getObstacle()->setDensity(BASIC_DENSITY);
    plat->getObstacle()->setFriction(BASIC_FRICTION);
    plat->getObstacle()->setRestitution(BASIC_RESTITUTION);
    plat->getObstacle()->setDebugColor(DEBUG_COLOR);
    plat->getObstacle()->setName("platform");

    poly *= _scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, poly);
    
    addObstacle(plat->getObstacle(), sprite, 1); // All walls share the same texture
    
    
    _gameObjects->push_back(plat);

    return plat;
}
/**
 * Creates a new platform.
 *
 * @return the platform being created
 *
 * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
 * @param size The dimensions (width, height) of the platform.
 */
std::shared_ptr<Object> ObjectController::createPlatform(Vec2 pos, Size size, string jsonType) {

    std::shared_ptr<Platform> plat = Platform::alloc(pos, size, jsonType);

    return createPlatform(plat);
}
/**
 * Creates a moving platform.
 *
 * @return the moving platform
 *
 * @param pos The bottom left position of the platform starting position
 * @param size The dimensions of the platform.
 * @param end The bottom left position of the platform's destination.
 * @param speed The speed at which the platform moves.
 */
std::shared_ptr<Object> ObjectController::createMovingPlatform(Vec2 pos, Size size, Vec2 end, float speed) {
    CULog("creating moving platform");
    std::shared_ptr<Texture> image = _assets->get<Texture>(GLIDING_LOG_TEXTURE);

    std::shared_ptr<Platform> plat = Platform::allocMoving(pos, size, pos, end, speed);

    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Poly2 poly(Rect(plat->getPosition().x, plat->getPosition().y, plat->getSize().width - blendingOffset, plat->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    plat->getObstacle()->setDensity(BASIC_DENSITY);
    plat->getObstacle()->setFriction(BASIC_FRICTION);
    plat->getObstacle()->setRestitution(BASIC_RESTITUTION);
    plat->getObstacle()->setDebugColor(DEBUG_COLOR);
    plat->getObstacle()->setName("movingPlatform");

    poly *= _scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, poly);

    addObstacle(plat->getObstacle(), sprite, 1);
    _gameObjects->push_back(plat);

    return plat;
}
/**
 * Creates a new spike.
 * @param pos The position of the bottom left corner of the spike in Box2D coordinates.
 * @param size The dimensions (width, height) of the spike.
 */
std::shared_ptr<Object> ObjectController::createSpike(Vec2 pos, Size size, float scale, float angle, string jsonType)
{
    std::shared_ptr<Spike> spk = Spike::alloc(pos, size, scale, angle, jsonType);
    return createSpike(spk);
}

std::shared_ptr<Object> ObjectController::createSpike(std::shared_ptr<Spike> spk)
{
    std::shared_ptr<Texture> image = _assets->get<Texture>(SPIKE_TILE_TEXTURE);

    // Set the physics attributes
    spk->getObstacle()->setBodyType(b2_staticBody);
    spk->getObstacle()->setDensity(BASIC_DENSITY);
    spk->getObstacle()->setFriction(BASIC_FRICTION);
    spk->getObstacle()->setRestitution(BASIC_RESTITUTION);
    spk->getObstacle()->setDebugColor(DEBUG_COLOR);
    spk->getObstacle()->setName("spike");

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    spk->setSceneNode(sprite, spk->getAngle());
    addObstacle(spk->getObstacle(), sprite);
    _gameObjects->push_back(spk);
    return spk;
}
/**
 * Creates a new windobstacle
 *
 * @return the wind obstacle
 *
 * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
 * @param size The dimensions (width, height) of the platform.
 */
std::shared_ptr<Object> ObjectController::createWindObstacle(Vec2 pos, Size size, Vec2 gust, string jsonType)
{
    std::shared_ptr<Texture> image = _assets->get<Texture>(WIND_TEXTURE);
    std::shared_ptr<WindObstacle> wind = WindObstacle::alloc(pos, size, gust);

    // Allow movement of obstacle
    wind->getObstacle()->setBodyType(b2_dynamicBody);

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);

    wind->setTrajectory(gust);
    wind->setPosition(pos);

    addObstacle(wind->getObstacle(), sprite, 1); // All walls share the same texture

    _gameObjects->push_back(wind);

    return wind;
}

std::shared_ptr<Object> ObjectController::createWindObstacle(std::shared_ptr<WindObstacle> wind)
{
    return createWindObstacle(wind->getPosition(), wind->getSize(), wind->gustDir(), wind->getJsonType());
}

std::shared_ptr<Object> ObjectController::createTreasure(Vec2 pos, Size size, string jsonType){
    std::shared_ptr<Texture> image;
    std::shared_ptr<scene2::PolygonNode> sprite;
    Vec2 treasurePos = pos;
    image = _assets->get<Texture>("treasure");
    _treasure = Treasure::alloc(treasurePos,image->getSize()/_scale,_scale);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    _treasure->setSceneNode(sprite);
    addObstacle(_treasure->getObstacle(),sprite);
    _treasure->getObstacle()->setName("treasure");
    _treasure->getObstacle()->setDebugColor(Color4::YELLOW);

    _treasure->setPosition(pos);
    _gameObjects->push_back(_treasure);
    return _treasure;
}

std::shared_ptr<Object> ObjectController::createTreasure(std::shared_ptr<Treasure> _treasure) {
    return createTreasure(_treasure->getPosition(), _treasure->getSize(), _treasure->getJsonType());
}

std::shared_ptr<physics2::BoxObstacle> ObjectController::createGoalDoor(Vec2 goalPos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>(GOAL_TEXTURE);
    std::shared_ptr<scene2::PolygonNode> sprite;

    Size goalSize(image->getSize().width / _scale, image->getSize().height / _scale);
    
    std::shared_ptr<physics2::BoxObstacle> goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);

    goalDoor->setBodyType(b2_staticBody);
    goalDoor->setDensity(0.0f);
    goalDoor->setFriction(0.0f);
    goalDoor->setRestitution(0.0f);
    goalDoor->setSensor(true);

    sprite = scene2::PolygonNode::allocWithTexture(image);
    sprite->setColor(Color4(1, 255, 0));  // Greenish color
    goalDoor->setDebugColor(DEBUG_COLOR);

    addObstacle(goalDoor, sprite);

    return goalDoor;
}
/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is to make a subclass of a physics object, like we did
 * with dude.  The other is to use callback functions to loosely couple
 * the two.  This function is an example of the latter.
 *
 * @param obj             The physics object to add
 * @param node            The scene graph node to attach it to
 * @param zOrder          The drawing order
 * @param useObjPosition  Whether to update the node's position to be at the object's position
 */
void ObjectController::addObstacle(const std::shared_ptr<physics2::Obstacle> &obj,
                            const std::shared_ptr<scene2::SceneNode> &node,
                            bool useObjPosition)
{
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);

    // Position the scene graph node (enough for static objects)
    if (useObjPosition)
    {
        node->setPosition(obj->getPosition() * _scale);
    }
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() != b2_staticBody)
    {
        scene2::SceneNode *weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=, this](physics2::Obstacle *obs) {
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle()); });
    }
}
void ObjectController::processLevelObject(std::shared_ptr<Object> obj) {
    std::string key = obj->getJsonKey();

    if (key == "platforms") {
        createPlatform(std::dynamic_pointer_cast<Platform>(obj));
    }
    else if (key == "spikes") {
        createSpike(std::dynamic_pointer_cast<Spike>(obj));
    }
    else if (key == "treasures") {
        _treasure = (std::dynamic_pointer_cast<Treasure> (_networkController->createTreasureNetworked(obj->getPosition(), obj->getSize(),
                                                            _scale,
                                                            false)));
    }
    else if (key == "windObstacles") {
        createWindObstacle(std::dynamic_pointer_cast<WindObstacle>(obj));
    }
}

/**
 * Create the growing wall if not created. Otherwise, increase its width
 *
 * @param timestep  The elapsed time since the last frame.
 */

//void ObjectController::updateGrowingWall(float timestep)
//{
//    // Increase the width
//    _growingWallWidth += _growingWallGrowthRate * timestep;
//
//    // Remove the old wall if it exists
//    if (_growingWall && _world->getObstacles().count(_growingWall) > 0)
//    {
//        _world->removeObstacle(_growingWall);
//        _worldnode->removeChild(_growingWallNode);
//    }
//
//    // Create a new polygon for the wall
//    Poly2 wallPoly;
//    wallPoly.vertices.push_back(Vec2(0, DEFAULT_HEIGHT*.80));
//    wallPoly.vertices.push_back(Vec2(0, 0));
//    wallPoly.vertices.push_back(Vec2(_growingWallWidth, 0));
//    wallPoly.vertices.push_back(Vec2(_growingWallWidth, DEFAULT_HEIGHT*.80));
//
//    EarclipTriangulator triangulator;
//    triangulator.set(wallPoly.vertices);
//    triangulator.calculate();
//    wallPoly.setIndices(triangulator.getTriangulation());
//    triangulator.clear();
//
//    // Create the collision box
//    _growingWall = physics2::PolygonObstacle::allocWithAnchor(wallPoly, Vec2::ANCHOR_BOTTOM_LEFT);
//    _growingWall->setName("growingWall");
//    _growingWall->setBodyType(b2_staticBody);
//    _growingWall->setDensity(BASIC_DENSITY);
//    _growingWall->setFriction(BASIC_FRICTION);
//    _growingWall->setRestitution(BASIC_RESTITUTION);
//    _growingWall->setDebugColor(Color4::RED);
//
//    wallPoly *= _scale;
//    _growingWallNode = scene2::PolygonNode::allocWithPoly(wallPoly);
//    _growingWallNode->setColor(Color4::RED);
//    _growingWallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//
//    addObstacle(_growingWall, _growingWallNode, true);
//}




