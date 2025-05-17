#include "WindObstacle.h"
#include "Object.h"

#define JSON_KEY "windObstacles"
using namespace cugl;
using namespace cugl::graphics;

/**
 * Sets the position
 *
 * @param position   The position
 */
void WindObstacle::setPositionInit(const cugl::Vec2& position) {
    PolygonObstacle::setPosition((position)*_drawScale);
    _position = position;
    

    // Update ray positions
    setRayOrigins();
}

void WindObstacle::update(float timestep) {
    PolygonObstacle::update(timestep);
    
    _playerHits = 0;
    _currentPlayerDist = 2.0f;
    _prevRayDist = _minRayDist;
    _minRayDist = 2.0f;
    

    for (auto it = 0; it != RAYS; ++it) {
        if (_playerDist[it]<_rayDist[it]) {
            _playerHits++;
            _currentPlayerDist = min(_playerDist[it], _currentPlayerDist);
        }
        _minRayDist = min(_rayDist[it], _minRayDist);
        
    }
    CULog("playerdist %f", _currentPlayerDist);
    CULog("raydist %f", _minRayDist);
    CULog("pos %f", _position.x);

    /*Reset all the arrays**/
    std::fill(_playerDist, _playerDist+RAYS,600);
    std::fill(_rayDist, _rayDist + RAYS, 600);
    
    updateAnimation(timestep);
    
    
}

void WindObstacle::updateAnimation(float timestep) {
    
    if (!_fanTimeline->isActive("current")) {
        _fanTimeline->add("current", _fanAction, FAN_ANIM_CYCLE);
    }
    _fanTimeline->update(timestep);
    //we need this piece of shit variable becasue for some fucking reason raycasting only works every other call?
    //Seriously what the fuck
    float actualMin = min(_prevRayDist, _minRayDist);

    if (_gustTimeline4 != nullptr) {
        if (!_gustTimeline4->isActive("current")) {
            _gustTimeline4->add("current", _gustAction4, GUST_ANIM_CYCLE);
            _gustTimeline3->add("current", _gustAction3, GUST_ANIM_CYCLE);
            _gustTimeline2->add("current", _gustAction2, GUST_ANIM_CYCLE);
            _gustTimeline1->add("current", _gustAction1, GUST_ANIM_CYCLE);
            _gustSpriteNode2->setVisible(false);
            _gustSpriteNode3->setVisible(false);
            _gustSpriteNode1->setVisible(false);
        }
        _gustSpriteNode2->setVisible(false);
        _gustSpriteNode3->setVisible(false);
        _gustSpriteNode1->setVisible(false);
        _gustSpriteNode4->setVisible(false);

        if (actualMin >= 1.0f) {
            _gustSpriteNode4->setVisible(true);
        }
        else if (actualMin >= 0.75f) {
            _gustSpriteNode3->setVisible(true);
        }
        else if (actualMin >= 0.25f) {
            _gustSpriteNode2->setVisible(true);
        }
        else {
            _gustSpriteNode1->setVisible(true);
        }

        _gustTimeline4->update(timestep);
        _gustTimeline3->update(timestep);
        _gustTimeline2->update(timestep);
        _gustTimeline1->update(timestep);
    }
}
void WindObstacle::setRayOrigins() {
    // Update ray positions
    _rayOrigins.clear();
    //Account for the angle.
    float rayDiff = (_size.width - 2 * OFFSET) / (RAYS * _size.width);
    Vec2 diffVec = Vec2();
    Vec2 posDiff = Vec2(OFFSET, OFFSET);
    Vec2 posAdjust = Vec2();
    //We need to offset the position if we have a different angle. seems a bit overkill?
    if (_angle <= 0) {
        diffVec = Vec2(rayDiff, 0);
        
    }
    else if (_angle <= (M_PI / 2 + 0.1f)) {
        diffVec = Vec2(0, -rayDiff);
        posDiff.y = 1 - OFFSET;
        posAdjust = Vec2(-1,0);
        _anchorOffset = Vec2(0, 0.2);
        _fanOffset = Vec2(0, 1.0);
    }
    else if (_angle <= M_PI+0.1f) {
        diffVec = Vec2(-rayDiff,0);
        posDiff.x = 1 - OFFSET;
        posAdjust = Vec2(-1, 1);
        _anchorOffset = Vec2(1.0, 0.2);
        _fanOffset = Vec2(1.0, 1.0);
    }
    else{
        diffVec = Vec2(0,-rayDiff);
        posDiff.y = 1 - OFFSET;
        posDiff.x = 1 - OFFSET;
        posAdjust = Vec2(0, 1);
        _anchorOffset = Vec2(1.0,0.0f);
        _fanOffset = Vec2(1.0f, 0.0);
    }
    

    for (int it = 0; it != RAYS; it++) {
        Vec2 origin = _position + posDiff + diffVec * it;
            //Vec2(rayDiff * it, 0);
        _rayOrigins.push_back(origin);
        CULog("Origin.x %f", origin.x);
        CULog("Origin.y %f", origin.y);
    }
    
}

