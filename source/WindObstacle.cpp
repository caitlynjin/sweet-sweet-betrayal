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
void WindObstacle::setPosition(const cugl::Vec2& position) {
    _position = position;
    _gust->setPosition(position + _size / 2);
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

    if (bd->getName() == "player") {

    }
    return bd->getName();
}

string WindObstacle::getJsonKey() {
    return JSON_KEY;
}

void WindObstacle::dispose() {}

using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 gust) {
    float _rayDist[RAYS + 1];
    float _playerDist[RAYS + 1];
    return WindObstacle::init(pos, size, gust, "default");
}
/**
 * Initializes a new WindObstacle at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool WindObstacle::init(const Vec2 pos, const Size size, const Vec2 gust, string jsonType) {
    float _rayDist[RAYS + 1];
    float _playerDist[RAYS + 1];

    Size nsize = size;
    _size = size;
    _itemType = WIND;
    _position = pos;
    _size = size;
    _jsonType = jsonType;

    _gust = cugl::physics2::BoxObstacle::alloc(pos + _size/2, nsize);
    _gust->setDensity(0.0f);
    _gust->setFriction(0.0f);
    _gust->setRestitution(0.0f);
    _gust->setSensor(true);
    _gust->setName("gust");
    
    /**Intialize wind specific variables*/

    for (int it = 0; it != RAYS; it++) {
        Vec2 origin = pos + Vec2(0, 0.1f + _size.height / 2) + Vec2((_size.width/2)*(RAYS-2*it)/RAYS, 0);

        _rayOrigins.push_back(origin);
    }

    _gustDir = gust;



    setTrajectory(Vec2(0, 15.0f));
    return true;
    
}

std::map<std::string, std::any> WindObstacle::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"gustDirX", double(_gustDir.x)},
        {"gustDirY", double(_gustDir.y)},
         {"type", std::string(_jsonType)}
    };
    return m;
}

