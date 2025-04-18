//
//  ObjectController.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 3/13/25.
//

#include "SSBGameController.h"
#include "Constants.h"
#include "Platform.h"
#include "Tile.h"
#include "Spike.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "PlayerModel.h"
#include "WindObstacle.h"
#include "LevelModel.h"
#include "ObjectController.h"
#include "ArtObject.h"

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
/**
Creates a 1 by 1 tile
*/
std::shared_ptr<Object> ObjectController::createTile(Vec2 pos, Size size, string jsonType, float scale) {
    CULog("MADE PLATFORM");
    std::shared_ptr<Tile> tile = Tile::alloc(pos, size, jsonType, scale);

    std::shared_ptr<Texture> image;
    image = _assets->get<Texture>(jsonTypeToAsset[jsonType]);

    float blendingOffset = 0.01f;

    Poly2 poly(Rect(tile->getPositionInit().x, tile->getPositionInit().y, tile->getSize().width - blendingOffset, tile->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Set the physics attributes
    tile->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    tile->setDensity(BASIC_DENSITY);
    tile->setFriction(BASIC_FRICTION);
    tile->setRestitution(BASIC_RESTITUTION);
    tile->setDebugColor(DEBUG_COLOR);
    tile->setName("tile");

    poly *= _scale;
    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    tile->setSceneNode(sprite);

    addObstacle(tile, sprite, 1); // All walls share the same texture


    _gameObjects->push_back(tile);

    return tile;
}
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

    Poly2 poly(Rect(plat->getPositionInit().x, plat->getPositionInit().y, plat->getSize().width - blendingOffset, plat->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Set the physics attributes
    plat->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    plat->setDensity(BASIC_DENSITY);
    plat->setFriction(BASIC_FRICTION);
    plat->setRestitution(BASIC_RESTITUTION);
    plat->setDebugColor(DEBUG_COLOR);
    plat->setName("platform");

    poly *= _scale;
    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    sprite->setAnchor(sprite->getAnchor().x, sprite->getAnchor().y + plat->getSize().height*0.25f );
    
    plat->setSceneNode(sprite);

    addObstacle(plat, sprite, 1); // All walls share the same texture
    
    
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
    CULog("MADE PLATFORM");
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

    Poly2 poly(Rect(plat->getPositionInit().x, plat->getPositionInit().y, plat->getSize().width - blendingOffset, plat->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    plat->setDensity(BASIC_DENSITY);
    plat->setFriction(BASIC_FRICTION);
    plat->setRestitution(BASIC_RESTITUTION);
    plat->setDebugColor(DEBUG_COLOR);
    plat->setName("movingPlatform");

    poly *= _scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, poly);
    plat->setSceneNode(sprite);

    addObstacle(plat, sprite, 1);
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
    spk->setBodyType(b2_staticBody);
    spk->setDensity(BASIC_DENSITY);
    spk->setFriction(BASIC_FRICTION);
    spk->setRestitution(BASIC_RESTITUTION);
    spk->setDebugColor(DEBUG_COLOR);
    spk->setName("spike");

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    spk->setSceneNode(sprite, spk->getAngle());
    addObstacle(spk, sprite);
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
std::shared_ptr<Object> ObjectController::createWindObstacle(Vec2 pos, Size size, const Vec2 windDirection, const Vec2 windStrength, string jsonType)
{
    std::shared_ptr<Texture> fan = _assets->get<Texture>(FAN_TEXTURE);
    std::shared_ptr<Texture> gust = _assets->get<Texture>(GUST_TEXTURE);
    std::shared_ptr<scene2::SpriteNode> gustSprite = scene2::SpriteNode::allocWithSheet(gust, 1, 1);
    std::shared_ptr<scene2::PolygonNode> fanSprite = scene2::PolygonNode::allocWithTexture(fan);

    std::shared_ptr<WindObstacle> wind = WindObstacle::alloc(pos, size, windDirection, windStrength);

    // Allow movement of obstacle
    wind->setBodyType(b2_dynamicBody);
    wind->setPositionInit(pos);

    addObstacle(wind, fanSprite, 1);
    wind->setSceneNode(fanSprite);
    //Set the texture of the gust
    wind->setGustSprite(gustSprite);

    _gameObjects->push_back(wind);

    return wind;
}

std::shared_ptr<Object> ObjectController::createWindObstacle(std::shared_ptr<WindObstacle> wind)
{
    return createWindObstacle(wind->getPositionInit(), wind->getSize(),wind->getWindDirection(), wind->getWindForce(), wind->getJsonType());
}

std::shared_ptr<Object> ObjectController::createTreasure(Vec2 pos, Size size, string jsonType){
    std::shared_ptr<Texture> image;
    std::shared_ptr<scene2::PolygonNode> sprite;
    Vec2 treasurePos = pos;
    image = _assets->get<Texture>("treasure");
    _treasure = Treasure::alloc(treasurePos,image->getSize()/_scale,_scale);
    
    
    auto animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("treasure-sheet"), 8, 8, 64);
    _treasure->setAnimation(animNode);
    
    
//    _treasure->setSceneNode(sprite);
    addObstacle(_treasure,_treasure->getSceneNode());
    _treasure->setName("treasure");
    _treasure->setDebugColor(Color4::YELLOW);

    _treasure->setPosition(pos);
    _gameObjects->push_back(_treasure);
    _networkController->setTreasure(_treasure);
    return _treasure;
}

std::shared_ptr<Object> ObjectController::createTreasure(std::shared_ptr<Treasure> _treasure) {
    return createTreasure(_treasure->getPositionInit(), _treasure->getSize(), _treasure->getJsonType());
}

std::shared_ptr<Object> ObjectController::createArtObject(Vec2 pos, Size size, float scale, float angle, std::string jsonType) {
    return createArtObject(ArtObject::alloc(pos, size, scale, angle, jsonType));
}

std::shared_ptr<Object> ObjectController::createArtObject(std::shared_ptr<ArtObject> art) {
    std::shared_ptr<Texture> image;

    image = _assets->get<Texture>(jsonTypeToAsset[art->getJsonType()]);
    if (image == nullptr) {
        image = _assets->get<Texture>("earth");
    }
    // Removes the black lines that display from wrapping
    float blendingOffset = 0.01f;

    Vec2 offset = Vec2(0, 0);
    if (std::find(xOffsetArtObjects.begin(), xOffsetArtObjects.end(), art->getJsonType()) != xOffsetArtObjects.end()) {
        offset += Vec2(32, 0);
    }
    if (std::find(yOffsetArtObjects.begin(), yOffsetArtObjects.end(), art->getJsonType()) != yOffsetArtObjects.end()) {
        offset += Vec2(0, 32);
    }

    Poly2 poly(Rect(art->getPositionInit().x, art->getPositionInit().y, art->getSize().width - blendingOffset, art->getSize().height - blendingOffset));

    // Call this on a polygon to get a solid shape
    EarclipTriangulator triangulator;
    triangulator.set(poly.vertices);
    triangulator.calculate();
    poly.setIndices(triangulator.getTriangulation());
    triangulator.clear();
    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    art->setSceneNode(sprite);
    if (jsonTypeToLayer.find(art->getJsonType()) != jsonTypeToLayer.end()) {
        // jsonType IS in the map
        art->setLayer(jsonTypeToLayer[art->getJsonType()]);
    }
    CULog("layer is %d", art->getLayer());
    /* ArtObjects are still objects, and thus still have BoxObstacles.
     * They also need this to be rendered properly in the physics world.
     * Otherwise we'd have to have separate logic using an addChild method 
     * to add the sprite nodes just for art objects to all the game scenes.
     * So we keep the physics body for consistency, and just disable it for art objects.
     */
    art->setBodyType(b2_staticBody);
    art->setDensity(BASIC_DENSITY);
    art->setFriction(BASIC_FRICTION);
    art->setRestitution(BASIC_RESTITUTION);
    art->setDebugColor(DEBUG_COLOR);
    art->setName("artObject");
    // Disable ArtObject collision physics
    art->setSensor(true);
    addObstacle(art, sprite);

    _gameObjects->push_back(art);

    return art;
}

std::shared_ptr<physics2::BoxObstacle> ObjectController::createGoalDoor(Vec2 goalPos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>(GOAL_TEXTURE);
    std::shared_ptr<scene2::PolygonNode> sprite;

    _goalPos = goalPos;

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
void ObjectController::processLevelObject(std::shared_ptr<Object> obj, bool levelEditing) {
    std::string key = obj->getJsonKey();

    if (key == "platforms") {
        createPlatform(std::dynamic_pointer_cast<Platform>(obj));
    }
    else if (key == "spikes") {
        createSpike(std::dynamic_pointer_cast<Spike>(obj));
    }
    else if (key == "treasures") {
        // Required because it crashes if you try to set up a networked treasure during build mode
        if (!levelEditing) {
            //TODO: Change so that NetworkController has a list of treasure positions, then MovePhaseScene will init the treasure based on these positions
            _networkController->addTreasureSpawn(obj->getPositionInit());
        }
        else {
            createTreasure(std::dynamic_pointer_cast<Treasure>(obj));
        }
        
    }
    else if (key == "windObstacles") {
        createWindObstacle(std::dynamic_pointer_cast<WindObstacle>(obj));
    }
    else if (key == "artObjects") {
        createArtObject(std::dynamic_pointer_cast<ArtObject>(obj));
    }
    else if (key == "tiles") {
        createTile(obj->getPositionInit(), obj->getSize(), obj->getJsonType(), _scale);
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




