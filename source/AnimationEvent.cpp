//
//  AnimationEvent.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/26/25.
//

#include "AnimationEvent.h"
#include "Message.h"

using namespace cugl::physics2::distrib;

std::shared_ptr<NetEvent> AnimationEvent::newEvent() {
    return std::make_shared<AnimationEvent>();
}

std::shared_ptr<AnimationEvent> AnimationEvent::allocAnimationEvent(int playerID,
                                                                    AnimationType animation,
                                                                    bool activate) {
    auto event = std::make_shared<AnimationEvent>();
    event->_playerID   = playerID;
    event->_animation  = animation;
    event->_activate    = activate;
    return event;
}

std::vector<std::byte> AnimationEvent::serialize() {
    _serializer.reset();
    _serializer.writeSint32(static_cast<Sint32>(_playerID));
    _serializer.writeSint32(static_cast<Sint32>(_animation));
    _serializer.writeBool(_activate);
    return _serializer.serialize();
}

void AnimationEvent::deserialize(const std::vector<std::byte>& data) {
    _deserializer.reset();
    _deserializer.receive(data);
    _playerID   = _deserializer.readSint32();
    _animation  = static_cast<AnimationType>(_deserializer.readSint32());
    _activate    = _deserializer.readBool();
}
