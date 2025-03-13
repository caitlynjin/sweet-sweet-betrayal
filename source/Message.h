//
//  Message.h
//  SweetSweetBetrayal
//
//  Created by Angelica Borowy on 3/9/25.
//

#ifndef Message_h
#define Message_h

/**
 * Enum representing different message types for network communication.
 */
enum class Message {
    BUILD_READY,      // Signal to build something
    MOVEMENT_END      // Signal that a player has ended movement phase by either dying or winning
};

#endif /* Message_h */
