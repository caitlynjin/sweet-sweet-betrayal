#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <cugl/cugl.h>
#include <any>
#include "Constants.h"

using namespace cugl;
using namespace Constants;

class Object {

protected:
	/** The position of the object */
	cugl::Vec2 _position;
	/** The texture that will be used to draw the object on screen */
	std::shared_ptr<graphics::Texture> _texture;
    /** The item type of this object */
    Item _itemType;
	/** Whether or not this object has been placed by a player */
	bool _playerPlaced;
    /** Size of the object */
    Size _size = Size(1, 1);

public:
#pragma mark -
#pragma mark Constructors

	Object(Vec2 pos, Item itemType, bool playerPlaced);

    Object(Vec2 pos, Item itemType);

	Object(Vec2 pos);

	Object();

    /**
     * Gets the position of the object.
     * @return the position of the object
     */
    const cugl::Vec2& getPosition() const { return _position; }


    /**
     * Get the size of the object.
     * @return the size of the object
     */
    const Size getSize() const { return _size; }


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
	virtual void setPosition(const cugl::Vec2& position);

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
     * Gets the obstacle of this object.
     */
    virtual std::shared_ptr<cugl::physics2::Obstacle> getObstacle() { return nullptr; };

    /**
     * Get the item type of the object.
     */
    const Item getItemType() const {
        return _itemType;
    }
	/**
	* Gets whether this object was placed by a player.
	* @return whether this object was placed by a player
	*/
	const bool isPlayerPlaced() const {
		return _playerPlaced;
	}


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
	void dispose();

	void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch,
		cugl::Size size);

	std::map<std::string, std::any> getMap();
};


#endif /* __OBJECT_H__ */
