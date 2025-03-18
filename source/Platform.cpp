#include "Platform.h"
#include "Object.h"

#define JSON_KEY  "platforms";

using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void Platform::setPosition(const cugl::Vec2& position) {
    _position = position;
    _box->setPosition(position + _size / 2);
}

void Platform::update(float timestep) {
    if (!_moving) return;
    CULog("udpating moving");
    
    Vec2 pos = _box->getPosition();
    Vec2 target = _forward ? _endPos- _size/2 : _startPos-_size/2;
    Vec2 toTarget = target - pos;
    float distance = toTarget.length();
    CULog("Pos:(%.2f, %.2f) Target:(%.2f, %.2f) Dist:%.2f Speed:%.2f Forward:%d",
          pos.x, pos.y, target.x, target.y, distance, _speed, _forward);


    Vec2 direction = toTarget;
            direction.normalize();
            Vec2 step = direction * (_speed * timestep);

    //if next step will move over the end_pos
    if (distance < _speed * timestep || toTarget.dot(_box->getLinearVelocity() * timestep) < 0) {
        CULog("turning");
        pos = target;
        _box->setPosition(pos);
        _forward = !_forward;
        Vec2 newTarget = _forward ? _endPos-_size/2 : _startPos-_size/2;
        Vec2 direction = newTarget - pos;
        direction.normalize();         
        Vec2 velocity = direction * _speed;
        _box->setLinearVelocity(velocity);
    }

}

string Platform::getJsonKey() {
    return JSON_KEY;
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
    return Platform::init(pos, size, "default");
}
bool Platform::init(const Vec2 pos, const Size size, string jsonType) {
    Size nsize = size;
    _box = cugl::physics2::BoxObstacle::alloc(pos, Size(nsize.width, nsize.height));
    _size = size;
    _itemType = Item::PLATFORM;
    _jsonType = jsonType;
    _position = pos;
    _size = size;
    return true;
}

// Init method used for networked platforms
bool Platform::init(const Vec2 pos, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
    Size nsize = size;
    // The long platform is shorter in height
    _box = box;
    _size = size;
    _itemType = Item::PLATFORM;
    _position = pos;
    _size = size;
    return true;
}

bool Platform::initMoving(const Vec2 pos, const Size size, const Vec2 start, const Vec2 end, float speed) {
    if (!init(pos, size)) return false;
    _moving = true;
    _startPos = start;
    _endPos   = end+ Vec2(size.width/2, size.height/2);
    _speed    = speed;
    _forward  = true;
    _position = pos;
    _size = size;
    //enable moving
    _box->setBodyType(b2_kinematicBody);
    Vec2 direction = _endPos - _startPos;
    direction.normalize();
    _box->setLinearVelocity(direction * _speed);
    return true;
}
//for networked moving platform 
bool Platform::initMoving(const Vec2 pos, const Size size, const Vec2 start, const Vec2 end, float speed, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
    if (!init(pos, size)) return false;
    _moving = true;
    _startPos = start;
    _endPos   = end;
    _speed    = speed;
    _forward  = true;
    _position = pos;
    _size = size;
    //enable moving
    _box  =box;
    _box->setBodyType(b2_kinematicBody);
    Vec2 direction = _endPos - _startPos;
    direction.normalize();
    _box->setLinearVelocity(direction * _speed);
    return true;
}

std::map<std::string, std::any> Platform::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
         {"type", std::string(_jsonType)}
    };
    return m;
}
