#ifndef MushroomBounceEvent_h
#define MushroomBounceEvent_h

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;

class MushroomBounceEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;

    Vec2 _position;

public:
    std::shared_ptr<NetEvent> newEvent() override {
        return std::make_shared<MushroomBounceEvent>();
    }

    static std::shared_ptr<MushroomBounceEvent> allocMushroomBounceEvent(Vec2 position) {
        auto event = std::make_shared<MushroomBounceEvent>();
        event->_position = position;
        return event;
    }

    std::vector<std::byte> serialize() override {
        _serializer.reset();
        _serializer.writeFloat(_position.x);
        _serializer.writeFloat(_position.y);
        return _serializer.serialize();
    }

    void deserialize(const std::vector<std::byte>& data) override {
        _deserializer.reset();
        _deserializer.receive(data);
        _position.x = _deserializer.readFloat();
        _position.y = _deserializer.readFloat();
    }

    Vec2 getPosition() const {
        return _position;
    }
};

#endif /* MushroomBounceEvent_h */
