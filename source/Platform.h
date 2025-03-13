#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include <cugl/cugl.h>
#include "Object.h"

using namespace cugl;
using namespace std;
using namespace Constants;

class Platform : public Object {

private:
	/** The BoxObstacle wrapped by this Platform object */
	std::shared_ptr<cugl::physics2::BoxObstacle> _box;
    bool   _moving = false;
    Vec2   _startPos;
    Vec2   _endPos;
    float  _speed = 0;
    bool   _forward = true;
    bool _wall = false;

public:
	Platform() : Object() {}

    Platform(Vec2 pos) : Object(pos, Item::PLATFORM) {}

	/** The update method for the platform */
	void update(float timestep) override;

    string getJsonKey() override;

	~Platform(void) override { dispose(); }

	void dispose();

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPosition(const cugl::Vec2& position) override;

    std::shared_ptr<cugl::physics2::Obstacle> getObstacle() override {
        return _box;
    }

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Platform and link it to a physics object.
    */
    static std::shared_ptr<Platform> alloc(const Vec2 position, const Size size, string jsonType) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(position + size/2, size, jsonType) ? result : nullptr);
    }

    static std::shared_ptr<Platform> alloc(const Vec2 position, const Size size) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(position + size/2, size) ? result : nullptr);
    }
    
    static std::shared_ptr<Platform> alloc(const Vec2 position, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(position, size, box) ? result : nullptr);
    }
  
    // New alloc method for moving platform.
    static std::shared_ptr<Platform> allocMoving(const Vec2 position, const Size size, const Vec2 start, const Vec2 end, float speed) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->initMoving(position + size/2, size, start + size/2, end, speed) ? result : nullptr);
    }


    
    // New init method for networked platforms
    bool init(const Vec2 pos, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box);

    bool init(const Vec2 pos, const Size size);

    bool init(const Vec2 pos, const Size size, string jsonType);


    // New init for moving platform.
    bool initMoving(const Vec2 pos, const Size size, const Vec2 start, const Vec2 end, float speed);

    // Map for JSON level management
    std::map<std::string, std::any> getMap() override;

    // Gets if this is a wall
    bool isWall() { return _wall; }
};


#endif /* __PLATFORM_H__ */
