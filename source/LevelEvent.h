//
//  LevelEvent.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 5/3/25.
//

#ifndef LevelEvent_h
#define LevelEvent_h

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;


class LevelEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    
    int _levelNum;
    bool _showModal;
    bool _playPressed;
    
public:
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocLevelEvent(int levelNum, bool showModal, bool playPressed);
    
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
    
    /** Gets the level number of the event. */
    int getLevelNum() { return _levelNum; }
    
    /** Gets whether to show the modal. */
    bool getShowModal() { return _showModal; }
    
    /** Gets whether play was pressed. */
    bool getPlayPressed() { return _playPressed; }

};

#endif /* LevelEvent_h */
