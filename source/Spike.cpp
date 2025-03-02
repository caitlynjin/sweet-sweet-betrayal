#include "Spike.h"
#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;

void Spike::update(float timestep) {
}

void Spike::dispose() {}



using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Spike at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Spike::init(const Vec2 pos, const Size size, float scale, float angle) {
    _spikeTexture = "";
    _drawScale = scale;
    Size nsize = size*.40; // FIX LATER
    
    Vec2 posA = Vec2(-nsize.width/2, -nsize.height/2);
    Vec2 posB = Vec2(nsize.width/2, -nsize.height/2);
    Vec2 posC = Vec2(0, nsize.height / 2);
    
    PolyFactory factory;
    Poly2 triangle = factory.makeTriangle(posA, posB, posC);
    _hitbox = physics2::PolygonObstacle::alloc(triangle);
    _hitbox->setDebugColor(Color4::YELLOW);
    _hitbox->setPosition(pos + size / 2);
    _hitbox->setAngle(angle);
    
    
    return true;
}




