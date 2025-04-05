#include "Object.h"
#ifndef __WINDOBSTACLE_H__
#define __WINDOBSTACLE_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;
/**How many raycasting points we generate plus one*/
constexpr int RAYS = 3;
#define OFFSET 0.1f;

class WindObstacle : public Object {

private:
	/** The BoxObstacle wrapped by this WindObstacle object */
	std::shared_ptr<cugl::physics2::BoxObstacle> _gust;

	/*The direction of where the wind wind will push you*/
	Vec2 _windForce;
	/**The offset of the ray endpoints. Determines both wind directio and range. */
	Vec2 _windDirection = Vec2(0,1);

	/**How many of the rays we have hit the player with.*/
	int _playerHits;

	/**The points where we are raycasting from. */

	std::vector<Vec2> _rayOrigins;

	/**Vectors that stores the raycasting data for each update. stores info for obstacle and player distance from rays respectively.*/
	float _rayDist[RAYS+1];
	float _playerDist[RAYS + 1];

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
	void dispose() override;


	/** Return the obstacle */
	std::shared_ptr<cugl::physics2::Obstacle> getObstacle() override {
		return _gust;
	}

	/** This method allocates a BoxObstacle.
	* It is important to call this method to properly set up the WindObstacle and link it to a physics object.
	*/
	static std::shared_ptr<WindObstacle> alloc(const Vec2 position, const Size size, const Vec2 windDirection, const Vec2 windStrength) {
		std::shared_ptr<WindObstacle> result = std::make_shared<WindObstacle>();
		return (result->init(position, size, windDirection, windStrength) ? result : nullptr);
	}

	static std::shared_ptr<WindObstacle> alloc(const Vec2 position, const Size size, const Vec2 windDirection, const Vec2 windStrength, string jsonType) {
		std::shared_ptr<WindObstacle> result = std::make_shared<WindObstacle>();
		return (result->init(position, size, windDirection, windStrength, jsonType) ? result : nullptr);
	}
	/*Intialize according to position and size. Need to change later*/
	bool init(const Vec2 pos, const Size size, const Vec2 gustDir, const Vec2 windStrength);

	bool init(const Vec2 pos, const Size size, const Vec2 gustDir, const Vec2 windStrength, string jsonType);

	string ReportFixture(b2Fixture* contact, const Vec2& point, const Vec2& normal, float fraction);

	/*How hard and what direction the wind is blowing the player in*/
	const Vec2 getWindForce() { return _windForce; };

	/**Returns the list of wind origins*/

	const std::vector<Vec2> getRayOrigins(){return _rayOrigins;};

	const Vec2 getWindDirection() { return _windDirection; }

	/**Getter and setters for player and ray distancearrays*/
	const float getRayDist(int x) { return _rayDist[x]; }
	const float getPlayerDist(int x) { return _playerDist[x]; }

	void setRayDist(int x, float y) { _rayDist[x] = y; }
	void setPlayerDist(int x, float y) { _playerDist[x] = y; }

	/*Getter for player hits**/

	const int getPlayerHits() { return _playerHits; }
	
	std::map<std::string, std::any> getMap() override;
};


#endif /* __WindObstacle_H__ */
