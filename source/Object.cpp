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
    _sceneNode = nullptr;
}
Object::Object(Vec2 pos, Item itemType) {
    _position = pos;
    _texture = nullptr;
    _itemType = itemType;
    _sceneNode = nullptr;
}

Object::Object(Vec2 pos) {
	_position = pos;
	_texture = nullptr;
    _itemType = NONE;
    _sceneNode = nullptr;
}

//Object::Object() {
//	_position = Vec2(0, 0);
//	_texture = nullptr;
//    _itemType = NONE;
//    _sceneNode = nullptr;
//}

void Object::update(float timestep) { }

void Object::setTexture(const std::shared_ptr<graphics::Texture>& texture) {
	_texture = texture;
}

void Object::setPositionInit(const cugl::Vec2& position) {
	_position = position;
}

void Object::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _sceneNode = node;
}

void Object::setItemType(Item itemType)
{
	_itemType = itemType;
}


void Object::setPlayerPlaced(const bool playerPlaced) {
	_playerPlaced = playerPlaced;
}

void Object::dispose() {
	_texture = nullptr;

    if (_sceneNode && _sceneNode->getParent()) {
        _sceneNode->removeFromParent();
        _sceneNode = nullptr;
    }
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

bool operator==(Object self, Object other) {
	return self.getPositionInit() == other.getPositionInit() && self.getSize() == other.getSize();
}

/**
 * Sets whether the object is transparent.
 *
 * @param node      the object scene node
 * @param value     whether to set the object to transparent or not
 */
void Object::setGhost(const std::shared_ptr<cugl::scene2::SceneNode>& node, bool value) {
    if (node) {
        if (value) {
            // Set transparent
            Color4 color = node->getColor();
            color.a = 150;
            node->setColor(color);
        } else {
            // Restore color
            Color4 color = node->getColor();
            color.a = 255;
            node->setColor(color);
        }
        
        for (const auto& child : node->getChildren()) {
            setGhost(child, value);
        }
    }
}
