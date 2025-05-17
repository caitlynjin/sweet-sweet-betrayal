//
//  AnimationStateEvent.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 5/16/25.
//

#include "AnimationStateEvent.h"
#include "Message.h"
#include "PlayerModel.h"

using namespace cugl::physics2::distrib;

std::shared_ptr<NetEvent> AnimationStateEvent::newEvent() {
    return std::make_shared<AnimationStateEvent>();
}

std::shared_ptr<AnimationStateEvent> AnimationStateEvent::allocAnimationStateEvent(int playerID, PlayerModel::State state, bool facing) {
    auto event = std::make_shared<AnimationStateEvent>();
    event->_playerID   = playerID;
    event->_state  = state;
    event->_facing    = facing;
    return event;
}

std::vector<std::byte> AnimationStateEvent::serialize() {
    _serializer.reset();
    _serializer.writeSint32(static_cast<Sint32>(_playerID));
    _serializer.writeSint32(static_cast<Sint32>(_state));
    _serializer.writeBool(_facing);
    return _serializer.serialize();
}

void AnimationStateEvent::deserialize(const std::vector<std::byte>& data) {
    _deserializer.reset();
    _deserializer.receive(data);
    _playerID   = _deserializer.readSint32();
    _state  = static_cast<PlayerModel::State>(_deserializer.readSint32());
    _facing    = _deserializer.readBool();
}
