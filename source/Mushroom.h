#pragma once
#include "Object.h"
#ifndef __MUSHROOM_H__
#define __MUSHROOM_H__
#include <cugl/cugl.h>


using namespace cugl;
using namespace std;

class Mushroom : public Object {

private:
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;
    
protected:
    std::string _mushroomTexture;
    
    float _drawScale;
    std::shared_ptr<scene2::SceneNode> _node;
    

public:
    
    Mushroom() : Object() {}

    Mushroom(Vec2 pos) : Object(pos) {}

    // void update(float timestep) override;

    ~Mushroom(void) override { dispose(); }

    void dispose();
    
    virtual void setPosition(const cugl::Vec2& position);

    std::shared_ptr<cugl::physics2::Obstacle> getObstacle() {
        return _box;
    }

    static std::shared_ptr<Mushroom> alloc(const Vec2 position, const Size size, float scale) {
        std::shared_ptr<Mushroom> result = std::make_shared<Mushroom>();
        return (result->init(position, size, scale) ? result : nullptr);
    }

    /** for networking */
    static std::shared_ptr<Mushroom> alloc(const Vec2 position, const Size size, float scale, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
        std::shared_ptr<Mushroom> result = std::make_shared<Mushroom>();
        return (result->init(position, size, scale, box) ? result : nullptr);
    }

    bool init(const Vec2 pos, const Size size, float scale);

    /** for networking */
    bool init(const Vec2 pos, const Size size, float scale, std::shared_ptr<cugl::physics2::BoxObstacle> box);
    
    /**
     * Sets the scene graph node representing this Mushroom.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this DudeModel, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
        _node = node;
        _node->setPosition(getPosition() * _drawScale);
    }
   
};


#endif /* __MUSHROOM_H__ */
