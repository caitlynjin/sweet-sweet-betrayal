//
//  Bomb.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 4/25/25.
//

#ifndef __BOMB_H__
#define __BOMB_H__
#include <cugl/cugl.h>
#include "Object.h"

#define DURATION 1.0f
#define ACT_KEY  "current"

using namespace cugl;
using namespace std;
using namespace Constants;

class Bomb : public Object {

private:

protected:

#pragma mark Animation Variables
    /** Manager to process the animation actions */
    std::shared_ptr<cugl::ActionTimeline> _timeline;

    /** Animation variables */
    std::shared_ptr<AnimateSprite> _animSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _animNode;
    cugl::ActionFunction _animAction;

public:
    Bomb() : Object() {}

    Bomb(Vec2 pos) : Object(pos, Item::PLATFORM) {}

    void update(float timestep) override;

    string getJsonKey() override;

    ~Bomb(void) override { dispose(); }

    void dispose() override;

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPositionInit(const cugl::Vec2& position) override;

    /**
     * This method allocates a BoxObstacle.
     * It is important to call this method to properly set up the Bomb and link it to a physics object.
     */
    static std::shared_ptr<Bomb> alloc(const Vec2 position, const Size size, string jsonType) {
        std::shared_ptr<Bomb> result = std::make_shared<Bomb>();
        return (result->init(position, size, jsonType) ? result : nullptr);
    }

    static std::shared_ptr<Bomb> alloc(const Vec2 position, const Size size) {
        std::shared_ptr<Bomb> result = std::make_shared<Bomb>();
        return (result->init(position, size) ? result : nullptr);
    }

    // New init method for networked bombs
    bool init(const Vec2 pos, const Size size, std::shared_ptr<cugl::physics2::BoxObstacle> box);

    bool init(const Vec2 pos, const Size size);

    bool init(const Vec2 pos, const Size size, string jsonType);

    /**
     * Returns the scene graph node representing this Treasure.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlayerModel.
     */
    const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _sceneNode; }

    /**
     * Sets the scene graph node representing this Treasure.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this PlayerModel, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
        _sceneNode = node;
        _sceneNode->setPosition(getPositionInit());
    }

    void setAnimation(std::shared_ptr<scene2::SpriteNode> sprite);

    void updateAnimation(float timestep);

    /** Increments an animation film strip */
    void doStrip(cugl::ActionFunction action, float duration);

    // Map for JSON level management
    std::map<std::string, std::any> getMap() override;
};

#endif /* __BOMB_H__ */
