#include "Treasure.h"
//#include "Object.h"

#define JSON_KEY "treasures"

using namespace cugl;
using namespace cugl::graphics;


void Treasure::update(float timestep) {
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
    
    if (_stealCooldown > 0){
        _stealCooldown -= 0.1f;
        _node->setColor(Color4::GREEN);
    }
    else{
        _node->setColor(Color4::WHITE);
    }
}

string Treasure::getJsonKey() {
    return JSON_KEY;
}

void Treasure::setPositionInit(const cugl::Vec2 &position){
    _position = position;
    PolygonObstacle::setPosition(position);
    
    if (_node != nullptr) {
        _node->setPosition(position*_drawScale);
    }
}

void Treasure::dispose() {}



using namespace cugl;

#pragma mark -
#pragma mark Constructors


/**
 * Initializes a new Treasure at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Treasure::init(const Vec2 pos, const Size size, float scale) {
    return Treasure::init(pos, size, scale, "default");
}
bool Treasure::init(const Vec2 pos, const Size size, float scale, string jsonType) {
    Size nsize = size;
    _treasureTexture = "";
    _position = pos;
    _size = size;
    _jsonType = jsonType;
    _drawScale = scale;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(), nsize*0.5);
    
    if (PolygonObstacle::init(rect)){
        setSensor(true);
        setName("treasure");
        setDebugColor(Color4::YELLOW);
        setPosition(pos);
        _node = scene2::SpriteNode::alloc();
        
        return true;
    }

    return false;
}
bool Treasure::init(const Vec2 pos, const Size size, float scale,bool taken, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
//    Size nsize = size;
    _treasureTexture = "";
    _position = pos;
    _size = size;
    _jsonType = "default";
    _drawScale = scale;
    _taken = taken;
    _box = box;
    return true;
}

void Treasure::reset(){
    _taken = false;
    _isStealable = true;
}

std::map<std::string, std::any> Treasure::getMap() {
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


