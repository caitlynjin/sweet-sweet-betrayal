//
//  GameObjectEvent.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 4/30/25.
//

#ifndef GameObjectEvent_h
#define GameObjectEvent_h

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;

enum class GameObjEventType {
    DELETE,
    // Add other event types like MOVE, CREATE if needed
};

class GameObjectEvent : public NetEvent {

protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;

    GameObjEventType _type;
    int _objId;

public:

    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;

    std::shared_ptr<NetEvent> allocDeletion(int objId);

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

    GameObjEventType getType() const { return _type; }

    /** Gets the player id of the event. */
    int getPlayerID() { return _playerID; }

};


#endif /* GameObjectEvent_h */
