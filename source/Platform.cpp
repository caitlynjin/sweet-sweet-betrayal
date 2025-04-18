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
void Platform::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position);
    
}

void Platform::update(float timestep) {
    PolygonObstacle::update(timestep);
    
    if (!_moving) return;
    Vec2 pos = getPosition();
    Vec2 target = _forward ? _endPos : _startPos;
    Vec2 toTarget = target - pos;
    float distance = toTarget.length();
//    CULog("Pos:(%.2f, %.2f) Target:(%.2f, %.2f) Dist:%.2f Speed:%.2f Forward:%d",
//          pos.x, pos.y, target.x, target.y, distance, _speed, _forward);


    Vec2 direction = toTarget;
    direction.normalize();
    Vec2 step = direction * (_speed * timestep);

    //if next step will move over the end_pos
    if (distance < _speed * timestep || toTarget.dot(getLinearVelocity() * timestep) < 0) {
        CULog("turning");
        pos = target;
        setPosition(pos);
        _forward = !_forward;
        Vec2 newTarget = _forward ? _endPos : _startPos;
        Vec2 direction = newTarget - pos;
        direction.normalize();         
        Vec2 velocity = direction * _speed;
        setLinearVelocity(velocity);
    }

}

string Platform::getJsonKey() {
    return JSON_KEY;
}

void Platform::dispose() {
    Object::dispose();

    markRemoved(true);
//    _box = nullptr;
}



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
    
    
    _size = size;
    _itemType = Item::PLATFORM;
    _jsonType = jsonType;
    _position = pos;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.5f, 0.0f), Size(nsize.width, nsize.height * 0.5));
        
    if (PolygonObstacle::init(rect)){
        setPosition(pos);
        return true;
    }
    
    return false;
}


bool Platform::initMoving(const Size size, const Vec2 start, const Vec2 end, float speed) {
//    if (!init(pos, size)) return false;
    _moving = true;
    _startPos = start;
    _endPos   = end;
    _speed    = speed;
    _forward  = true;
    _position = start;
    _size = size;
    _itemType = Item::MOVING_PLATFORM;
    //enable moving
    
    
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.5f, 0.0f), Size(_size.width, _size.height * 0.5));
        
    if (PolygonObstacle::init(rect)){
        setPosition(start);
        setBodyType(b2_kinematicBody);
        Vec2 direction = _endPos - _startPos;
        direction.normalize();
        setLinearVelocity(direction * _speed);
        
        return true;
    }
    
    
    return false;
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
bool Platform::updateMoving(Vec2 gridpos) {
    if (_moving) {
        Vec2 oldStartPos = _startPos;
        Vec2 oldEndPos = _endPos;
        
        _endPos = gridpos + (_endPos - _startPos);
        _startPos = gridpos;
        _forward = true;
        
        Vec2 direction = _endPos - _startPos;
        direction.normalize();
        _box->setLinearVelocity(direction * _speed);
        CULog("Platform Moved | Start: (%.2f, %.2f) -> (%.2f, %.2f) | End: (%.2f, %.2f) -> (%.2f, %.2f) | Velocity: (%.2f, %.2f)",
              oldStartPos.x, oldStartPos.y, _startPos.x, _startPos.y,
              oldEndPos.x, oldEndPos.y, _endPos.x, _endPos.y,
              direction.x * _speed, direction.y * _speed);
        
        return true;
    }
        
    return false;
}

