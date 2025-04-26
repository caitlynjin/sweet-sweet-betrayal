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

using namespace cugl;
using namespace std;
using namespace Constants;

class Bomb : public Object {

private:

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

    // Map for JSON level management
    std::map<std::string, std::any> getMap() override;
};

#endif /* __BOMB_H__ */
