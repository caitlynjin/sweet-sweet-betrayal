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
	std::shared_ptr<cugl::physics2::BoxObstacle> _box;

public:
	Spike() : Object() {}

	Spike(Vec2 pos) : Object(pos) {}

	/** The update method for the spike */
	void update(float timestep) override;

	~Spike(void) override { dispose(); }

	void dispose();

	std::shared_ptr<cugl::physics2::BoxObstacle> getObstacle() {
		return _box;
	}

	/** This method allocates a BoxObstacle.
	* It is important to call this method to properly set up the Spike and link it to a physics object.
	*/
	static std::shared_ptr<Spike> alloc(const Vec2 position, const Size size) {
		std::shared_ptr<Spike> result = std::make_shared<Spike>();
		return (result->init(position, size) ? result : nullptr);
	}

	bool init(const Vec2 pos, const Size size);
};


#endif /* __SPIKE_H__ */