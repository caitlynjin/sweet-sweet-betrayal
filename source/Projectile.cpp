#include "Projectile.h"

#define JSON_KEY "PROJECTILE"

using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a new Projectile at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Projectile::init(const Vec2 pos, const Size size, float scale) {
    return Projectile::init(pos, size, scale, "default");
}
bool Projectile::init(const Vec2 pos, const Size size, float scale, string jsonType) {
    Size nsize = size;
    _ProjectileTexture = "";
    _position = pos;
    _size = size;
    _jsonType = jsonType;
    //_itemType = Item::Projectile;
    _drawScale = scale;

    PolyFactory factory;
    Poly2 circle = factory.makeCircle(pos, 0.5f);

    if (PolygonObstacle::init(circle)) {
        setName("Projectile");
        setDebugColor(Color4::YELLOW);
        setPosition(pos + size / 2);
        setBodyType(b2_dynamicBody);
        _node = scene2::SpriteNode::alloc();

        return true;
    }

    return false;
}

std::map<std::string, std::any> Projectile::getMap() {
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

void Projectile::update(float timestep) {
    //    Vec2 currPos = getPosition();
    //    currPos += Vec2(1,1);
    //    setPosition(currPos);
    //    _box->setPosition(currPos);

    //    if (_node != nullptr) {
    //        _node->setPosition(getPosition()*_drawScale);
    //    }

    PolygonObstacle::update(timestep);
    if (_node != nullptr)
    {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

    //updateAnimation(timestep);
}

void Projectile::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position + _size / 2);

    if (_node != nullptr) {
        _node->setPosition((position + _size / 2) * _drawScale);
    }
}

string Projectile::getJsonKey() {
    return JSON_KEY;
}

void Projectile::updateAnimation(float timestep) {
    doStrip(_spinAction, DURATION);
    _timeline->update(timestep);
}

/**
 * Performs a film strip action
 *
 * @param action The film strip action
 * @param slide  The associated movement slide
 */
void Projectile::doStrip(cugl::ActionFunction action, float duration = DURATION) {

    if (_timeline->isActive(ACT_KEY)) {
        // NO OP
    }
    else {
        _timeline->add(ACT_KEY, action, duration);
    }
}

void Projectile::setAnimation(std::shared_ptr<scene2::SpriteNode> sprite) {
    _spinSpriteNode = sprite;

    _node = _spinSpriteNode;
    _spinSpriteNode->setVisible(true);

    _timeline = ActionTimeline::alloc();

    // Create the frame sequence
    const int span = 64;
    std::vector<int> forward;
    for (int ii = 1; ii < span; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animation
    _spinAnimateSprite = AnimateSprite::alloc(forward);
    _spinAction = _spinAnimateSprite->attach<scene2::SpriteNode>(_spinSpriteNode);
}

void Projectile::dispose() {
    _node->dispose();
}

