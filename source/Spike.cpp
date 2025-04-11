#include "Spike.h"
#include "Object.h"

#define JSON_KEY "spikes"

using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void Spike::setPosition(const cugl::Vec2& position) {
    _position = position;
    _hitbox->setPosition(position + _size/2);
    _node->setPosition((position + _size/2) * _drawScale);
}

void Spike::update(float timestep) {
}

string Spike::getJsonKey() {
    return JSON_KEY;
}

void Spike::dispose() {
    Object::dispose();

    _hitbox->markRemoved(true);
    _hitbox = nullptr;

    if (_node && _node->getParent()) {
        _node->removeFromParent();
        _node = nullptr;
    }
}



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
bool Spike::init(const Vec2 pos, const Size size, float scale, float angle) {
    return Spike::init(pos, size, scale, angle, "default");
}
bool Spike::init(const Vec2 pos, const Size size, float scale, float angle, string jsonType) {
    _spikeTexture = "";
    _drawScale = scale;
    _position = pos;
    _itemType = Item::SPIKE;
    _size = size;
    _angle = angle;
    Size nsize = size*.40; // FIX LATER
    
    Vec2 posA = Vec2(-nsize.width/2, -nsize.height/2);
    Vec2 posB = Vec2(nsize.width/2, -nsize.height/2);
    Vec2 posC = Vec2(0, nsize.height / 2);
    
    PolyFactory factory;
    Poly2 triangle = factory.makeTriangle(posA, posB, posC);
    _hitbox = physics2::PolygonObstacle::alloc(triangle);
    _hitbox->setDebugColor(Color4::YELLOW);
    _hitbox->setPosition(pos + size / 2);
    _hitbox->setAngle(angle);
    
    
    return true;
}

std::map<std::string, std::any> Spike::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"scale", double(_drawScale)},
        {"angle", double(_angle)},
        {"type", std::string(_jsonType)}
    };
    return m;
}


