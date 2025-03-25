#include "Mushroom.h"

#define JSON_KEY "mushroom"

using namespace cugl;
using namespace cugl::graphics;



void Mushroom::setPosition(const cugl::Vec2 &position){
  _position = position;
  _box->setPosition(position);
}

void Mushroom::dispose() {}


#pragma mark -
#pragma mark Constructors


bool Mushroom::init(const Vec2 pos, const Size size, float scale) {
  _position = pos;         
  _size = size;     
  _itemType = Item::MUSHROOM;    
  Size nsize = size;
  _box = cugl::physics2::BoxObstacle::alloc(pos, Size(nsize.width, nsize.height));
  _box->setSensor(true);
  return true;
}

// For networking: using an already-created BoxObstacle.
bool Mushroom::init(const Vec2 pos, const Size size, float scale, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
  _position = pos;
  _size = size;
  _box = box;   
  return true;
}
