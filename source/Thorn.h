//
//  Thorn.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 4/12/25.
//
#ifndef __THORN_H__
#define __THORN_H__
#include "Object.h"
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;

class Thorn : public Object {

private:
    /** The BoxObstacle wrapped by this Thorn object */
    std::shared_ptr<cugl::physics2::BoxObstacle> _box;

public:
    Thorn() : Object() {}

    Thorn(Vec2 pos) : Object(pos, Item::THORN) {}

    string getJsonKey() override;

    ~Thorn(void) override { dispose(); }

    void dispose() override;

    /**
     * Sets the position
     *
     * @param position   The position
     */
    void setPositionInit(const cugl::Vec2& position) override;


    /** This method allocates a BoxObstacle.
    * It is important to call this method to properly set up the Thorn and link it to a physics object.
    */
    static std::shared_ptr<Thorn> alloc(const Vec2 position, const Size size) {
        std::shared_ptr<Thorn> result = std::make_shared<Thorn>();
        return (result->init(position, size) ? result : nullptr);
    }

    static std::shared_ptr<Thorn> alloc(const Vec2 position, const Size size, string jsonType) {
        std::shared_ptr<Thorn> result = std::make_shared<Thorn>();
        return (result->init(position, size, jsonType) ? result : nullptr);
    }


    bool init(const Vec2 pos, const Size size);
    bool init(const Vec2 pos, const Size size, string jsonType);

    void update(float timestep) override;

    std::shared_ptr<scene2::SceneNode>& getSceneNode() { return _sceneNode; }

    std::map<std::string, std::any> getMap() override;
};


#endif /* __THORN_H__ */
