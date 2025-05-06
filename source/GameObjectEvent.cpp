//
//  GameObjectEvent.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 4/30/25.
//

#include <stdio.h>

#include "GameObjectEvent.h"
#include "Message.h"
using namespace cugl::physics2::distrib;

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> GameObjectEvent::newEvent() {
    return std::make_shared<GameObjectEvent>();
}

std::shared_ptr<NetEvent> GameObjectEvent::allocDeletion(int objId) {
    auto event = std::make_shared<GameObjectEvent>();
    event->_type = GameObjEventType::DELETE_EVENT;
    event->_objId = objId;
    return event;
}

std::vector<std::byte> GameObjectEvent::serialize() {
    _serializer.reset();
    _serializer.writeSint32(static_cast<Sint32>(_objId));

    return _serializer.serialize();
}

void GameObjectEvent::deserialize(const std::vector<std::byte>& data) {
    _deserializer.reset();
    _deserializer.receive(data);
    _objId = _deserializer.readSint32();
}

