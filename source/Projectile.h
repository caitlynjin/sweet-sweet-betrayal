//
//  Projectile.h
//  SweetSweetBetrayal
//
//  Created by ERIC YANG on 4/24/25.
//

#pragma once
#include "Object.h"
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__
#include <cugl/cugl.h>

#define DURATION 8.0f
#define ACT_KEY  "current"


using namespace cugl;
using namespace std;
class Projectile : public Object {

private:
    /** The BoxObstacle wrapped by this Projectile object */
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;
    /**owned by a player**/


protected:
    /** The texture for the Projectile */
    std::string _ProjectileTexture;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    /** The scene graph node for the Projectile. */
    std::shared_ptr<scene2::SceneNode> _node;

#pragma mark Animation Variables
    /** Manager to process the animation actions */
    std::shared_ptr<cugl::ActionTimeline> _timeline;

    /** Animation variables */
    std::shared_ptr<AnimateSprite> _spinAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _spinSpriteNode;
    cugl::ActionFunction _spinAction;

public:

    Projectile() : Object() {}

    Projectile(Vec2 pos) : Object(pos) {}

    /** The update method for the spike */
    void update(float timestep) override;

    string getJsonKey() override;

    ~Projectile(void) override { dispose(); }

    void dispose() override;

    virtual void setPositionInit(const cugl::Vec2& position) override;

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Spike and link it to a physics object.
    */
    static std::shared_ptr<Projectile> alloc(const Vec2 position, const Size size, float scale) {
        std::shared_ptr<Projectile> result = std::make_shared<Projectile>();
        return (result->init(position, size, scale) ? result : nullptr);
    }

    static std::shared_ptr<Projectile> alloc(const Vec2 position, const Size size, float scale, string jsonType) {
        std::shared_ptr<Projectile> result = std::make_shared<Projectile>();
        return (result->init(position, size, scale, jsonType) ? result : nullptr);
    }

    bool init(const Vec2 pos, const Size size, float scale);

    bool init(const Vec2 pos, const Size size, float scale, string jsonType);


    /**
     * Returns the scene graph node representing this Projectile.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlayerModel.
     */
    const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _node; }


    /**
     * Sets the scene graph node representing this Projectile.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this PlayerModel, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
        _node = node;
        _node->setPosition(getPositionInit() * _drawScale);
    }

    /**
     Sets the spinning animation for the Projectile.
     */
    void setAnimation(std::shared_ptr<scene2::SpriteNode> sprite);

    void updateAnimation(float timestep);

    /** Increments an animation film strip */
    void doStrip(cugl::ActionFunction action, float duration);

    std::map<std::string, std::any> getMap() override;
    /**
     * Sets the taken status of the Projectile.
     * @param taken Whether the Projectile has been taken by a player.
     */


    void reset();

};

#endif /* __PROJECTILE_H__ */
