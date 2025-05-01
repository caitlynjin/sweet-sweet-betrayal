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
    PolygonObstacle::setPosition(position + _size/2);
}

void Platform::updateAnimation(float timestep) {
    if (_platTimeline->isActive("current")) {
        // NO OP
        CULog("PlatformAnimationPrepping");
    }
    else {
        _platTimeline->add("current", _platAction, 1.0f);
    }
    _platTimeline->update(timestep);
}

void Platform::update(float timestep) {
    PolygonObstacle::update(timestep);
}

void Platform::updateMovingPlatform(float timestep) {
    _turnCount++;
    if (!_moving) return;

    Vec2 pos = getPosition();
    Vec2 target = _forward ? _endPos : _startPos;
    Vec2 toTarget = target - pos;
    float distance = toTarget.length();
    Vec2 direction = toTarget;
    direction.normalize();
    Vec2 step = direction * (_speed * timestep);

    // If next step will move over the end_pos
    if (_turnCount==150) {
        CULog("turning");
        pos = target;
        setPosition(pos);
        _forward = !_forward;
        
        Vec2 newTarget = _forward ? _endPos : _startPos;
        Vec2 direction = newTarget - pos;
        direction.normalize();
        Vec2 velocity = direction * _speed;
        setLinearVelocity(velocity);

        // Optionally log the turn count for debugging
        CULog("Platform has turned %d times", _turnCount);
        _turnCount=0;
    }
    updateAnimation(timestep);
}


string Platform::getJsonKey() {
    return JSON_KEY;
}

void Platform::dispose() {
    Object::dispose();

    markRemoved(true);
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
    _size = size;
    _itemType = Item::PLATFORM;
    _jsonType = jsonType;
    _position = pos;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.5f, 0), Size(size.width, size.height * 0.5));

    if (PolygonObstacle::init(rect)){
        setPosition(pos + size/2);
        return true;
    }
    
    return false;
}


bool Platform::initMoving(const Vec2 pos, const Size size, const Vec2 start, const Vec2 end, float speed) {
    if (!init(pos, size)) return false;
    _moving = true;
    _startPos = start + size/2;
    _endPos   = end + size/2;
    _speed    = speed;
    _forward  = true;
    _position = pos;
    _size = size;
    _itemType = Item::MOVING_PLATFORM;

    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.5f, 0), Size(_size.width, _size.height * 0.5));

    if (PolygonObstacle::init(rect)){
        setPosition(pos + size/2);
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
    CULog("update moving");
    if (_moving) {
        Vec2 oldStartPos = _startPos;
        Vec2 oldEndPos = _endPos;

        _endPos = gridpos + (_endPos - _startPos) + _size/2;
        _startPos = gridpos + _size/2;
        _forward = true;
        
        Vec2 direction = _endPos - _startPos;
        direction.normalize();
        setLinearVelocity(direction * _speed);
        CULog("Platform Moved | Start: (%.2f, %.2f) -> (%.2f, %.2f) | End: (%.2f, %.2f) -> (%.2f, %.2f) | Velocity: (%.2f, %.2f)",
              oldStartPos.x, oldStartPos.y, _startPos.x, _startPos.y,
              oldEndPos.x, oldEndPos.y, _endPos.x, _endPos.y,
              direction.x * _speed, direction.y * _speed);
        
        return true;
    }
        
    return false;
}

void Platform::setPlatformAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    //Create sprite object
    _platSpriteNode = sprite;
    _platSpriteNode->setAnchor(0.0f, 0.0f);
    _platSpriteNode->setPosition(getPosition().x - 120, getPosition().y - 32);
    _platSpriteNode->setVisible(true);
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }
    _node->addChild(_platSpriteNode);

    //Create the spritesheet
    _platTimeline = ActionTimeline::alloc();

    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _platAnimateSprite = AnimateSprite::alloc(forward);
    _platAction = _platAnimateSprite->attach<scene2::SpriteNode>(_platSpriteNode);
}

