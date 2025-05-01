#include "Mushroom.h"
#define JSON_KEY "mushroom"

using namespace cugl;
using namespace cugl::graphics;

void Mushroom::setPositionInit(const Vec2 &position) {
    _position = position;
    PolygonObstacle::setPosition(position + _size * 0.5f);
}

void Mushroom::dispose() {
    Object::dispose();
    markRemoved(true);
}

bool Mushroom::init(const Vec2 pos, const Size size, float scale) {
    _position = pos;
    _size     = size;
    _drawScale = scale;
    _itemType = Item::MUSHROOM;

    // Define pickup region
    PolyFactory factory;
    Poly2 rect = factory.makeRect(
        Vec2(-size.width * 0.4f, -size.height * 0.5f),
        Size(size.width * 0.8f, size.height)
    );
    if (!PolygonObstacle::init(rect)) {
        return false;
    }

    setPosition(pos + size * 0.5f);
    setSensor(true);
    setDebugColor(Color4::YELLOW);

    _node = scene2::SpriteNode::alloc();
    _node->setPosition(getPosition() * _drawScale);

    return true;
}

void Mushroom::setMushroomAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _mushroomSpriteNode = sprite;
    _mushroomSpriteNode->setAnchor(0.0f, 0.0f);
    _mushroomSpriteNode->setPosition(Vec2());
    _mushroomSpriteNode->setVisible(true);

    if (!_node) {
        _node = scene2::SpriteNode::alloc();
    } else {
        _node->removeAllChildren();
    }
    _node->addChild(_mushroomSpriteNode);

    std::vector<int> frames;
    frames.reserve(nFrames);
    for (int i = 1; i < nFrames; ++i) {
        frames.push_back(i);
    }
    frames.push_back(0);

    _mushroomTimeline      = ActionTimeline::alloc();
    _mushroomAnimateSprite = AnimateSprite::alloc(frames);
    _mushroomAction        = _mushroomAnimateSprite->attach<scene2::SpriteNode>(_mushroomSpriteNode);
}

void Mushroom::updateAnimation(float timestep) {
    if (_animating) {
        _mushroomTimeline->update(timestep);
        //end of 1 cycle
        if (!_mushroomTimeline->isActive("current")) {
            _animating = false;
            _mushroomSpriteNode->setFrame(0);
        }
    }
}

void Mushroom::update(float timestep) {
    PolygonObstacle::update(timestep);

    if (_node) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }
    if (!_animating) {
        _mushroomSpriteNode->setFrame(0);
    }
    updateAnimation(timestep);
}

void Mushroom::triggerAnimation() {
    CULog("triggerAnimation");
    _mushroomTimeline->remove("current");
    _animating = true;
    _mushroomTimeline->add("current", _mushroomAction, 1.0f);
}



