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
#define DENSITY 12.0f
#define INIT_IMPULSE 6.0f
#define UP_FORCE 23.0f
#define RADIUS 0.25f
#define EXPLOSION_TIME 0.1
#define DAMPING 2.25f

using namespace cugl;
using namespace std;
class Projectile : public Object {

private:
    /** The BoxObstacle wrapped by this Projectile object */
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;
    /**owned by a player**/

    bool _justInit;

    //Keeps track of whether the projectile is still active, counting down towards its explosion animation if not
    bool _exploding = false;
    float _explosionCountdown = 0;

    std::function<void()> _delFunction;


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

    std::shared_ptr<cugl::scene2::SpriteNode> _projectileSprite;

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

    //Set the texture for this projectile
    void setTextureNode(std::shared_ptr<cugl::scene2::SpriteNode> tex);

    //Updating motion only for host, so we don't add in too much velocity
    void hostUpdate(float timestep);

    std::map<std::string, std::any> getMap() override;
    /**
     * Sets the taken status of the Projectile.
     * @param taken Whether the Projectile has been taken by a player.
     */

    //Detonate the projectile-Cause it to being counting down and change its animation, before deleting itself
    void detonate();

    bool isExplode() {
        return (_explosionCountdown > EXPLOSION_TIME);
    };


    void reset();

};

#endif /* __PROJECTILE_H__ */
