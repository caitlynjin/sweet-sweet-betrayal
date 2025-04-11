//
//  ScoreEvent.h
//  SweetSweetBetrayal
//
//  Created by jessie jia on 4/9/25.
//

#ifndef SCOREEVENT_H
#define SCOREEVENT_H

#include <stdio.h>
#include <cugl/cugl.h>
#include "Message.h"
using namespace cugl;
using namespace cugl::physics2::distrib;


class ScoreEvent : public NetEvent {
    
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    
    int _playerID;
    
    
    
    int roundNum;
    
public:
    enum ScoreType {
        END,          // reach end without treasure: 1 point
        END_TREASURE, // reach end with treasure: 3 points
        DEAD,          // dead: 0 points
        NONE
    } scoreType;
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocScoreEvent(int playerID, ScoreType type, int roundNum);
    
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
    
    /** Gets the player id of the event. */
    int getPlayerID() { return _playerID; }
    
    /** Gets the score type of the event. */
    ScoreType getScoreType() const { return scoreType; }

    /** Gets the number of points for this event based on score type. */
    int getScore() const;

    /** Gets the round number of the event. */
    int getRoundNumber() const { return roundNum; }
};
#endif /* ScoreEvent_hpp */
