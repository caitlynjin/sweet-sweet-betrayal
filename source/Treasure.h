//
//  Treasure.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 2/22/25.
//

#pragma once
#include "Object.h"
#ifndef __TREASURE_H__
#define __TREASURE_H__
#include <cugl/cugl.h>

#define DURATION 4.0f
#define ACT_KEY  "current"


using namespace cugl;
using namespace std;

class Treasure : public Object {

private:
    /** The BoxObstacle wrapped by this Treasure object */
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;
    /**owned by a player**/
    bool _taken = false;
    
    /** Whether the treasure is stealable */
    bool _isStealable = true;
    
    float const MAX_ALPHA = 200.0f;
    
    float const MIN_ALPHA = 100.0f;
    
    float const FLASH_RATE = 5.0f;

    
    
protected:
    /** The texture for the treasure */
    std::string _treasureTexture;
    
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    /** The scene graph node for the Treasure. */
    std::shared_ptr<scene2::SceneNode> _node;
    
    /** The length of the treasur's steal cooldown */
    int const STEAL_COOLDOWN = 30;
    
    /** The current progress of the steal cooldown */
    float _stealCooldown = 0.0f;
    
    /** Whether we are decreasing alpha value */
    bool _decreaseAlpha = true;
    
    /** Whether the host is currently updating the treasure */
    bool _isHost;
    
    bool _atGoal = false;
    
    
#pragma mark Animation Variables
    /** Manager to process the animation actions */
    std::shared_ptr<cugl::ActionTimeline> _timeline;
    
    /** Animation variables */
    std::shared_ptr<AnimateSprite> _spinAnimateSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _spinSpriteNode;
    cugl::ActionFunction _spinAction;

public:
    
    Treasure() : Object() {}

    Treasure(Vec2 pos) : Object(pos) {}

    /** The update method for the spike */
    void update(float timestep) override;

    string getJsonKey() override;

    ~Treasure(void) override { dispose(); }

    void dispose() override;
    
    virtual void setPositionInit(const cugl::Vec2& position) override;

    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Spike and link it to a physics object.
    */
    static std::shared_ptr<Treasure> alloc(const Vec2 position, const Size size, float scale) {
        std::shared_ptr<Treasure> result = std::make_shared<Treasure>();
        return (result->init(position, size, scale) ? result : nullptr);
    }

    static std::shared_ptr<Treasure> alloc(const Vec2 position, const Size size, float scale, string jsonType) {
        std::shared_ptr<Treasure> result = std::make_shared<Treasure>();
        return (result->init(position, size, scale, jsonType) ? result : nullptr);
    }

    bool init(const Vec2 pos, const Size size, float scale);

    bool init(const Vec2 pos, const Size size, float scale, string jsonType);
    
    
    /**
     * Returns the scene graph node representing this Treasure.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlayerModel.
     */
    const std::shared_ptr<scene2::SceneNode>& getSceneNode() const { return _node; }

    
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
        _node = node;
        _node->setPosition(getPositionInit() * _drawScale);
    }
    
    /**
     Sets the spinning animation for the treasure.
     */
    void setAnimation(std::shared_ptr<scene2::SpriteNode> sprite);
    
    void updateAnimation(float timestep);
    
    /** Increments an animation film strip */
    void doStrip(cugl::ActionFunction action, float duration);

    std::map<std::string, std::any> getMap() override;
    /**
     * Sets the taken status of the treasure.
     * @param taken Whether the treasure has been taken by a player.
     */
    void setTaken(bool taken) {
        _taken = taken;
        
        if (taken){
            _stealCooldown = STEAL_COOLDOWN;
        }
    }
    
    void setAtGoal(bool value){
        _atGoal = value;
    }
    
    
    /**
     * Returns whether the treasure can be stolen  by a player.
     * @return True if stealable, false otherwise
     */
    bool isStealable() {
        if (!_atGoal){
            if (_stealCooldown > 0){
                _isStealable = false;
            }
            else{
                _isStealable = true;
            }
        }
        else{
            return false;
        }
        return _isStealable;
    }
    
    void setStealable(bool value){
        _isStealable = value;
    }
    /**
     * Returns whether the treasure has been taken by a player.
     * @return True if taken, false otherwise.
     */
    bool isTaken() const {
        return _taken; }
    
    void reset();
    
    /**
     Updates the flashing effect of the treasure to represent currently in cooldown
     */
    void updateFlash();
    
    /**
     Checks if the cooldown is currently active and needs to be updated.
     Also returns treasure back to original state once cooldown is over.
     */
    void updateCooldown();
};


#endif /* __TREASURE_H__ */
