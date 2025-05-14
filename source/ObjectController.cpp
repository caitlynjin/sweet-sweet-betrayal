//
//  ObjectController.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 3/13/25.
//

#include "SSBGameController.h"
#include "Constants.h"
#include "Platform.h"
#include "Mushroom.h"
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
#include "GoalDoor.h"

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

void ObjectController::reset() {
    _gameObjects->clear();
}
/**
Creates a 1 by 1 tile
*/
std::shared_ptr<Object> ObjectController::createTile(Vec2 pos, Size size, string jsonType, float scale) {
    std::shared_ptr<Tile> tile = Tile::alloc(pos, size, jsonType, scale);
    return createTile(tile);
}

std::shared_ptr<Object> ObjectController::createTile(std::shared_ptr<Tile> tile) {
    std::shared_ptr<Texture> image;
    image = _assets->get<Texture>(jsonTypeToAsset[tile->getJsonType()]);

    // Set the physics attributes
    tile->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    tile->setDensity(BASIC_DENSITY);
    tile->setFriction(BASIC_FRICTION);
    tile->setRestitution(BASIC_RESTITUTION);
    tile->setDebugColor(DEBUG_COLOR);
    tile->setName("tile");

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

    // Set the physics attributes
    plat->setBodyType(b2_dynamicBody);   // Must be dynamic for position to update
    plat->setDensity(BASIC_DENSITY);
    plat->setFriction(BASIC_FRICTION);
    plat->setRestitution(BASIC_RESTITUTION);
    plat->setDebugColor(DEBUG_COLOR);
    plat->setName("platform");

    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    
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

    std::shared_ptr<Platform> plat = Platform::allocMoving(pos, size, pos, end, speed);
    return createMovingPlatform(plat);
}

std::shared_ptr<Object> ObjectController::createMovingPlatform(shared_ptr<Platform> plat){
    std::shared_ptr<Texture> image = _assets->get<Texture>(GLIDING_LOG_TEXTURE);
    std::shared_ptr<scene2::SpriteNode> glidingPlatSprite = scene2::SpriteNode::allocWithSheet(image, 1, 1);

    auto animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(GLIDING_LOG_ANIMATED), 1, 15, 15);
    plat->setPlatformAnimation(animNode, 15);

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
    std::shared_ptr<Texture> image = _assets->get<Texture>(jsonTypeToAsset[spk->getJsonType()]);
    std::string temp2 = spk->getJsonType();
    std::string temp = jsonTypeToAsset[spk->getJsonType()];

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
std::shared_ptr<Object> ObjectController::createWindObstacle(Vec2 pos, Size size, float scale, const Vec2 windDirection, const Vec2 windStrength, string jsonType, bool isLevelEditorMode)
{
    std::shared_ptr<WindObstacle> wind = WindObstacle::alloc(pos, size, scale, windDirection, windStrength, jsonType);
    return createWindObstacle(wind, isLevelEditorMode);
}

std::shared_ptr<Object> ObjectController::createWindObstacle(std::shared_ptr<WindObstacle> wind, bool isLevelEditorMode)
{
    std::shared_ptr<Texture> gust = _assets->get<Texture>(GUST_TEXTURE);
    std::shared_ptr<scene2::SpriteNode> gustSprite = scene2::SpriteNode::allocWithSheet(gust, 1, 1);
    std::shared_ptr<scene2::PolygonNode> staticSprite;
    std::shared_ptr<scene2::SpriteNode> animNode;

    if (true) {
        animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(FAN_TEXTURE_ANIMATED), 1, 4, 4);
        wind->setFanAnimation(animNode, 4);
        wind->setSceneNode(animNode);
        wind->setName("wind");
    }
    else {
        staticSprite = scene2::SpriteNode::allocWithTexture(_assets->get<Texture>(FAN_TEXTURE));
        wind->setSceneNode(staticSprite);
        wind->getSceneNode()->setVisible(true);
        wind->setName("windLevelEditor");
    }
    

    /*auto animNode1 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_1), 1, 14, 4);
    auto animNode2 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_2), 1, 14, 4);
    auto animNode3 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_3), 1, 14, 4);
    auto animNode4 = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WIND_LVL_4), 1, 14, 4);
    std::vector<std::shared_ptr<scene2::SpriteNode>> gusts;

    gusts.push_back(animNode1);
    gusts.push_back(animNode2);
    gusts.push_back(animNode3);
    gusts.push_back(animNode4);

    wind->setGustAnimation(gusts, 14);*/

    addObstacle(wind, wind->getSceneNode());
    _gameObjects->push_back(wind);

    return wind;
}

