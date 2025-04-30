//
//  AnimationEvent.h
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/26/25.
//


#ifndef AnimationEvent_h
#define AnimationEvent_h

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;

/**
 * Animation types supported by AnimationEvent
 */
enum class AnimationType : int {
    DEATH = 0,
    GLIDE = 1,
};


class AnimationEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    
    int _playerID;
    AnimationType _animation;
    bool          _activate;
    
public:

    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<AnimationEvent> allocAnimationEvent(int playerID, AnimationType animation, bool activate);
    
    /**
     * Serialize any paramater that the event contains to a vector of bytes.
     */
    std::vector<std::byte> serialize() override;
    /**
     * Deserialize a vector of bytes and set the corresponding parameters.
     *
     * @param data  a byte vector packed by serialize()
     *
     * This function should be the "reverse" of the serialize() function: it
     * should be able to recreate a serialized event entirely, setting all the
     * useful parameters of this class.
     */
    void deserialize(const std::vector<std::byte>& data) override;
    
    /** Gets the player id of the event. */
    int getPlayerID() { return _playerID; }
    
    /** Gets the animation type of the event. */
    AnimationType getAnimation() const { return _animation; }
    
    bool isActivate() const { return _activate; }

};


#endif /* AnimationEvent_h */
