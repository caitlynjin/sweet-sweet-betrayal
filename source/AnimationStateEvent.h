//
//  AnimationStateEvent.h
//  SweetSweetBetrayal
//
//  Created by jessie jia on 5/16/25.
//


#ifndef AnimationStateEvent_h
#define AnimationStateEvent_h

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
#include "PlayerModel.h"
using namespace cugl;
using namespace cugl::physics2::distrib;

class AnimationStateEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    
    int _playerID;
    PlayerModel::State _state;
    bool          _facing;
    
    
public:

    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<AnimationStateEvent> allocAnimationStateEvent(int playerID, PlayerModel::State state, bool facing);
    
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
    PlayerModel::State getAnimationState() const { return _state; }
    
    bool getFacing() const { return _facing; }

};


#endif /* AnimationStateEvent_h */
