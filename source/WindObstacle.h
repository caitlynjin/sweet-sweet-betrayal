#include "Object.h"
#ifndef __WINDOBSTACLE_H__
#define __WINDOBSTACLE_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;

class WindObstacle : public Object {

private:
	/** The BoxObstacle wrapped by this WindObstacle object */
	std::shared_ptr<cugl::physics2::BoxObstacle> _gust;

	/*The direction of where the wind gust will push you*/
	Vec2 _gustDir;

public:
	WindObstacle() : Object() {}

	WindObstacle(Vec2 pos) : Object(pos) {}

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPosition(const cugl::Vec2& position) override;

	/** The update method for the WindObstacle */
	void update(float timestep) override;

  /** Return the JSON Key
  * Every derived class should override this
  */
	string getJsonKey() override;

	/** Disposal */
	~WindObstacle(void) override { dispose(); }
	void dispose();


	/** Return the obstacle */
	std::shared_ptr<cugl::physics2::Obstacle> getObstacle() override {
		return _gust;
	}

	/** This method allocates a BoxObstacle.
	* It is important to call this method to properly set up the WindObstacle and link it to a physics object.
	*/
	static std::shared_ptr<WindObstacle> alloc(const Vec2 position, const Size size, const Vec2 gustDir) {
		std::shared_ptr<WindObstacle> result = std::make_shared<WindObstacle>();
		return (result->init(position, size, gustDir) ? result : nullptr);
	}

	static std::shared_ptr<WindObstacle> alloc(const Vec2 position, const Size size, const Vec2 gustDir, string jsonType) {
		std::shared_ptr<WindObstacle> result = std::make_shared<WindObstacle>();
		return (result->init(position, size, gustDir, jsonType) ? result : nullptr);
	}
	/*Intialize according to position and size. Need to change later*/
	bool init(const Vec2 pos, const Size size, const Vec2 gustDir);

	bool init(const Vec2 pos, const Size size, const Vec2 gustDir, string jsonType);

	/*Return the wind vector*/
	const Vec2 gustDir() { return _gustDir; };

	std::map<std::string, std::any> getMap() override;
};


#endif /* __WindObstacle_H__ */
