//
//  Bomb.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 4/25/25.
//

#include "Bomb.h"
#include "Object.h"

#define JSON_KEY  "bombs";

using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void Bomb::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position + _size/2);
}

void Bomb::update(float timestep) {
    PolygonObstacle::update(timestep);
}

string Bomb::getJsonKey() {
    return JSON_KEY;
}

void Bomb::dispose() {
    Object::dispose();
    markRemoved(true);
}

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Bomb at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the bomb in grid units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Bomb::init(const Vec2 pos, const Size size) {
    return Bomb::init(pos, size, "default");
}

bool Bomb::init(const Vec2 pos, const Size size, string jsonType) {
    _size = size;
    _itemType = Item::BOMB;
    _jsonType = jsonType;
    _position = pos;

    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-0.5f, -0.5f), size);

    if (PolygonObstacle::init(rect)){
        setPosition(pos + size/2);
        return true;
    }

    return false;
}

std::map<std::string, std::any> Bomb::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"type", std::string(_jsonType)}
    };
    return m;
}

