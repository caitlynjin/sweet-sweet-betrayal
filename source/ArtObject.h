#pragma once
#ifndef __ART_OBJECT_H__
#define __ART_OBJECT_H__
#include <cugl/cugl.h>
#include "Object.h"

#define ACT_KEY "defaultArtAnimation"

using namespace cugl;
using namespace std;

class ArtObject : public Object {

protected:

    /** The BoxObstacle wrapped by this ArtObject 
    * Note that this does NOT participate in collisions or physics interactions.
    * It is only here to make object setup in the world consistent for all objects.
    * This gets disabled in the createArtObject() method of ObjectController.
    */
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;

    /** The layer in which to draw the art object */
    int _layer;

    /** The angle for the sprite */
    float _angle;

    /** Whether or not the art object is animated */
    bool _isAnimated = false;

    /** The scene graph node for the ArtObject. */
    std::shared_ptr<scene2::SceneNode> _node;

#pragma mark Animation Variables
    /** Manager to process the animation actions */
    std::shared_ptr<cugl::ActionTimeline> _timeline;

    /** Animation variables */
    std::shared_ptr<AnimateSprite> _animateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _animateSpriteNode;
    cugl::ActionFunction _animationAction;
    float _animationDuration;

public:
    ArtObject() : Object(), _layer(0), _angle(0), _drawScale(0) {}

    ArtObject(Vec2 pos) : Object(pos) {}

    /** The update method for the spike */
    void update(float timestep) override;

    string getJsonKey() override;

    ~ArtObject(void) override { dispose(); }

    void dispose() override;

    void setAnimated(bool isAnimated);

    void setAnimationDuration(float dur);

    void setAnimation(std::shared_ptr<scene2::SpriteNode> sprite);

    void setPositionInit(const cugl::Vec2& position) override;

    void setLayer(int layer);

    int getLayer() {
        return _node->getPriority();
    }

    /** Increments an animation film strip */
    void doStrip(cugl::ActionFunction action, float duration);

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the ArtObject.
    */
    static std::shared_ptr<ArtObject> alloc(const Vec2 position, const Size size, float scale, float angle, int layer) {
        std::shared_ptr<ArtObject> result = std::make_shared<ArtObject>();
        return (result->init(position, size, scale, angle, layer) ? result : nullptr);
    }

    static std::shared_ptr<ArtObject> alloc(const Vec2 position, const Size size, float scale, float angle, string jsonType) {
        std::shared_ptr<ArtObject> result = std::make_shared<ArtObject>();
        return (result->init(position, size, scale, angle, jsonType) ? result : nullptr);
    }
    static std::shared_ptr<ArtObject> alloc(const Vec2 position, const Size size, float scale, float angle, int layer, string jsonType) {
        std::shared_ptr<ArtObject> result = std::make_shared<ArtObject>();
        return (result->init(position, size, scale, angle, layer, jsonType) ? result : nullptr);
    }

    bool init(const Vec2 pos, const Size size, float scale, float angle, int layer);
    bool init(const Vec2 pos, const Size size, float scale, float angle, string jsonType);
    bool init(const Vec2 pos, const Size size, float scale, float angle, int layer, string jsonType);

    /**
     * Sets the scene graph node representing this Spike.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this Spike, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node, float angle); 

    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node);


    std::map<std::string, std::any> getMap() override;
};


#endif /* __ART_OBJECT_H__ */
