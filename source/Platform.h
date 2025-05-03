#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include <cugl/cugl.h>
#include "Object.h"

using namespace cugl;
using namespace std;
using namespace Constants;

class Platform : public Object {

private:
    bool   _moving = false;
    Vec2   _startPos;
    Vec2   _endPos;
    float  _speed = 0;
    bool   _forward = true;
    bool _wall = false;
    int _turnCount = 0;

public:
	Platform() : Object() {}

    Platform(Vec2 pos) : Object(pos, Item::PLATFORM) {}

	/** The update method for the platform */
	void update(float timestep) override;

    void updateMovingPlatform(float timestep); 

    string getJsonKey() override;

	~Platform(void) override { dispose(); }

	void dispose() override;

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPositionInit(const cugl::Vec2& position) override;

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Platform and link it to a physics object.
    */
    static std::shared_ptr<Platform> alloc(const Vec2 position, const Size size, string jsonType) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(position, size, jsonType) ? result : nullptr);
    }

    static std::shared_ptr<Platform> alloc(const Vec2 position, const Size size) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(position, size) ? result : nullptr);
    }

  
    // New alloc method for moving platform.
    static std::shared_ptr<Platform> allocMoving(const Vec2 position, const Size size, const Vec2 start, const Vec2 end, float speed) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->initMoving(position, size, start, end, speed) ? result : nullptr);
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
    /**update start.end pos**/
    bool updateMoving(Vec2 gridpos);
    /*Return the Y value of the top of the platform*/
    float getPlatformTop() {
        return (_position.y + _size.height*0.5);
         }

    void setPlatformAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);
    void updateAnimation(float timestep);

    std::shared_ptr<cugl::ActionTimeline> _platTimeline;
    std::shared_ptr<cugl::scene2::SpriteNode> _platSpriteNode;
    std::shared_ptr<AnimateSprite> _platAnimateSprite;
    cugl::ActionFunction _platAction;

    const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _sceneNode; }
};


#endif /* __PLATFORM_H__ */
