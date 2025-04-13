//
//  ScoreEvent.cpp
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/9/25.
//

#include "ScoreEvent.h"
#include "Message.h"
using namespace cugl::physics2::distrib;

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> ScoreEvent::newEvent() {
    return std::make_shared<ScoreEvent>();
}

std::shared_ptr<NetEvent> ScoreEvent::allocScoreEvent(int playerID, ScoreType type, int roundNum) {
    auto event = std::make_shared<ScoreEvent>();
    event->_playerID = playerID;
    event->scoreType = type;
    event->roundNum = roundNum;
    return event;
}

std::vector<std::byte> ScoreEvent::serialize() {
    _serializer.reset();
    _serializer.writeSint32(static_cast<Sint32>(_playerID));
    _serializer.writeSint32(static_cast<Sint32>(scoreType));
    _serializer.writeSint32(static_cast<Sint32>(roundNum));
    return _serializer.serialize();
}

void ScoreEvent::deserialize(const std::vector<std::byte>& data) {
    _deserializer.reset();
    _deserializer.receive(data);
    _playerID = _deserializer.readSint32();
    scoreType = static_cast<ScoreType>(_deserializer.readSint32());
    roundNum = _deserializer.readSint32();
}

int ScoreEvent::getScore() const {
    switch (scoreType) {
        case END: // reach end without treasure: 1 point
            return 1;
        case END_TREASURE: // reach end with treasure: 3 points
            return 3;
        case DEAD: // dead: 0 points
            return 0;
        default:
            return 0;
    }
}
