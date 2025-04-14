//
//  Thorn.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 4/12/25.
//

#include "Thorn.h"
#include "Object.h"

#define JSON_KEY "thorns"

using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void Thorn::setPosition(const cugl::Vec2& position) {
    _position = position;
    _box->setPosition(position + _size/2);
}

string Thorn::getJsonKey() {
    return JSON_KEY;
}

void Thorn::dispose() {
    Object::dispose();

    _box->markRemoved(true);
    _box = nullptr;
}



using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Thorn at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Thorn::init(const Vec2 pos, const Size size) {
    return Thorn::init(pos, size, "default");
}
bool Thorn::init(const Vec2 pos, const Size size, string jsonType) {
    _position = pos;
    _itemType = Item::THORN;
    _size = size;
    _box = cugl::physics2::BoxObstacle::alloc(pos + size/2, size);
    return true;
}

/** Init method used for networked thorns */
bool Thorn::init(const Vec2 pos, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
    _box = box;
    _size = size;
    _itemType = Item::THORN;
    _position = pos;
    return true;
}

std::map<std::string, std::any> Thorn::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"type", std::string(_jsonType)}
    };
    return m;
}


