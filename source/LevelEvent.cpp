//
//  LevelEvent.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 5/3/25.
//

#include <stdio.h>

#include "LevelEvent.h"
#include "Message.h"
using namespace cugl::physics2::distrib;

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> LevelEvent::newEvent(){
    return std::make_shared<LevelEvent>();
}

std::shared_ptr<NetEvent> LevelEvent::allocLevelEvent(int levelNum, bool showModal, bool playPressed){
    auto event = std::make_shared<LevelEvent>();
    event->_levelNum = levelNum;
    event->_showModal = showModal;
    event->_playPressed = playPressed;
    return event;
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> LevelEvent::serialize(){
    _serializer.reset();
    // Serialize the enum as an integer
    _serializer.writeSint32(static_cast<Sint32>(_levelNum));
    _serializer.writeBool(_showModal);
    _serializer.writeBool(_playPressed);
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
void LevelEvent::deserialize(const std::vector<std::byte>& data){
    _deserializer.reset();
    _deserializer.receive(data);
    // Read the integer and cast it back to the enum type
    int levelNum = _deserializer.readSint32();
    _levelNum = levelNum;
    _showModal = _deserializer.readBool();
    _playPressed = _deserializer.readBool();
}
