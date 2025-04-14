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
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;
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

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPosition(const cugl::Vec2& position) override;

    std::shared_ptr<cugl::physics2::Obstacle> getObstacle() override {
        return _box;
    }

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Tile and link it to a physics object.
    */
    static std::shared_ptr<Tile> alloc(const Vec2 position, const Size size) {
        std::shared_ptr<Tile> result = std::make_shared<Tile>();
        return (result->init(position, size) ? result : nullptr);
    }

    static std::shared_ptr<Tile> alloc(const Vec2 position, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
        std::shared_ptr<Tile> result = std::make_shared<Tile>();
        return (result->init(position, size, box) ? result : nullptr);
    }

    // New init method for networked Tiles
    bool init(const Vec2 pos, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box);

    bool init(const Vec2 pos, const Size size);

    // Map for JSON level management
    std::map<std::string, std::any> getMap() override;

    // Gets if this is a wall
    bool isWall() { return _wall; }

};


#endif /* __Tile_H__ */



