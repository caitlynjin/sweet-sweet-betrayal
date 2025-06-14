#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <cugl/cugl.h>
#include <any>
#include "Constants.h"

using namespace cugl;
using namespace Constants;

class Object : public physics2::PolygonObstacle {

protected:
	/** The position of the object */
	cugl::Vec2 _position;
	/** The texture that will be used to draw the object on screen */
	std::shared_ptr<graphics::Texture> _texture;
    /** The scene node of the object */
    std::shared_ptr<scene2::SceneNode> _sceneNode;
    /** The item type of this object */
    Item _itemType;
	/**The trajectory of the object-Should be unused by most objects, used by gust objects rn*/
	cugl::Vec2 _trajectory;
	/** Whether or not this object has been placed by a player */
	bool _playerPlaced;
	/** The type of this object in the json file
	* This allows different textures and sizes to be assigned to different "types" of the same object */
	std::string _jsonType;
    /** Size of the object */
    Size _size = Size(1, 1);
		//playerid owned
		int _ownerId = -1;

public:
#pragma mark -
#pragma mark Constructors

	Object(Vec2 pos, Item itemType, bool playerPlaced);

    Object(Vec2 pos, Item itemType);

	Object(Vec2 pos);

    Object() : PolygonObstacle() { }

    /**
     * Gets the position of the object.
     * @return the position of the object
     */
    const cugl::Vec2& getPositionInit() const { return _position; }


    /**
     * Get the size of the object.
     * @return the size of the object
     */
    const Size getSize() const { return _size; }

	/** Get the JSON type of the object.
	* @return the JSON type of the object
	*/
	const std::string& getJsonType() const { return _jsonType; }


	/**
	 * Gets the object texture.
	 * @return the texture of the object
	 */
	const std::shared_ptr<graphics::Texture>& getTexture() const { return _texture; }

	/**
	 * Sets the position
	 *
	 *
	 * @param position   The position
	 */
	virtual void setPositionInit(const cugl::Vec2& position);

	/**Gets the trajectory*/
	const cugl::Vec2& getTrajectory() const { return _trajectory; }

	/**Sets the trajectory*/
	void setTrajectory(cugl::Vec2 trajectory) { _trajectory = trajectory; };
	

	/**
	* Sets if the player placed this object
	*
	*
	* @param playerPlaced  Whether the player placed this object 
	*/
	void setPlayerPlaced(const bool playerPlaced);

	/**
	 * Sets the texture
	 *
	 * @param texture   The texture
	 */
	void setTexture(const std::shared_ptr<graphics::Texture>& texture);

    /**
     * Gets the scene node
     */
    std::shared_ptr<scene2::SceneNode> getSceneNode() { return _sceneNode; };

    /**
     * Sets the scene node
     */
    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node);


    /**
     * Get the item type of the object.
     */
    const Item getItemType() const {
        return _itemType;
    }

	void setItemType(Item itemType);
	/**
	* Gets whether this object was placed by a player.
	* @return whether this object was placed by a player
	*/
	const bool isPlayerPlaced() const {
		return _playerPlaced;
	}

	void setOwnerId(int ownerId) { _ownerId = ownerId; }
  int getOwnerId() const { return _ownerId; }


	/** Update method for this object. This will probably be different for each subclass. */
	virtual void update(float timestep);

	/**
	 * Destroys this Object, releasing all resources.
	 */

	virtual ~Object(void) { dispose(); }

	/** Returns the JSON key for this object 
	* All derived classes must override this
	*/

	virtual std::string getJsonKey();

	/**
	 * Disposes all resources and assets of this Object
	 *
	 * Any assets owned by this object will be immediately released.
	 */
	virtual void dispose();

	void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch,
		cugl::Size size);

	virtual std::map<std::string, std::any> getMap();

	friend bool operator==(Object self, Object other);

    /**
     * Sets whether the object is transparent.
     *
     * @param node      the object scene node
     * @param value     whether to set the object to transparent or not
     */
    void setGhost(const std::shared_ptr<cugl::scene2::SceneNode>& node, bool value);
};


#endif /* __OBJECT_H__ */