/**
* Creates a new mushroom.
*
* @return the mushroom
*
* @param pos The position of the bottom left corner of the platform in Box2D coordinates.
* @param size The dimensions (width, height) of the platform.
*/
std::shared_ptr<Object> ObjectController::createMushroom(Vec2 pos, Size size, float scale, std::string jsonType, bool isLevelEditorMode) {
    std::shared_ptr<Mushroom> mush = Mushroom::alloc(pos, size, jsonType);
    return createMushroom(mush, isLevelEditorMode);
}

std::shared_ptr<Object> ObjectController::createMushroom(std::shared_ptr<Mushroom> mush, bool isLevelEditorMode) {
    auto animNode = scene2::SpriteNode::allocWithSheet(
        _assets->get<Texture>(MUSHROOM_BOUNCE), 1, 9, 9
    );
    mush->setMushroomAnimation(animNode, 9);

    mush->setDensity(BASIC_DENSITY);
    mush->setFriction(BASIC_FRICTION);
    mush->setRestitution(BASIC_RESTITUTION);
    mush->setName("mushroom");
    mush->setDebugColor(DEBUG_COLOR);

    addObstacle(mush, mush->getSceneNode());
    _gameObjects->push_back(mush);

    return mush;
}

/**
* Creates a new bomb.
*
* @return the bomb
*
* @param pos The position of the bottom left corner of the platform in Box2D coordinates.
* @param size The dimensions (width, height) of the platform.
*/
std::shared_ptr<Object> ObjectController::createBomb(Vec2 pos, Size size, float scale, std::string jsonType, bool isLevelEditorMode) {
    std::shared_ptr<Bomb> bomb = Bomb::alloc(pos, size, jsonType);
    return createBomb(bomb, isLevelEditorMode);
}

std::shared_ptr<Object> ObjectController::createBomb(std::shared_ptr<Bomb> bomb, bool isLevelEditorMode) {
    std::shared_ptr<Texture> texture = _assets->get<Texture>(BOMB_TEXTURE);

    bomb->setBodyType(b2_dynamicBody);
    bomb->setDensity(BASIC_DENSITY);
    bomb->setFriction(BASIC_FRICTION);
    bomb->setRestitution(BASIC_RESTITUTION);
    bomb->setName("bomb");
    bomb->setDebugColor(DEBUG_COLOR);

    auto animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(BOMB_TEXTURE_ANIMATED), 1, 14, 14);
    bomb->setAnimation(animNode);

    addObstacle(bomb, bomb->getSceneNode());
    _gameObjects->push_back(bomb);

    return bomb;
}

std::shared_ptr<Object> ObjectController::createTreasure(Vec2 pos, Size size, string jsonType, bool isLevelEditorMode){
    std::shared_ptr<Texture> image = _assets->get<Texture>("treasure");
    std::shared_ptr<Treasure> treas = Treasure::alloc(pos, image->getSize() / _scale, _scale);
    return createTreasure(treas, isLevelEditorMode);
}

std::shared_ptr<Object> ObjectController::createTreasure(std::shared_ptr<Treasure> _treasure, bool isLevelEditorMode) {
    std::shared_ptr<scene2::PolygonNode> sprite;
    std::shared_ptr<scene2::SpriteNode> animNode;
    if (!isLevelEditorMode) {
        animNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("treasure-sheet"), 8, 8, 64);
        _treasure->setAnimation(animNode);
        _treasure->setName("treasure");
    }
    else {
        sprite = scene2::SpriteNode::allocWithTexture(_assets->get<Texture>("treasure"));
        _treasure->setSceneNode(sprite);
        _treasure->setName("treasureLevelEditor");
    }

    //    _treasure->setSceneNode(sprite);
    addObstacle(_treasure, _treasure->getSceneNode());
    
    _treasure->setDebugColor(Color4::YELLOW);
    _gameObjects->push_back(_treasure);
    if (!isLevelEditorMode) {
        _networkController->setTreasure(_treasure);
    }
    return _treasure;
}

std::shared_ptr<Object> ObjectController::createParallaxArtObject(Vec2 pos, Size size, float scale, float angle, int layer, float scrollRate, string jsonType) {
    std::shared_ptr<ArtObject> artObj = ArtObject::alloc(pos, size, scale, angle, layer, jsonType);
    artObj->setParallaxScrollRate(scrollRate);
    auto newObj = createParallaxArtObject(artObj);
    (dynamic_pointer_cast<ArtObject>(newObj))->setLayer(layer);
    return newObj;
}

