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

public:
    
    Mushroom() : Object() {}

    Mushroom(Vec2 pos) : Object(pos, Item::MUSHROOM) {}

    // void update(float timestep) override;

    ~Mushroom(void) override { dispose(); }

    void dispose() override;

    void setPositionInit(const cugl::Vec2& position) override;

    static std::shared_ptr<Mushroom> alloc(const Vec2 position, const Size size, float scale) {
        std::shared_ptr<Mushroom> result = std::make_shared<Mushroom>();
        return (result->init(position, size, scale) ? result : nullptr);
    }


    bool init(const Vec2 pos, const Size size, float scale);
   
};


#endif /* __MUSHROOM_H__ */
