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
    HOST_START,       // Signal that host has started the game
    BUILD_READY,      // Signal to build something
    MOVEMENT_END,     // Signal that a player has ended movement phase by either dying or winning
    TREASURE_TAKEN,   // Signal that a player has picked up a treasure
    TREASURE_LOST,    // Signal that a player has lost possession of a treasure
    SCORE_UPDATE,     // Signal a score update
    RESET_LEVEL       // Signal a level to reset
};

/**
 * Enum representing different color types for each player.
 */
enum class ColorType {
    RED,
    BLUE,
    GREEN,
    YELLOW
};

#endif /* Message_h */
