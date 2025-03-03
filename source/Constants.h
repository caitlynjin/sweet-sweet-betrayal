//
//  Constants.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/2/25.
//
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__
#include "string"

namespace Constants {

enum Item {
    /** A standard platform */
    PLATFORM,
    /** A moving platform */
    MOVING_PLATFORM,
    /** A wind object */
    WIND,
    /** No type */
    NONE
};

/**
 * Convert an Item enum to the corresponding string.
 */
std::string itemToString(Item item);

}

#endif /* __CONSTANTS_H__ */
