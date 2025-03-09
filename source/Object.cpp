#include "Object.h"
#include "Constants.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace Constants;

#pragma mark -
#pragma mark Constructors

Object::Object(Vec2 pos, Item itemType, bool playerPlaced) {
	_position = pos;
	_texture = nullptr;
	_itemType = itemType;
	_playerPlaced = playerPlaced;
}
Object::Object(Vec2 pos, Item itemType) {
    _position = pos;
    _texture = nullptr;
    _itemType = itemType;
}

Object::Object(Vec2 pos) {
	_position = pos;
	_texture = nullptr;
    _itemType = NONE;
}

Object::Object() {
	_position = Vec2(0, 0);
	_texture = nullptr;
    _itemType = NONE;
}

void Object::update(float timestep) { }

void Object::setTexture(const std::shared_ptr<graphics::Texture>& texture) {
	_texture = texture;
}

void Object::setPosition(const cugl::Vec2& position) {
	_position = position;
}

void Object::setPlayerPlaced(const bool playerPlaced) {
	_playerPlaced = playerPlaced;
}

void Object::dispose() {
	_texture = nullptr;
}

std::string Object::getJsonKey() {
	return "objects";
}

void Object::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch,
	cugl::Size size) {

}

std::map<std::string, double> Object::getMap() {
	std::map<std::string, double> m = {
		{"x", _position.x},
		{"y", _position.y}
	};
	return m;
}
