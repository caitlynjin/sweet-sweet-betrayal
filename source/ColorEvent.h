//
//  ColorEvent.hpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/8/25.
//

#ifndef ColorEvent_hpp
#define ColorEvent_hpp

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;


class ColorEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    
    ColorType _color;
    int _prevColorInt;
    int _playerID;
    
public:
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocColorEvent(int playerID, ColorType color, int prevColorInt);
    
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
    
    /** Gets the previous color int of the event. */
    int getPrevColor() { return _prevColorInt; }
    
    /** Gets the player id of the event. */
    int getPlayerID() { return _playerID; }
};
#endif /* MessageEvent_hpp */

