//
//  TreasureEvent.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 4/15/25.
//

#include <stdio.h>

#include "TreasureEvent.h"
#include "Message.h"
using namespace cugl::physics2::distrib;

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> TreasureEvent::newEvent() {
    return std::make_shared<TreasureEvent>();
}

std::shared_ptr<NetEvent> TreasureEvent::allocTreasureEvent(int playerID) {
    auto event = std::make_shared<TreasureEvent>();
    event->_playerID = playerID;
    return event;
}

std::vector<std::byte> TreasureEvent::serialize() {
    _serializer.reset();
    _serializer.writeSint32(static_cast<Sint32>(_playerID));

    return _serializer.serialize();
}

void TreasureEvent::deserialize(const std::vector<std::byte>& data) {
    _deserializer.reset();
    _deserializer.receive(data);
    _playerID = _deserializer.readSint32();
}


