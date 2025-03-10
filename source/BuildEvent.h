//
//  BuildEvent.hpp
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 3/9/25.
//

#ifndef BuildEvent_hpp
#define BuildEvent_hpp

#include <stdio.h>
#include <cugl/cugl.h>
using namespace cugl;
using namespace cugl::physics2::distrib;

enum class BuildType {
    PLATFORM,
    MOVING_PLATFORM,
    WIND
};

enum class BuildAction {
    ADD,
    DELETE
};


class BuildEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    
    Vec2 _pos;
    BuildType _buildType;
    BuildAction _buildAction;
    
    
public:
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocBuildEvent(Vec2 pos, BuildType buildType, BuildAction buildAction);
    
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
    
    /** Gets the grid position of the event. */
    Vec2 getPos() { return _pos; }
    
    /** Get object type*/
    BuildType getBuildType() const {return _buildType;}
    
    /** Get build action*/
    BuildAction getBuildAction() const {return _buildAction;}
};

#endif /* BuildEvent_hpp */
