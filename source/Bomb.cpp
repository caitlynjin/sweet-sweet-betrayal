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
    updateAnimation(timestep);
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
        setSensor(true);

        // Create bomb radius sensor
        b2PolygonShape sensorBox;
        sensorBox.SetAsBox(1.5f, 1.5f);

        b2FixtureDef sensorDef;
        sensorDef.shape = &sensorBox;
        sensorDef.isSensor = true;
        sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

        // Attach the sensor to the body
        if (_body != nullptr) {
            _body->CreateFixture(&sensorDef);
        }

        return true;
    }

    return false;
}

void Bomb::updateAnimation(float timestep){
    doStrip(_animAction, DURATION);
    _timeline->update(timestep);
}

void Bomb::setAnimation(std::shared_ptr<scene2::SpriteNode> sprite){
    _animNode = sprite;
    _sceneNode = _animNode;
    _animNode->setVisible(true);
    _timeline = ActionTimeline::alloc();

    // Create the frame sequence
    const int nFrames = 14;

    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animation
    _animSprite = AnimateSprite::alloc(forward);
    _animAction = _animSprite->attach<scene2::SpriteNode>(_animNode);
}

/**
 * Performs a film strip action
 *
 * @param action The film strip action
 * @param slide  The associated movement slide
 */
void Bomb::doStrip(cugl::ActionFunction action, float duration = DURATION) {

    if (_timeline->isActive(ACT_KEY)) {
        // NO OP
    } else {
        _timeline->add(ACT_KEY, action, duration);
    }
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

