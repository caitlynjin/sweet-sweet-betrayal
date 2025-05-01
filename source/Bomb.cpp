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

        // Create bomb radius sensor
        b2PolygonShape sensorBox;
        sensorBox.SetAsBox(1.5f, 1.5f);

        b2FixtureDef sensorDef;
        sensorDef.shape = &sensorBox;
        sensorDef.isSensor = true;
//        sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(this); // Optional: useful for contact handling

        // Attach the sensor to the body
        if (_body != nullptr) {
            _body->CreateFixture(&sensorDef);
        }

//        auto debugBox = scene2::WireNode::alloc();
//        debugBox->setAnchor(Vec2::ANCHOR_CENTER);
//        debugBox->setColor(Color4::RED); // You can change color as needed
//        debugBox->setContentSize(Size(3.0f, 3.0f)); // 3x3 box
//        debugBox->setPosition(pos + size / 2); // Same center as bomb
//
//        // Attach it to the scene graph (e.g., the same node as the bomb)
//        setSceneNode(debugBox);

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

