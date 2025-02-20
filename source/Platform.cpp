#include "Platform.h"
#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;

void Platform::update(float timestep) {
}

void Platform::dispose() {}



using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Platform at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Platform::init(const Vec2& pos, const Size& size) {
    Size nsize = size;
    _box = std::make_shared<cugl::physics2::BoxObstacle>();
    if (_box->init(pos, nsize)) {
        return true;
    }
    return false;
}




