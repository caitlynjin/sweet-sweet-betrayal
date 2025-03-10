//
//  Constants.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/2/25.
//
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__
#include <cugl/cugl.h>
#include "string"

/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "gray"
/** The key for the platform texture in the asset manager*/
#define PLATFORM_TEXTURE   "platform"
/** The key for the 3x0.5 platform texture in the asset manager */
#define PLATFORM_LONG_TEXTURE   "platform_long"
/** The key for the moving platform texture in the asset manager*/
#define MOVING_TEXTURE   "moving"
/** Name of the wind texture*/
#define WIND_TEXTURE "up"

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

/**
 * Gets the default size of this Item..
 */
cugl::Size itemToSize(Item item);

/**
 * Returns the corresponding asset name to the item.
 *
 * @return the item's asset name
 *
 * @param item The item
 */
std::string itemToAssetName(Item item);

}

#endif /* __CONSTANTS_H__ */
