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
//    float testScale = 1.0f;
    CULog("Tile drawscale: %f", scale);
    _drawScale = 1.0f;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-0.5f, -0.5f), Size(nsize.width, nsize.height));
        
    if (PolygonObstacle::init(rect)){
        setPosition(pos + size / 2);
        return true;
    }
    
    return false;
}


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
