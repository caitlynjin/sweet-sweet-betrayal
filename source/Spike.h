#pragma once
#include "Object.h"
#ifndef __SPIKE_H__
#define __SPIKE_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;

class Spike : public Object {

private:
	/** The BoxObstacle wrapped by this Spike object */
	std::shared_ptr<cugl::physics2::PolygonObstacle> _hitbox;
    
protected:
    /** The texture for the spike */
    std::string _spikeTexture;
    
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;

    /** The angle for the hitbox */
    float _angle;
    /** The scene graph node for the Treasure. */
    std::shared_ptr<scene2::SceneNode> _node;

public:
	Spike() : Object() {}

	Spike(Vec2 pos) : Object(pos) {}

	/** The update method for the spike */
	void update(float timestep) override;

    string getJsonKey() override;

	~Spike(void) override { dispose(); }

	void dispose();

	std::shared_ptr<cugl::physics2::Obstacle> getObstacle() override {
		return _hitbox;
	}

	/** This method allocates a BoxObstacle.
	* It is important to call this method to properly set up the Spike and link it to a physics object.
	*/
	static std::shared_ptr<Spike> alloc(const Vec2 position, const Size size, float scale, float angle = 0) {
		std::shared_ptr<Spike> result = std::make_shared<Spike>();
		return (result->init(position, size, scale, angle) ? result : nullptr);
	}


	bool init(const Vec2 pos, const Size size, float scale, float angle);
    
    /**
     * Sets the scene graph node representing this Spike.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this Spike, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node, float angle) {
        _node = node;
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(angle);
    }

    std::map<std::string, std::any> getMap() override;

    /* Gets the angle for the hitbox.
    * @return the angle for the hitbox */
    float getAngle() { return _angle; }
};


#endif /* __SPIKE_H__ */
