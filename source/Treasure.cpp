#include "Treasure.h"
//#include "Object.h"

#define JSON_KEY "treasures"

using namespace cugl;
using namespace cugl::graphics;


void Treasure::update(float timestep) {
//    Vec2 currPos = getPosition();
//    currPos += Vec2(1,1);
//    setPosition(currPos);
//    _box->setPosition(currPos);
    
//    if (_node != nullptr) {
//        _node->setPosition(getPosition()*_drawScale);
//    }
    
    PolygonObstacle::update(timestep);
    if (_node != nullptr)
    {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }
    if (getName() != "treasureLevelEditor") {
        updateAnimation(timestep);
    }
    
    
    // Should be called every frame
    updateCooldown();
}

/**
 Checks if the cooldown is currently active and needs to be updated.
 Also returns treasure back to original state once cooldown is over.
 */
void Treasure::updateCooldown(){
    // Update flashing and cooldown when first stolen
    if (_stealCooldown > 0){
        _stealCooldown -= 0.1f;
        updateFlash();
    }
    else{
        // Check if current alpha needs to be reset back to normal
        Color4 currColor = _node->getColor();
        if (currColor.a != 255.0f){
            currColor.a += FLASH_RATE;
            _node->setColor(currColor);
            _decreaseAlpha = true;
        }
    }
}

/**
 Updates the flashing effect of the treasure to represent currently in cooldown
 */
void Treasure::updateFlash(){
    Color4 currColor = _node->getColor();
    float alpha = currColor.a;
    
    if (_decreaseAlpha){
        alpha -= FLASH_RATE;
        if (alpha < MIN_ALPHA){
            _decreaseAlpha = false;
        }
    }
    else{
        alpha += FLASH_RATE;
        if (alpha > MAX_ALPHA){
            _decreaseAlpha = true;
        }
    }
    
    currColor.a = alpha;
    _node->setColor(currColor);
}

string Treasure::getJsonKey() {
    return JSON_KEY;
}

void Treasure::updateAnimation(float timestep){
    // Update animation
//    std::vector<int> frames = _spinAnimateSprite->getSequence();
//    for (int i = 0; i < frames.size(); i++) {
//        CULog("Frame %d: %d", i, frames[i]);
//    }
    doStrip(_spinAction, DURATION);
    _timeline->update(timestep);
    
    
}

void Treasure::setAnimation(std::shared_ptr<scene2::SpriteNode> sprite){
    _spinSpriteNode = sprite;

    _node = _spinSpriteNode;
    _spinSpriteNode->setVisible(true);
//    _node->setScale(0.065f);
    
    _timeline = ActionTimeline::alloc();

    // Create the frame sequence
    // For an 8x8 spritesheet
    const int span = 32;

    std::vector<int> forward;
    for (int ii = 1; ii < span; ii++) {
        forward.push_back(ii);
    }
    // Loop back to beginning
    forward.push_back(0);
    
    // Create animation
    _spinAnimateSprite = AnimateSprite::alloc(forward);
    _spinAction = _spinAnimateSprite->attach<scene2::SpriteNode>(_spinSpriteNode);
}

/**
 * Performs a film strip action
 *
 * @param action The film strip action
 * @param slide  The associated movement slide
 */
void Treasure::doStrip(cugl::ActionFunction action, float duration = DURATION) {

    if (_timeline->isActive(ACT_KEY)) {
        // NO OP
    } else {
        _timeline->add(ACT_KEY, action, duration);
    }
}

void Treasure::setPositionInit(const cugl::Vec2 &position){
    _position = position;
    PolygonObstacle::setPosition(position + _size / 2);
    
    if (_node != nullptr) {
        _node->setPosition((position+_size/2)*_drawScale);
    }
}

void Treasure::dispose() {
    _node->dispose();
}



using namespace cugl;

#pragma mark -
#pragma mark Constructors


/**
 * Initializes a new Treasure at the given position.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Treasure::init(const Vec2 pos, const Size size, float scale) {
    return Treasure::init(pos, size, scale, "default");
}
bool Treasure::init(const Vec2 pos, const Size size, float scale, string jsonType) {
    Size nsize = size;
    _treasureTexture = "";
    _position = pos;
    _size = size;
    _jsonType = jsonType;
    _itemType = Item::TREASURE;
    _drawScale = scale;
    
    PolyFactory factory;
    Poly2 rect = factory.makeRect(size/-2, nsize*0.5);
    
    if (PolygonObstacle::init(rect)){
        setSensor(true);
        setName("treasure");
        setDebugColor(Color4::YELLOW);
        setPosition(pos + size/2);
        _node = scene2::SpriteNode::alloc();
        
        return true;
    }

    return false;
}


void Treasure::reset(){
    _taken = false;
    _isStealable = true;
}

std::map<std::string, std::any> Treasure::getMap() {
    std::map<std::string, std::any> m = {
        {"x", double(_position.x)},
        {"y", double(_position.y)},
        {"width", double(_size.getIWidth())},
        {"height", double(_size.getIHeight())},
        {"scale", double(_drawScale)},
         {"type", std::string(_jsonType)}
    };
    return m;
}