string WindObstacle::ReportFixture(b2Fixture* contact, const Vec2& point, const Vec2& normal, float fraction) {
    b2Body* body = contact->GetBody();
    std::string* fd = reinterpret_cast<std::string*>(contact->GetUserData().pointer);
    physics2::Obstacle* bd = reinterpret_cast<physics2::Obstacle*>(body->GetUserData().pointer);
    return bd->getName();
}

string WindObstacle::getJsonKey() {
    return JSON_KEY;
}

void WindObstacle::dispose() {
    Object::dispose();

    CULog("Diposing wind");
    markRemoved(true);

    if (_node && _node->getParent()) {
        _node->removeFromParent();
        _node = nullptr;
    }
}

using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool WindObstacle::init(const Vec2 pos, const Size size, float scale, const Vec2 windDirection, const Vec2 windStrength, const float angle) {
    return WindObstacle::init(pos, size, scale, windDirection, windStrength, angle, "default");
}
/**
 * Initializes a new WindObstacle at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param windDirection the endpoint of the wind projected out-determines both range and direction
 * @param windStrength How strongly the wind blows the player.
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool WindObstacle::init(const Vec2 pos, const Size size, float scale, const Vec2 windDirection, const Vec2 windStrength, const float angle, string jsonType) {
    float _rayDist[RAYS + 1];
    float _playerDist[RAYS + 1];

    Size nsize = size;
    _size = size;
    _itemType = WIND;
    _position = pos;
    _size = size;
    _jsonType = jsonType;
    _angle = angle;

    _drawScale = 1.0f;
    _minRayDist = 2.0f;
    
    /**Intialize wind specific variables*/
    /**Here we intialize the origins of the ray tracers*/
    

    /*Here we intialize the direction and trajectory*/
    _windDirection = windDirection;
    _windForce = windStrength;

    setRayOrigins();
    //Intialize the 'fan' component of the windbostacle
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(0.5f,0.5f), size);
    
    

    if (PolygonObstacle::init( rect, Vec2(0.5f, 0.5f))){
        setPosition(pos);
        
        setDensity(0.0f);
        setFriction(0.0f);
        setRestitution(0.0f);
        setSensor(true);
        setAngle(_angle);
        
        _node = scene2::SpriteNode::alloc();
        _node->setPriority(PRIORITY);
        return true;
    }
    return false;
}

