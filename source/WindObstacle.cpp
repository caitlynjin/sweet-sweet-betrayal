#include "WindObstacle.h"
#include "Object.h"

#define JSON_KEY "windObstacles"

using namespace cugl;
using namespace cugl::graphics;

void WindObstacle::update(float timestep) {
}

string WindObstacle::getJsonKey() {
    return JSON_KEY;
}

void WindObstacle::dispose() {}

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new WindObstacle at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 gust) {
    Size nsize = size;
    _gust = cugl::physics2::BoxObstacle::alloc(pos, nsize);
    _gust->setBodyType(b2_staticBody);
    _gust->setDensity(0.0f);
    _gust->setFriction(0.0f);
    _gust->setRestitution(0.0f);
    _gust->setSensor(true);
    _gust->setName("gust");
    
    _gustDir = gust;
    return true;
    
}

