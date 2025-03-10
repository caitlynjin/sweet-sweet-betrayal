//
//  Constants.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/3/25.
//
#include "Constants.h"
#include <cugl/cugl.h>
#include "string"

namespace Constants {

/**
 * Convert an Item enum to the corresponding string.
 */
std::string itemToString(Item item) {
    switch (item) {
        case PLATFORM:
            return "platform";
        case MOVING_PLATFORM:
            return "moving platform";
        case WIND:
            return "wind";
        default:
            return "unknown";
    }
}

/**
 * Gets the default size of this Item..
 */
cugl::Size itemToSize(Item item) {
    switch (item) {
        case PLATFORM:
            return cugl::Size(3, 1);
        case MOVING_PLATFORM:
            return cugl::Size(1, 1);
        case WIND:
            return cugl::Size(1, 1);
        case NONE:
            return cugl::Size(1, 1);
    }
}

/**
 * Returns the corresponding asset name to the item.
 *
 * @return the item's asset name
 *
 * @param item The item
 */
std::string itemToAssetName(Item item) {
    switch (item){
        case (PLATFORM):
            return PLATFORM_LONG_TEXTURE;
        case (MOVING_PLATFORM):
            return MOVING_TEXTURE;
        case (WIND):
            return WIND_TEXTURE;
        case (NONE):
            return nullptr;
    }
}

}
