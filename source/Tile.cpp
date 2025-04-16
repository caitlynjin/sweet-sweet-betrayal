#include "Tile.h"
#include "Object.h"

#define JSON_KEY  "tiles";

using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void Tile::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position + _size / 2);
}

void Tile::update(float timestep) {
    return;
}

string Tile::getJsonKey() {
    return JSON_KEY;
}

void Tile::dispose() {
    Object::dispose();

    markRemoved(true);
//    _box = nullptr;
}

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Tile at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Tile::init(const Vec2 pos, const Size size, std::string jsonType, float scale) {
    Size nsize = size;
    _position = pos;
    _size = size;
    _jsonType = jsonType;
    _itemType = jsonTypeToItemType[jsonType];
    _drawScale = scale;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(pos + size / 2, Size(nsize.width, nsize.height));
        
    if (PolygonObstacle::init(rect)){
        return true;
    }
    
    return false;
}

// Init method used for networked Tiles
//bool Tile::init(const Vec2 pos, const Size size, std::string jsonType, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
//    Size nsize = size;
//    // The long Tile is shorter in height
//    _box = box;
//    _size = size;
//    _jsonType = jsonType;
//    _itemType = jsonTypeToItemType[jsonType];
//    _position = pos;
//    return true;
//}

std::map<std::string, std::any> Tile::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
         {"type", std::string(_jsonType)}
    };
    return m;
}
