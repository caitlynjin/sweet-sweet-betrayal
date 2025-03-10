//
//  BuildEvent.cpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 3/9/25.
//

#include "BuildEvent.h"
using namespace cugl::physics2::distrib;

std::shared_ptr<NetEvent> BuildEvent::newEvent(){
    return std::make_shared<BuildEvent>();
}

std::shared_ptr<NetEvent> BuildEvent::allocBuildEvent(Vec2 pos, BuildType buildType, BuildAction buildAction) {
    auto event = std::make_shared<BuildEvent>();
    event->_pos = pos;
    event->_buildType = buildType;
    event->_buildAction  = buildAction;
    return event;
}

std::vector<std::byte> BuildEvent::serialize() {
    _serializer.reset();
    
    _serializer.writeFloat(_pos.x);
    _serializer.writeFloat(_pos.y);
    
    // convert build type to integer
    _serializer.writeSint32(static_cast<Sint32>(_buildType));
    _serializer.writeSint32(static_cast<Sint32>(_buildAction));

    return _serializer.serialize();
}

void BuildEvent::deserialize(const std::vector<std::byte>& data) {
    _deserializer.reset();
    _deserializer.receive(data);
    
    float x = _deserializer.readFloat();
    float y = _deserializer.readFloat();
    _pos = Vec2(x, y);
    
    int typeInt = _deserializer.readSint32();
    _buildType = static_cast<BuildType>(typeInt);
    int actionInt = _deserializer.readSint32();
    _buildAction = static_cast<BuildAction>(actionInt);
}
