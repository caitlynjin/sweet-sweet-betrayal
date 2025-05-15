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
    _jsonType = jsonType;
    _drawScale = scale;
    _justInit = true;
    
    PolyFactory factory;
    Poly2 circle = factory.makeCircle(Vec2(0,0), RADIUS);
    
    if (PolygonObstacle::init(circle)) {
        setName("projectile");
        setDebugColor(Color4::YELLOW);
        setDensity(DENSITY);
        setPosition(pos);
        setBodyType(b2_dynamicBody);
        setFixedRotation(true);
        setLinearDamping(DAMPING);
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
    if (_justInit) {
        _body->ApplyLinearImpulseToCenter(b2Vec2(INIT_IMPULSE, 1.0f), true);
        _justInit = false;
    }
    
    if (_exploding) {
        _explosionCountdown += timestep;
    }
    //Run if the projectile isn't being frozen due to it exploding.
    else {
        PolygonObstacle::update(timestep);
        b2Vec2 force(12.5f, UP_FORCE);
        _body->ApplyForceToCenter(force, true);
        _projectileSprite->setAngle(_projectileSprite->getAngle() + (timestep * 5 / 2 * M_PI));
    }
}

void Projectile::hostUpdate(float timestep) {

    /*if (_justInit) {
        _body->ApplyLinearImpulseToCenter(b2Vec2(INIT_IMPULSE, 1.0f), true);
        _justInit = false;
    }
    b2Vec2 force(12.5f, UP_FORCE);
    _body->ApplyForceToCenter(force, true);
    _projectileSprite->setAngle(_projectileSprite->getAngle() + (timestep * 5 / 2 * M_PI));*/
}

void Projectile::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position);

    if (_node != nullptr) {
        _node->setPosition((position));
    }
}

void Projectile::setTextureNode(std::shared_ptr<cugl::scene2::SpriteNode> sprite) {

    //Create and iterate through all our animations
    if (!_node) {
        _node = scene2::SceneNode::alloc();
    }
    //GustSpriteNode4
    sprite->setVisible(true);
    sprite->setPriority(0);
    sprite->setAnchor(0.5f, 0.5f);
    sprite->setPosition(-13.0f, 0.0f);

    _node->addChild(sprite);

    _projectileSprite = sprite;
    
}

void Projectile::detonate() {
    _exploding = true;
    
    setSensor(true);
    
}

string Projectile::getJsonKey() {
    return JSON_KEY;
}

void Projectile::dispose() {
    _node->dispose();
}