/* DO NOT call this overload directly. If you do, it will not have a proper scroll rate. Use the other overload instead. */
std::shared_ptr<Object> ObjectController::createParallaxArtObject(std::shared_ptr<ArtObject> art) {
    std::shared_ptr<Texture> image;
    bool isAnimated = animatedArtObjects.find(art->getJsonType()) != animatedArtObjects.end();
    image = _assets->get<Texture>(jsonTypeToAsset[art->getJsonType()]);
    if (image == nullptr) {
        image = _assets->get<Texture>("earth");
    }
    int rows = 1;
    int cols = 1;
    if (isAnimated) {
        auto pair = animatedArtObjects[art->getJsonType()];
        rows = pair.first;
        cols = pair.second;
        // TODO: fix this. Maybe use same system as ArtAssetHelperMaps?
        art->setAnimationDuration(1.0f);
    }
    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, rows, cols);
    art->setSceneNode(sprite);
    art->setAnimated(isAnimated);
    art->setBodyType(b2_staticBody);
    art->setDensity(BASIC_DENSITY);
    art->setFriction(BASIC_FRICTION);
    art->setRestitution(BASIC_RESTITUTION);
    art->setDebugColor(DEBUG_COLOR);
    art->setName("artObject");
    // Disable ArtObject collision physics
    art->setSensor(true);
    addObstacle(art, sprite);
    art->setAnimation(sprite);
    _gameObjects->push_back(art);

    return art;
}

std::shared_ptr<Object> ObjectController::createArtObject(Vec2 pos, Size size, float scale, float angle, std::string jsonType) {
    return createArtObject(ArtObject::alloc(pos, size, scale, angle, jsonType));
}

std::shared_ptr<Object> ObjectController::createArtObject(std::shared_ptr<ArtObject> art) {
    std::shared_ptr<Texture> image;
    bool isAnimated = animatedArtObjects.find(art->getJsonType()) != animatedArtObjects.end();
    image = _assets->get<Texture>(jsonTypeToAsset[art->getJsonType()]);
    if (image == nullptr) {
        image = _assets->get<Texture>("earth");
    }
    int rows = 1;
    int cols = 1;
    if (isAnimated) {
        auto pair = animatedArtObjects[art->getJsonType()];
        rows = pair.first;
        cols = pair.second;
        // TODO: fix this. Maybe use same system as ArtAssetHelperMaps?
        art->setAnimationDuration(1.0f);
    }
    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image, rows, cols);
    art->setSceneNode(sprite);
    art->setAnimated(isAnimated);
    if (jsonTypeToLayer.find(art->getJsonType()) != jsonTypeToLayer.end()) {
        // jsonType IS in the map
        art->setLayer(jsonTypeToLayer[art->getJsonType()]);
    }
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
    art->setAnimation(sprite);
    _gameObjects->push_back(art);

    return art;
}

std::shared_ptr<Object> ObjectController::createGoalDoor(Vec2 goalPos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>(GOAL_TEXTURE);
    std::shared_ptr<scene2::SpriteNode> sprite;



    sprite = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("goal-spritesheet"), 1, 5, 5);
    

    _goalPos = goalPos;

    Size goalSize(image->getSize().width / _scale, image->getSize().height / _scale);
    
    std::shared_ptr<GoalDoor> goalDoor = GoalDoor::alloc(goalPos, goalSize, _scale);

    goalDoor->setAnimation(sprite);

    goalDoor->setBodyType(b2_staticBody);
    goalDoor->setDensity(0.0f);
    goalDoor->setFriction(0.0f);
    goalDoor->setRestitution(0.0f);
    goalDoor->setSensor(true);
    goalDoor->setName("goalDoor");

    //sprite = scene2::PolygonNode::allocWithTexture(image);
    //sprite->setColor(Color4(1, 255, 0));  // Greenish color
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
            createTreasure(std::dynamic_pointer_cast<Treasure>(obj), true);
        }
        
    }
    else if (key == "windObstacles") {
        createWindObstacle(std::dynamic_pointer_cast<WindObstacle>(obj));
    }
    else if (key == "artObjects") {
        createArtObject(std::dynamic_pointer_cast<ArtObject>(obj));
    }
    else if (key == "tiles") {
        createTile(std::dynamic_pointer_cast<Tile>(obj));
    }
}

void ObjectController::removeObject(std::shared_ptr<Object> object){
    auto it = std::find(_gameObjects->begin(), _gameObjects->end(), object);

    // Check if the element was found
    if (it != _gameObjects->end()) {
        // Calculate the index by subtracting the beginning iterator
        int index = static_cast<int>(std::distance(_gameObjects->begin(), it));
        _gameObjects->erase(_gameObjects->begin() + index);
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




