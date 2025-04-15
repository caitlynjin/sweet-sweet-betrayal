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

public:
    
    Treasure() : Object() {}

    Treasure(Vec2 pos) : Object(pos) {}

    /** The update method for the spike */
    void update(float timestep) override;

    string getJsonKey() override;

    ~Treasure(void) override { dispose(); }

    void dispose();
    
    virtual void setPosition(const cugl::Vec2& position) override;
    std::shared_ptr<cugl::physics2::Obstacle> getObstacle() override {
        return _box;
    }

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
    /** for networking */
    static std::shared_ptr<Treasure> alloc(const Vec2 position, const Size size, float scale, bool taken, std::shared_ptr<cugl::physics2::BoxObstacle> box) {
        std::shared_ptr<Treasure> result = std::make_shared<Treasure>();
        return (result->init(position, size, scale, taken, box) ? result : nullptr);
    }

    bool init(const Vec2 pos, const Size size, float scale);

    bool init(const Vec2 pos, const Size size, float scale, string jsonType);

    /** for networking */
    bool init(const Vec2 pos, const Size size, float scale, bool taken, std::shared_ptr<cugl::physics2::BoxObstacle> box);
    
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
        _node->setPosition(getPosition() * _drawScale);
    }

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
    
    
    /**
     * Returns whether the treasure can be stolen  by a player.
     * @return True if stealable, false otherwise
     */
    bool isStealable() {
        if (_stealCooldown > 0){
            _isStealable = false;
        }
        else{
            _isStealable = true;
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
        CULog("treasure taken");
        return _taken; }
    
    void reset();
};


#endif /* __TREASURE_H__ */
