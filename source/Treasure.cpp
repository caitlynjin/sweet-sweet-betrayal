#include "Treasure.h"
//#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;


void Treasure::update(float timestep) {
//    Vec2 currPos = getPosition();
//    currPos += Vec2(1,1);
//    setPosition(currPos);
//    _box->setPosition(currPos);
    
//    if (_node != nullptr) {
//        _node->setPosition(getPosition()*_drawScale);
//    }
}

void Treasure::setPosition(const cugl::Vec2 &position){
    _box->setPosition(position);
    if (_node != nullptr) {
        _node->setPosition(position*_drawScale);
    }
}

void Treasure::dispose() {}



using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Treasure at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Treasure::init(const Vec2 pos, const Size size, float scale) {
    Size nsize = size;
    _treasureTexture = "";
    _drawScale = scale;
    _box = cugl::physics2::BoxObstacle::alloc(pos, nsize);
    _box->setSensor(true);
    return true;
}
