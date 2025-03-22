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

void ArtObject::dispose() {}



using namespace cugl;

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
bool ArtObject::init(const Vec2 pos, const Size size, float scale, float angle) {
    return ArtObject::init(pos, size, scale, angle, "default");
}
bool ArtObject::init(const Vec2 pos, const Size size, float scale, float angle, string jsonType) {
    _drawScale = scale;
    _position = pos;
    _size = size;

    return true;
}

std::map<std::string, std::any> ArtObject::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"scale", double(_drawScale)},
        {"type", std::string(_jsonType)}
    };
    return m;
}