std::map<std::string, std::any> WindObstacle::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"scale", double(_drawScale)},
        {"gustDirX", double(_windDirection.x)},
        {"gustDirY", double(_windDirection.y)},
        {"gustForceX", double(_windForce.x)},
        {"gustForceY", double(_windForce.y)},
        {"angle", double(_angle)},
         {"type", std::string(_jsonType)}
    };
    return m;
}
void WindObstacle::setGustAnimation(std::vector<std::shared_ptr<scene2::SpriteNode>> sprite, int nFrames) {
    //Create and iterate through all our animations
    if (!_node) {
        _node = scene2::SceneNode::alloc();
        _node->setPriority(PRIORITY);
    }
    //GustSpriteNode4
    std::vector<int> forward;
    _gustSpriteNode4 = sprite[3];
    _gustSpriteNode4->setVisible(true);
    _gustSpriteNode4->setPriority(PRIORITY);

    _gustSpriteNode4->setAnchor(_anchorOffset);
    _gustSpriteNode4->setPosition(_animationOffest);
    
    _node->addChild(_gustSpriteNode4);
    //Create the spritesheet
    _gustTimeline4 = ActionTimeline::alloc();
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    forward.push_back(0);
    _gustAnimateSprite4 = AnimateSprite::alloc(forward);
    _gustAction4 = (_gustAnimateSprite4->attach<scene2::SpriteNode>(_gustSpriteNode4));
    //GustSpriteNode3
    forward.clear();
    _gustSpriteNode3 = sprite[2];
    _gustSpriteNode3->setPriority(PRIORITY);

    _gustSpriteNode3->setAnchor(_anchorOffset);
    _gustSpriteNode3->setPosition(_animationOffest);
    
    _node->addChild(_gustSpriteNode3);
    _gustTimeline3 = ActionTimeline::alloc();
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    forward.push_back(0);
    _gustAnimateSprite3 = AnimateSprite::alloc(forward);
    _gustAction3 = (_gustAnimateSprite3->attach<scene2::SpriteNode>(_gustSpriteNode3));
    //GustSpriteNode2
    forward.clear();
    _gustSpriteNode2 = sprite[1];
    _gustSpriteNode2->setPriority(PRIORITY);

    _gustSpriteNode2->setAnchor(_anchorOffset);
    _gustSpriteNode2->setPosition(_animationOffest);
    
    _node->addChild(_gustSpriteNode2);
    _gustTimeline2 = ActionTimeline::alloc();
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    forward.push_back(0);
    _gustAnimateSprite2 = AnimateSprite::alloc(forward);
    _gustAction2 = (_gustAnimateSprite2->attach<scene2::SpriteNode>(_gustSpriteNode2));
    forward.clear();
    _gustSpriteNode1 = sprite[0];
    _gustSpriteNode1->setPriority(PRIORITY);

    _gustSpriteNode1->setAnchor(_anchorOffset);
    _gustSpriteNode1->setPosition(_animationOffest);
    
    _node->addChild(_gustSpriteNode1);
    _gustTimeline1 = ActionTimeline::alloc();
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    forward.push_back(0);
    _gustAnimateSprite1 = AnimateSprite::alloc(forward);
    _gustAction1 = (_gustAnimateSprite1->attach<scene2::SpriteNode>(_gustSpriteNode1));
}

/** Sets the fan animation and adds the fan sprite node to the scene node (_node) */
void WindObstacle::setFanAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames) {
    //Create sprite object
    _fanSpriteNode = sprite;
    _fanSpriteNode->setVisible(true);
    _fanSpriteNode->setPriority(PRIORITY);

    _fanSpriteNode->setAnchor(_fanOffset);
    _fanSpriteNode->setPosition(_animationOffest);
    if (!_node) {
        _node = scene2::SceneNode::alloc();
        _node->setPriority(PRIORITY);
    }
    _node->addChild(_fanSpriteNode);
    
    //Create the spritesheet
    _fanTimeline = ActionTimeline::alloc();

    std::vector<int> forward;
    for (int ii = 1; ii < nFrames; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);

    // Create animations
    _fanAnimateSprite = AnimateSprite::alloc(forward);
    _fanAction = _fanAnimateSprite->attach<scene2::SpriteNode>(_fanSpriteNode);
}

