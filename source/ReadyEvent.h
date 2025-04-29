//
// Created by chvel on 4/26/2025.
//

#ifndef SWEETSWEETBETRAYAL_READYEVENT_H
#define SWEETSWEETBETRAYAL_READYEVENT_H

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;

class ReadyEvent : public NetEvent {
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;

    ColorType _color;
    int _playerID;
    bool _ready;

public:
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;

    static std::shared_ptr<NetEvent> allocReadyEvent(int playerID, ColorType color, bool ready);

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

    /** Gets the color enum of the event. */
    ColorType getColor() { return _color; }

    /** Gets the player id of the event. */
    int getPlayerID() { return _playerID; }

    /** Gets the ready state of the event */
    bool getReady() { return _ready; }
};
#endif //SWEETSWEETBETRAYAL_READYEVENT_H