#include "Mushroom.h"
#define JSON_KEY "mushroom"

using namespace cugl;
using namespace cugl::graphics;

void Mushroom::setPositionInit(const Vec2 &position) {
    _position = position;
    PolygonObstacle::setPosition(position + _size/2);
}

void Mushroom::dispose() {
    Object::dispose();
    markRemoved(true);
}

bool Mushroom::init(const Vec2 pos, const Size size, std::string jsonType) {
    _position = pos;
    _size     = size;
    _jsonType = jsonType;
    _itemType = Item::MUSHROOM;

    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.0f, -0.5f), size);
    if (!PolygonObstacle::init(rect)) {
        return false;
    }

    setPosition(pos + size * 0.5f);
    setSensor(false);

    return true;
}

bool Mushroom::init(const Vec2 pos, const Size size, float scale) {
    _position = pos;
    _size     = size;
    _drawScale = scale;
    _itemType = Item::MUSHROOM;

    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.0f, -0.5f), size);
    if (!PolygonObstacle::init(rect)) {
        return false;
    }

    setPosition(pos + size * 0.5f);
    setSensor(false);

    return true;
}

void Mushroom::setMushroomAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _mushroomSpriteNode = sprite;
    _mushroomSpriteNode->setPosition(Vec2());
    _mushroomSpriteNode->setVisible(true);

    if (!_sceneNode) {
        _sceneNode = scene2::SpriteNode::alloc();
    } else {
        _sceneNode->removeAllChildren();
    }
    _sceneNode->addChild(_mushroomSpriteNode);

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

void Mushroom::updateAnimation(float dt) {
    if (!_shouldAnimate) {
        _mushroomSpriteNode->setFrame(0);
        return;
    }

    // start action if it’s not already running
    if (!_mushroomTimeline->isActive("current")) {
        _mushroomTimeline->add("current", _mushroomAction, 1.0f);
    }
    _mushroomTimeline->update(dt);

    // only plays one cycle and sets back to frame 0
    if (!_mushroomTimeline->isActive("current")) {
        _shouldAnimate = false;
        _mushroomSpriteNode->setFrame(0);
    }
}

void Mushroom::update(float timestep) {
    PolygonObstacle::update(timestep);
    updateAnimation(timestep);
}



