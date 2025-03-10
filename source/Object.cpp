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

/** 
* This method returns the JSON encoding of the object for level creation purposes.
* NOTE: Currently, only std::string is supported for the first item in each pair.
		Only double, long, bool, and std::string are supported for the second item in each pair.
		This is because the JSONValue class only supports these key/value types.
		If you have an int, float, or anything else, you should convert it to one of the above types.
		ALSO, if you use a string literal as a VALUE (keys seem to be fine), you should cast it to an std::string.
		It will become a boolean in the JSON somehow if you don't.
*/
std::map<std::string, std::any> Object::getMap() {
	std::map<std::string, std::any> m = {
		{"x", double(_position.x)},
		{"y", double(_position.y)}
	};
	return m;
}