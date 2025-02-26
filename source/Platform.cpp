#include "Platform.h"
#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;

void Platform::update(float timestep) {
    if (!_moving) return;
    
    Vec2 pos = _box->getPosition();
    Vec2 target = _forward ? _endPos : _startPos;
    Vec2 toTarget = target - pos;
    float distance = toTarget.length();

    
    //if next step will move over the end_pos
    if (distance < _speed * timestep) {
        pos = target;
        _box->setPosition(pos);
        _forward = !_forward;
        Vec2 newTarget = _forward ? _endPos : _startPos;
        Vec2 direction = newTarget - pos;
        direction.normalize();         
        Vec2 velocity = direction * _speed;
        _box->setLinearVelocity(velocity);
    }

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
bool Platform::init(const Vec2 pos, const Size size) {
    Size nsize = size;
    _box = cugl::physics2::BoxObstacle::alloc(pos, nsize);
    return true;
}
bool Platform::initMoving(const Vec2 pos, const Size size, const Vec2 start, const Vec2 end, float speed) {
    if (!init(pos, size)) return false;
    _moving = true;
    _startPos = start;
    _endPos   = end+ Vec2(size.width/2, size.height/2);
    _speed    = speed;
    _forward  = true;
    //enable moving
    _box->setBodyType(b2_kinematicBody);
    Vec2 direction = _endPos - _startPos;
    direction.normalize();
    _box->setLinearVelocity(direction * _speed);
    return true;
}




