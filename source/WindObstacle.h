#include "Object.h"
#ifndef __WINDOBSTACLE_H__
#define __WINDOBSTACLE_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;
/**How many raycasting points we generate plus one*/
constexpr int RAYS = 3;
#define OFFSET 0.1f
#define FAN_ANIM_CYCLE 0.8f
#define GUST_ANIM_CYCLE 3.0f
#define PRIORITY -900

class WindObstacle : public Object {

private:

	/*The direction of where the wind wind will push you*/
	Vec2 _windForce;
	/**The offset of the ray endpoints. Determines both wind directio and range. */
	Vec2 _windDirection = Vec2(0,1);

	/**How many of the rays we have hit the player with.*/
	int _playerHits;
	//How far the player is along the ray
	float _currentPlayerDist;
	//The closest detected raycast collision
	float _minRayDist;
	float _prevRayDist;
	float _angle = M_PI/2;

	/**The points where we are raycasting from. */

	std::vector<Vec2> _rayOrigins;

	/**Vectors that stores the raycasting data for each update. stores info for obstacle and player distance from rays respectively.*/
	float _rayDist[RAYS+1];
	float _playerDist[RAYS + 1];

	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;


public:
	WindObstacle() : Object() {}

	WindObstacle(Vec2 pos) : Object(pos) {}

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPositionInit(const cugl::Vec2& position) override;

	/** The update method for the WindObstacle */
	void update(float timestep) override;

  /** Return the JSON Key
  * Every derived class should override this
  */
	string getJsonKey() override;


	/** Disposal */
	~WindObstacle(void) override { dispose(); }
	void dispose() override;

	/** This method allocates a BoxObstacle.
	* It is important to call this method to properly set up the WindObstacle and link it to a physics object.
	*/
	static std::shared_ptr<WindObstacle> alloc(const Vec2 position, const Size size, float scale, const Vec2 windDirection,
		const Vec2 windStrength, const float angle) {
		std::shared_ptr<WindObstacle> result = std::make_shared<WindObstacle>();
		return (result->init(position, size,scale, windDirection, windStrength, angle) ? result : nullptr);
	}

	static std::shared_ptr<WindObstacle> alloc(const Vec2 position, const Size size, float scale, const Vec2 windDirection,
		const Vec2 windStrength,const float angle, string jsonType) {
		std::shared_ptr<WindObstacle> result = std::make_shared<WindObstacle>();
		return (result->init(position, size, scale, windDirection, windStrength, angle,jsonType) ? result : nullptr);
	}
	/*Intialize according to position and size. Need to change later*/
	bool init(const Vec2 pos, const Size size, float scale, const Vec2 gustDir, const Vec2 windStrength, const float angle);

	bool init(const Vec2 pos, const Size size, float scale, const Vec2 gustDir, const Vec2 windStrength, const float angle,string jsonType);

	string ReportFixture(b2Fixture* contact, const Vec2& point, const Vec2& normal, float fraction);

	/*Sprite Node representing the wind object*/
	std::shared_ptr<AnimateSprite> _windAnimate;
	std::shared_ptr<cugl::scene2::SpriteNode> _gustNode;
	cugl::ActionFunction _windAction;

	//Regenerate and set ray origins
	void setRayOrigins();

	/*How hard and what direction the wind is blowing the player in*/
	const Vec2 getWindForce() { return _windForce; };

	//Get the distnace of the last ray cast to the player
	const float getPlayerToWindDist() { return _currentPlayerDist; }

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

	/*Animation methods*/
	void setFanAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);
	void updateAnimation(float timestep);

	//Animation variables
	std::shared_ptr<cugl::ActionTimeline> _fanTimeline;
	std::shared_ptr<scene2::SceneNode> _node;
	std::shared_ptr<cugl::scene2::SpriteNode> _fanSpriteNode;
	std::shared_ptr<AnimateSprite> _fanAnimateSprite;
	cugl::ActionFunction _fanAction;

	/*Sets up all the gust animations using an array of gust sprites*/
	void setGustAnimation(std::vector<std::shared_ptr<scene2::SpriteNode>> sprite, int nFrames);
	/*Sprite Node representing the gust levels. Level 1 is lowest, level 4 is highest*/
	std::shared_ptr<cugl::ActionTimeline> _gustTimeline1;
	std::shared_ptr<cugl::scene2::SpriteNode> _gustSpriteNode1;
	cugl::ActionFunction _gustAction1;
	std::shared_ptr<AnimateSprite> _gustAnimateSprite1;

	std::shared_ptr<cugl::ActionTimeline> _gustTimeline2;
	std::shared_ptr<cugl::scene2::SpriteNode> _gustSpriteNode2;
	cugl::ActionFunction _gustAction2;
	std::shared_ptr<AnimateSprite> _gustAnimateSprite2;

	std::shared_ptr<cugl::ActionTimeline> _gustTimeline3;
	std::shared_ptr<cugl::scene2::SpriteNode> _gustSpriteNode3;
	cugl::ActionFunction _gustAction3;
	std::shared_ptr<AnimateSprite> _gustAnimateSprite3;

	std::shared_ptr<cugl::ActionTimeline> _gustTimeline4;
	std::shared_ptr<cugl::scene2::SpriteNode> _gustSpriteNode4;
	cugl::ActionFunction _gustAction4;
	std::shared_ptr<AnimateSprite> _gustAnimateSprite4;

	/**
	* Returns the scene graph node representing this WindObstacle.
	*
	* By storing a reference to the scene graph node, the model can update
	* the node to be in sync with the physics info. It does this via the
	* {@link Obstacle#update(float)} method.
	*
	* @return the scene graph node representing this WinDNNDNDNDND.
	*/
	const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _node; }

};


#endif /* __WindObstacle_H__ */
