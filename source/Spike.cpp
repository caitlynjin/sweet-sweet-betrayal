#include "Spike.h"
#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;

void Spike::update(float timestep) {
}

void Spike::dispose() {}



using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Spike at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Spike::init(const Vec2 pos, const Size size) {
    Size nsize = size;
    _box = cugl::physics2::BoxObstacle::alloc(pos, nsize);
    return true;
}




