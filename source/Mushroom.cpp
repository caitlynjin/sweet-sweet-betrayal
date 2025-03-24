#include "Mushroom.h"

#define JSON_KEY "mushroom"

using namespace cugl;
using namespace cugl::graphics;



void Mushroom::setPosition(const cugl::Vec2 &position){
    _box->setPosition(position);
    if (_node != nullptr) {
        _node->setPosition(position*_drawScale);
    }
}

void Mushroom::dispose() {}


#pragma mark -
#pragma mark Constructors


bool Mushroom::init(const Vec2 pos, const Size size, float scale) {
  _mushroomTexture = "";   
  _position = pos;         
  _size = size;         
  _drawScale = scale;
  _box = cugl::physics2::BoxObstacle::alloc(pos, size);
  _box->setSensor(true);
  return true;
}

// For networking: using an already-created BoxObstacle.
bool Mushroom::init(const Vec2 pos, const Size size, float scale, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
  _mushroomTexture = "";
  _position = pos;
  _size = size;
  _drawScale = scale;
  _box = box;   
  return true;
}
