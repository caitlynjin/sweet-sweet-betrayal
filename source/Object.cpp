#include "Object.h"

using namespace cugl;
using namespace cugl::graphics;

Object::Object(Vec2 pos) {
	_position = pos;
	_texture = nullptr;
}

Object::Object() {
	_position = Vec2(0, 0);
	_texture = nullptr;
}

void Object::update(float timestep) { }

void Object::setTexture(const std::shared_ptr<graphics::Texture>& texture) {
	_texture = texture;
}

void Object::setPosition(const cugl::Vec2& position) {
	_position = position;
}

void Object::dispose() {
	_texture = nullptr;
}

void Object::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch,
	cugl::Size size) {

}