#include "ArtObject.h"
#include "Object.h"

#define JSON_KEY "artObjects"

using namespace cugl;
using namespace cugl::graphics;

void ArtObject::update(float timestep) {
    if (!_isAnimated) {
        doStrip(_animationAction, _animationDuration);
        _timeline->update(timestep);
    }
}

string ArtObject::getJsonKey() {
    return JSON_KEY;
}

void ArtObject::setAnimationDuration(float dur) {
    _animationDuration = dur;
}

void ArtObject::setAnimation(std::shared_ptr<scene2::SpriteNode> sprite) {
    _animateSpriteNode = sprite;

    _node = _animateSpriteNode;
    _animateSpriteNode->setVisible(true);
    //    _node->setScale(0.065f);

    _timeline = ActionTimeline::alloc();

    // Create the frame sequence
    // For an 8x8 spritesheet
    const int span = 32;

    std::vector<int> forward;
    for (int ii = 1; ii < span; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animation
    _animateSprite = AnimateSprite::alloc(forward);
    _animationAction = _animateSprite->attach<scene2::SpriteNode>(_animateSpriteNode);
}

/** Increments an animation film strip */
void ArtObject::doStrip(cugl::ActionFunction action, float duration) {
    if (_timeline->isActive(ACT_KEY)) {
        // NO OP
    }
    else {
        _timeline->add(ACT_KEY, action, duration);
    }
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
    _layer = layer;
    return ArtObject::init(pos, size, scale, angle, "default");
}

bool ArtObject::init(const Vec2 pos, const Size size, float scale, float angle, int layer, string jsonType) {
    _layer = layer;
    return ArtObject::init(pos, size, scale, angle, jsonType);
}

bool ArtObject::init(const Vec2 pos, const Size size, float scale, float angle, string jsonType) {
    _drawScale = scale;
    _position = pos;
    if (std::find(xOffsetArtObjects.begin(), xOffsetArtObjects.end(), jsonType) != xOffsetArtObjects.end()) {
        _position += Vec2(0.5, 0);
    }
    if (std::find(yOffsetArtObjects.begin(), yOffsetArtObjects.end(), jsonType) != yOffsetArtObjects.end()) {
        _position += Vec2(0, 0.5);
    }
    _size = size;
    _angle = angle;
    _itemType = Item::ART_OBJECT;
    _jsonType = jsonType;
    _itemType = jsonTypeToItemType[jsonType];
    CULog("jsonType is %s", jsonType.c_str());
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-0.5f, -1.0f), size*0.1);
    
    if (PolygonObstacle::init(rect)){

        setBodyType(b2_staticBody);
        setDebugColor(Color4::YELLOW);
        setPosition(_position + size/2);
        setAngle(angle);
    }

    return true;
}
/**
 * Sets the position
 *
 * @param position   The position
 */
void ArtObject::setPositionInit(const cugl::Vec2& position) {
    PolygonObstacle::setPosition(position);
    _position = position;
    _node->setPosition((position + _size / 2) * _drawScale);
}

void ArtObject::setLayer(int layer) {
    _node->setPriority(layer);
    _layer = layer;
}

void ArtObject::dispose() {
    Object::dispose();

    if (_node && _node->getParent()) {
        _node->removeFromParent();
        _node = nullptr;
    }
}

void ArtObject::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node, float angle) {
    _node = node;
    _node->setPosition(getPositionInit() * _drawScale);
    _node->setAngle(angle);
}

void ArtObject::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _node = node;
    _node->setPosition(getPositionInit() * _drawScale);
    _node->setAngle(0);
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
        {"type", std::string(_jsonType)},
    };
    return m;
}
