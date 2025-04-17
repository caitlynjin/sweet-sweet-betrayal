#ifndef __Tile_H__
#define __Tile_H__
#include <cugl/cugl.h>
#include "Object.h"

using namespace cugl;
using namespace std;
using namespace Constants;

class Tile : public Object {

private:
    /** The BoxObstacle wrapped by this Tile object */

    bool   _forward = true;
    bool _wall = false;

public:
    Tile() : Object() {}

    Tile(Vec2 pos) : Object(pos, Item::TILE_ITEM) {}

    /** The update method for the Tile */
    void update(float timestep) override;

    string getJsonKey() override;

    ~Tile(void) override { dispose(); }

    void dispose() override;
    
    /** The scene graph node for the Treasure. */
    std::shared_ptr<scene2::SceneNode> _node;
    
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPositionInit(const cugl::Vec2& position) override;


    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Tile and link it to a physics object.
    */
    static std::shared_ptr<Tile> alloc(const Vec2 position, const Size size, std::string jsonType, float scale) {
        std::shared_ptr<Tile> result = std::make_shared<Tile>();
        return (result->init(position, size, jsonType, scale) ? result : nullptr);
    }

    static std::shared_ptr<Tile> alloc(const Vec2 position, const Size size, std::string jsonType, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
        std::shared_ptr<Tile> result = std::make_shared<Tile>();
        return (result->init(position, size, jsonType, box) ? result : nullptr);
    }

    // New init method for networked Tiles
    bool init(const Vec2 pos, const Size size, std::string jsonType, std::shared_ptr<cugl::physics2::BoxObstacle> box);

    bool init(const Vec2 pos, const Size size, std::string jsonType, float scale);

    // Map for JSON level management
    std::map<std::string, std::any> getMap() override;

    // Gets if this is a wall
    bool isWall() { return _wall; }


};


#endif /* __Tile_H__ */



