//
//  ColorEvent.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/8/25.
//

#include "ColorEvent.h"
#include "Message.h"
using namespace cugl::physics2::distrib;

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> ColorEvent::newEvent(){
    return std::make_shared<ColorEvent>();
}

std::shared_ptr<NetEvent> ColorEvent::allocColorEvent(int playerID, ColorType color){
    auto event = std::make_shared<ColorEvent>();
    event->_playerID = playerID;
    event->_color = color;
    return event;
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> ColorEvent::serialize(){
    _serializer.reset();
    // Serialize the enum as an integer
    _serializer.writeSint32(static_cast<Sint32>(_playerID));
    _serializer.writeSint32(static_cast<Sint32>(_color));
    return _serializer.serialize();
}

/**
 * Deserialize a vector of bytes and set the corresponding parameters.
 *
 * @param data  a byte vector packed by serialize()
 *
 * This function should be the "reverse" of the serialize() function: it
 * should be able to recreate a serialized event entirely, setting all the
 * useful parameters of this class.
 */
void ColorEvent::deserialize(const std::vector<std::byte>& data){
    _deserializer.reset();
    _deserializer.receive(data);
    // Read the integer and cast it back to the enum type
    int playerID = _deserializer.readSint32();
    int colorInt = _deserializer.readSint32();
    _playerID = playerID;
    _color = static_cast<ColorType>(colorInt);
}

