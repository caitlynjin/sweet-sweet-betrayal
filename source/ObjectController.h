//
//  ObjectController.h
//  SweetSweetBetrayal
//
//  Created by jessie jia on 3/13/25.
//

//This file will handle generation logic for all obstacles: spikes, wind gust,treasure


#ifndef OBJECT_CONTROLLER_H
#define OBJECT_CONTROLLER_H

#include <memory>
#include "Constants.h"
#include "Platform.h"
#include "Spike.h"
#include "WindObstacle.h"
#include "Treasure.h"
#include "Object.h"
#include "Spike.h"
#include "Tile.h"
#include <cugl/cugl.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include "NetworkController.h"
#include "ArtObject.h"

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;


class ObjectController {
private:
    std::shared_ptr<cugl::AssetManager> _assets;
    std::shared_ptr<cugl::physics2::distrib::NetWorld> _world;
    float _scale;
    /** A list of all objects to be updated during each animation frame. */
    std::vector<std::shared_ptr<Object>>* _gameObjects;
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _debugnode;
    /** Initial width */
    float _growingWallWidth = 1.0f;
    /** Growth rate per second  */
    float _growingWallGrowthRate = 0.4f;
    std::shared_ptr<physics2::PolygonObstacle> _growingWall;
    std::shared_ptr<scene2::PolygonNode> _growingWallNode;
    /** Reference to the treasure */
    std::shared_ptr<Treasure> _treasure;
    /** Reference to goal position */
    Vec2 _goalPos;
   
    std::shared_ptr<NetworkController> _networkController;

    
    
public:
    ObjectController(const std::shared_ptr<AssetManager>& assets,
                     const std::shared_ptr<cugl::physics2::distrib::NetWorld>& world,
                     float scale,
                     const std::shared_ptr<scene2::SceneNode> world_node,
                     const std::shared_ptr<scene2::SceneNode> debug_node,
                     std::vector<std::shared_ptr<Object>>* gameObjects);


    /** Returns the list of objects. 
    *
    * @return the list of objects
    */
    std::vector<std::shared_ptr<Object>>* getObjects() {
        return _gameObjects;
    }
    /**
     * Creates a platform.
     *
     * @return the platform being created
     *
     * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
     * @param size The size of the platform in Box2D coordinates.
     * @param wall Whether this is a wall or not (if not it is a user placed platform)
     */
    std::shared_ptr<Object> createPlatform(Vec2 pos, Size size, string jsonType);

    /**
     * Creates a platform.
     *
     * @return the platform being created
     *
     * @param The platform being created (that has not yet been added to the physics world).
     */
    std::shared_ptr<Object> createPlatform(std::shared_ptr<Platform> plat);

    /*Creates a 1 by 1 tile*/
    std::shared_ptr<Object> createTile(Vec2 pos, Size size, string jsonType, float scale);
    std::shared_ptr<Object> createTile(std::shared_ptr<Tile> tile);

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
    std::shared_ptr<Object> createMovingPlatform(Vec2 pos, Size size, Vec2 end, float speed);
    /** Creates a spike.
    * @param pos The position of the bottom left corner of the spike in Box2D coordinates.
    * @param size The size of the spike in Box2D coordinates.
    */
    std::shared_ptr<Object> createSpike(Vec2 pos, Size size, float scale, float angle, string jsonType);

    std::shared_ptr<Object> createSpike(std::shared_ptr<Spike> spk);
    
    /**
    * Creates a new windobstacle
    *
    * @return the wind obstacle
    *
    * @param pos The position of the bottom left corner of the platform in Box2D coordinates.
    * @param size The dimensions (width, height) of the platform.
    */

   std::shared_ptr<Object> createWindObstacle(Vec2 pos, Size size, const Vec2 windDirection, const Vec2 windStrength, std::string jsonType);

   std::shared_ptr<Object> createWindObstacle(std::shared_ptr<WindObstacle> wind);
    
    
    /** Creates a treasure
    * @param pos The position of the bottom left corner of the treasure in Box2D coordinates.
    * @param size The size of the treasure in Box2D coordinates.
    */
    std::shared_ptr<Object> createTreasure(Vec2 pos, Size size, string jsonType, bool isLevelEditorMode = false);

    std::shared_ptr<Object> createTreasure(std::shared_ptr<Treasure> treasure, bool isLevelEditorMode = false);

    /** Creates an art object */
    std::shared_ptr<Object> createArtObject(std::shared_ptr<ArtObject> art);
    std::shared_ptr<Object> createArtObject(Vec2 pos, Size size, float scale, float angle, string jsonType);

    /**creates teh goal door**/
    std::shared_ptr<physics2::BoxObstacle> createGoalDoor(Vec2 goalPos);
    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with dude.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * @param obj    The physics object to add
     * @param node   The scene graph node to attach it to
     * @param useObjPosition  Whether to update the node's position to be at the object's position
     */
    void addObstacle(const std::shared_ptr<physics2::Obstacle>& obj,
                         const std::shared_ptr<scene2::SceneNode>& node,
                         bool useObjPosition=true);

    /**called in Game Scene to create the corresponding object type
    @param obj    The physics object to add
     **/
    void processLevelObject(std::shared_ptr<Object> obj, bool levelEditing = false);

    void setNetworkController(std::shared_ptr<NetworkController> networkController) {
        _networkController = networkController;
    }

    std::shared_ptr<Treasure> getTreasure() {return _treasure;}

    Vec2 getGoalPos() {return _goalPos;}
    

    
//    /**
//    * Create the growing wall if not created. Otherwise, increase its width
//    *
//    * @param timestep  The elapsed time since the last frame.
//    */
//    void updateGrowingWall(float timestep);
};

#endif // OBJECT_CONTROLLER_H
    
