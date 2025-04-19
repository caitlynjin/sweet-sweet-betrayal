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
    Size nsize = size;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(Vec2(-1.0f, -0.5f), Size(nsize.width*0.8, nsize.height));

    if (PolygonObstacle::init(rect)){
        setPosition(pos + size/2);
        setSensor(true);
        return true;
    }

    return false;
}
