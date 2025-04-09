#include "ArtObject.h"
#include "Object.h"

#define JSON_KEY "artObjects"

using namespace cugl;
using namespace cugl::graphics;

void ArtObject::update(float timestep) {
}

string ArtObject::getJsonKey() {
    return JSON_KEY;
}

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new ArtObject at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the ArtObject is initialized properly, false otherwise.
 */
bool ArtObject::init(const Vec2 pos, const Size size, float scale, float angle, int layer) {
    return ArtObject::init(pos, size, scale, angle, layer, "default");
}

bool ArtObject::init(const Vec2 pos, const Size size, float scale, float angle, int layer, string jsonType) {
    _drawScale = scale;
    _position = pos;
    _size = size;
    _layer = layer;
    _angle = angle;

    return true;
}
/**
 * Sets the position
 *
 * @param position   The position
 */
void ArtObject::setPosition(const cugl::Vec2& position) {
    _position = position;
    _node->setPosition((position + _size / 2) * _drawScale);
}

void ArtObject::setPriority(int layer) {
    _node->setPriority(layer);
}

void ArtObject::dispose() {
    Object::dispose();

    if (_node && _node->getParent()) {
        _node->removeFromParent();
        _node = nullptr;
    }
}

std::map<std::string, std::any> ArtObject::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"scale", double(_drawScale)},
        {"angle", double(_angle)},
        {"layer", long(_layer)},
        {"type", std::string(_jsonType)}
    };
    return m;
}