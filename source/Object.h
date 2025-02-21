#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <cugl/cugl.h>

using namespace cugl;

class Object {

public:

	/** The position of the object */
	cugl::Vec2 _position;
	/** The texture that will be used to draw the object on screen */
	std::shared_ptr<graphics::Texture> _texture;

	/** 
	* Gets the position of the object.
	* @return the position of the object 
	*/
	const cugl::Vec2& getPosition() const { return _position; }

	Object(Vec2 pos);

	Object();

public:
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
	void setPosition(const cugl::Vec2& position);

	/**
	 * Sets the texture
	 *
	 * @param texture   The texture
	 */
	void setTexture(const std::shared_ptr<graphics::Texture>& texture);


	/** Update method for this object. This will probably be different for each subclass. */
	virtual void update(float timestep);

	/**
	 * Destroys this Object, releasing all resources.
	 */

	virtual ~Object(void) { dispose(); }

	/**
	 * Disposes all resources and assets of this Object
	 *
	 * Any assets owned by this object will be immediately released.
	 */
	void dispose();

	void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch,
		cugl::Size size);
};


#endif /* __OBJECT_H__ */