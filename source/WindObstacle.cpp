#include "WindObstacle.h"
#include "Object.h"

#define JSON_KEY "windObstacles"

using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void WindObstacle::setPosition(const cugl::Vec2& position) {
    _position = position;
    _gust->setPosition(position + _size / 2);
}

void WindObstacle::update(float timestep) {
}

string WindObstacle::getJsonKey() {
    return JSON_KEY;
}

void WindObstacle::dispose() {}

using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 gust) {
    return WindObstacle::init(pos, size, gust, "default");
}
/**
 * Initializes a new WindObstacle at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 gust, string jsonType) {
    Size nsize = size;
    _size = size;
    _itemType = WIND;
    _position = pos;
    _size = size;
    _jsonType = jsonType;

    _gust = cugl::physics2::BoxObstacle::alloc(pos + _size/2, nsize);
    _gust->setDensity(0.0f);
    _gust->setFriction(0.0f);
    _gust->setRestitution(0.0f);
    _gust->setSensor(true);
    _gust->setName("gust");
    
    _gustDir = gust;

    
    return true;
    
}

std::map<std::string, std::any> WindObstacle::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"gustDirX", double(_gustDir.x)},
        {"gustDirY", double(_gustDir.y)},
         {"type", std::string(_jsonType)}
    };
    return m;
}

