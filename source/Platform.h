#include "Object.h"
#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;

class Platform : public Object {

private:
	/** The BoxObstacle wrapped by this Platform object */
	std::shared_ptr<cugl::physics2::BoxObstacle> _box;
    bool   _moving = false;
    Vec2   _startPos;
    Vec2   _endPos;
    float  _speed = 0;
    bool   _forward = true;

public:
	Platform() : Object() {}

	Platform(Vec2 pos) : Object(pos) {}

	/** The update method for the platform */
	void update(float timestep) override;

	~Platform(void) override { dispose(); }

	void dispose();

    std::shared_ptr<cugl::physics2::BoxObstacle> getObstacle() {
        return _box;
    }

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Platform and link it to a physics object.
    */
    static std::shared_ptr<Platform> alloc(const Vec2 position, const Size size) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(position, size) ? result : nullptr);
    }
  
    // New alloc method for moving platform.
    static std::shared_ptr<Platform> allocMoving(const Vec2 position, const Size size, const Vec2 start, const Vec2 end, float speed) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->initMoving(position, size, start, end, speed) ? result : nullptr);
    }

    bool init(const Vec2 pos, const Size size);

    // New init for moving platform.
    bool initMoving(const Vec2 pos, const Size size, const Vec2 start, const Vec2 end, float speed);
};


#endif /* __PLATFORM_H__ */
