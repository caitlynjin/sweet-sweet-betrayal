#include "WindObstacle.h"
#include "Object.h"

#define JSON_KEY "windObstacles"


using namespace cugl;
using namespace cugl::graphics;


#define OFFSET 0.1f
/**
 * Sets the position
 *
 * @param position   The position
 */
void WindObstacle::setPositionInit(const cugl::Vec2& position) {
    _position = position;
    PolygonObstacle::setPosition(position + _size / 2);
}

void WindObstacle::update(float timestep) {
    _playerHits = 0;

    for (auto it = 0; it != RAYS; ++it) {
        if (_playerDist[it]<_rayDist[it]) {
            _playerHits++;
        }
    }
    /*Reset all the arrays**/
    std::fill(_playerDist, _playerDist+RAYS,600);
    std::fill(_rayDist, _rayDist + RAYS, 600);
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

    markRemoved(true);
//    _gust = nullptr;
}

using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 windDirection, const Vec2 windStrength) {
    return WindObstacle::init(pos, size, windDirection, windStrength, "default");
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
bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 windDirection, const Vec2 windStrength, string jsonType) {
    float _rayDist[RAYS + 1];
    float _playerDist[RAYS + 1];

    Size nsize = size;
    _size = size;
    _itemType = WIND;
    _position = pos;
    _size = size;
    _jsonType = jsonType;
    
    /**Intialize wind specific variables*/
    /**Here we intialize the origins of the ray tracers*/
    for (int it = 0; it != RAYS; it++) {
        Vec2 origin = pos + Vec2(OFFSET, OFFSET + _size.height / 2) + 
            ((size.width-2*OFFSET)/(_size.width))*Vec2((_size.width/2)*(RAYS-2*it)/RAYS, 0);

        _rayOrigins.push_back(origin);
    }
    /*Here we intialize the direction and trajectory*/
    _windDirection = windDirection;
    _windForce = windStrength;
    setTrajectory(Vec2(0, 3.0f));
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(pos + _size/2, nsize);
        
    if (PolygonObstacle::init(rect)){
        setDensity(0.0f);
        setFriction(0.0f);
        setRestitution(0.0f);
        setName("gust");
        return true;
    }
    
    
    return false;
    /*Finally intialize the wind gust effect**/
    
}

void WindObstacle::setGustSprite(std::shared_ptr<scene2::SpriteNode> gustSprite) {
    _windNode = gustSprite;
    _windNode->setAnchor(0.0f, 0.0f);
    _windNode->setPosition(Vec2());
    _sceneNode->addChild(_windNode);
    _windNode->setVisible(true);
}

std::map<std::string, std::any> WindObstacle::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"gustDirX", double(_windForce.x)},
        {"gustDirY", double(_windForce.y)},
         {"type", std::string(_jsonType)}
    };
    return m;
}

