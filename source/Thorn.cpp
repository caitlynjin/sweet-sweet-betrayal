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
void Thorn::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position);
}

string Thorn::getJsonKey() {
    return JSON_KEY;
}

void Thorn::dispose() {
    Object::dispose();

    markRemoved(true);
//    _box = nullptr;
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
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-0.5f, -0.5f), size);
    
    if (PolygonObstacle::init(rect)){
        setPosition(pos);
        return true;
    }

    return false;
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


