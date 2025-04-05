#include "Mushroom.h"

#define JSON_KEY "mushroom"

using namespace cugl;
using namespace cugl::graphics;



void Mushroom::setPosition(const cugl::Vec2 &position){
    _position = position;
    _box->setPosition(position + _size / 2);
}

void Mushroom::dispose() {
    Object::dispose();

    _box->markRemoved(true);
    _box = nullptr;
}


#pragma mark -
#pragma mark Constructors


bool Mushroom::init(const Vec2 pos, const Size size, float scale) {
    _position = pos;
    _size = size;
    _itemType = Item::MUSHROOM;
    Size nsize = size;
    _box = cugl::physics2::BoxObstacle::alloc(pos + size/2, Size(nsize.width, nsize.height));
    _box->setSensor(true);
    return true;
}

// For networking: using an already-created BoxObstacle.
bool Mushroom::init(const Vec2 pos, const Size size, float scale, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
    _position = pos;
    _size = size;
    _box = box;
    _itemType = Item::MUSHROOM;
    return true;
}
