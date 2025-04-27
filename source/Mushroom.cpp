#include "Mushroom.h"

#define JSON_KEY "mushroom"

using namespace cugl;
using namespace cugl::graphics;



void Mushroom::setPositionInit(const cugl::Vec2 &position){
    _position = position;
    PolygonObstacle::setPosition(position + _size/2);
}

void Mushroom::dispose() {
    Object::dispose();
    markRemoved(true);
}


#pragma mark -
#pragma mark Constructors


bool Mushroom::init(const Vec2 pos, const Size size, float scale) {
    _position = pos;
    _size = size;
    _itemType = Item::MUSHROOM;
    _drawScale = scale;
    Size nsize = size;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.0f, -0.5f), Size(nsize.width*0.8, nsize.height));

    if (PolygonObstacle::init(rect)) {
        setPosition(pos + size/2);
        setSensor(true);
        setSceneNode(scene2::SceneNode::alloc());
        setDebugColor(Color4::YELLOW);
        return true;
    }

    return false;
}


#pragma mark -
#pragma mark Animations

void Mushroom::setMushroomAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    _mushroomSpriteNode = sprite;
    _mushroomSpriteNode->setAnchor(0.0f,0.0f);
    _mushroomSpriteNode->setPosition(Vec2());
    _mushroomSpriteNode->setVisible(true);
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }
    _node->addChild(_mushroomSpriteNode);
    std::vector<int> frames;
    frames.reserve(nFrames+1);
    for (int i = 0; i < nFrames; ++i) frames.push_back(i);
    frames.push_back(0);
    _mushroomTimeline = ActionTimeline::alloc();
    _mushroomAnimateSprite = AnimateSprite::alloc(frames);
    _mushroomAction = _mushroomAnimateSprite->attach<scene2::SpriteNode>(_mushroomSpriteNode);
}

void Mushroom::updateAnimation(float timestep) {
    if (!_mushroomTimeline->isActive("current")) {
        _mushroomTimeline->add("current", _mushroomAction, 1.0f);
    }
    _mushroomTimeline->update(timestep);
}

void Mushroom::update(float timestep) {
    PolygonObstacle::update(timestep);
    if (_node) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }
    updateAnimation(timestep);
}


