#include "Object.h"
#ifndef __MUSHROOM_H__
#define __MUSHROOM_H__
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;

class Mushroom : public Object {
private:
    float _drawScale = 1.0f;
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;

    std::shared_ptr<scene2::SceneNode> _node;

    // Animations
    std::shared_ptr<cugl::ActionTimeline>    _mushroomTimeline;
    std::shared_ptr<cugl::scene2::SpriteNode> _mushroomSpriteNode;
    std::shared_ptr<cugl::AnimateSprite>      _mushroomAnimateSprite;
    cugl::ActionFunction                      _mushroomAction;

public:
    Mushroom() : Object() {}
    Mushroom(Vec2 pos) : Object(pos, Item::MUSHROOM) {}
    ~Mushroom(void) override { dispose(); }

    bool init(const Vec2 pos, const Size size, float scale);
    void setPositionInit(const Vec2& position);
    void update(float timestep) override;
    void dispose() override;

    void setMushroomAnimation(std::shared_ptr<scene2::SpriteNode> sprite, int nFrames);
    void updateAnimation(float timestep);

    const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _node; }

    static std::shared_ptr<Mushroom> alloc(const Vec2 position, const Size size, float scale) {
        std::shared_ptr<Mushroom> result = std::make_shared<Mushroom>();
        return (result->init(position, size, scale) ? result : nullptr);
    }
};

#endif /* __MUSHROOM_H__ */
