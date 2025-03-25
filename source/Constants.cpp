//
//  Constants.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 3/3/25.
//
#include "Constants.h"
#include <cugl/cugl.h>
#include "string"

/** The key for the platform texture in the asset manager*/
#define TILE_TEXTURE   "tile"
/** The key for the 3x0.5 platform texture in the asset manager */
#define LOG_TEXTURE   "log_obstacle"
/** The key for the moving platform texture in the asset manager*/
#define GLIDING_LOG_TEXTURE   "gliding_log_obstacle"
/** Name of the wind texture*/
#define WIND_TEXTURE "up"
/** Name of the treasure texture */
#define TREASURE_TEXTURE "treasure"

namespace Constants {

/**
 * Convert a JsonType to the corresponding string.
 */
std::string jsonTypeToString(JsonType type) {
    switch (type) {
        case TILE:
            return "default";
        case PLATFORM_TILE:
            return "platform";
        case LOG:
            return "log";
        case GLIDING_LOG:
            return "gliding_log";
        case SPIKE_TILE:
            return "default";
    }
}

/*
    * Returns the additional scale factor needed to differentiate Box2D scaling on mobile and desktop.
    *
    * @return The scale difference for mobile vs desktop
    */
float getSystemScale() {
#ifdef CU_TOUCH_SCREEN
    return 1.0f;
#else
    return 2.0f;
#endif
}

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
        case SPIKE:
            return "spike";
        case TREASURE:
            return "treasure";
        case TILE_ALPHA:
            return "tile";
        default:
            return "unknown";
    }
}

/**
 * Gets the default size of this Item.
 */
cugl::Size itemToSize(Item item) {
    switch (item) {
        case PLATFORM:
            return cugl::Size(3, 1);
        case MOVING_PLATFORM:
            return cugl::Size(3, 1);
        case WIND:
            return cugl::Size(1, 1);
        case SPIKE:
            return cugl::Size(1, 1);
        case TREASURE:
            return cugl::Size(1, 1);
        case TILE_ALPHA:
            return cugl::Size(1, 1);
        case NONE:
            return cugl::Size(1, 1);
    }
}

/**
 * Gets the grid size of this item.
 * e.g. if the obstacle moves, the size will account for entire width/height of its movement as well.
 */
cugl::Size itemToGridSize(Item item) {
    switch (item) {
        case PLATFORM:
            return cugl::Size(3, 1);
        case MOVING_PLATFORM:
            return cugl::Size(6, 1);
        case WIND:
            return cugl::Size(1, 1);
        case SPIKE:
            return cugl::Size(1, 1);
        case TREASURE:
            return cugl::Size(1, 1);
        case TILE_ALPHA:
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
            return LOG_TEXTURE;
        case (MOVING_PLATFORM):
            return GLIDING_LOG_TEXTURE;
        case (WIND):
            return WIND_TEXTURE;
        case (SPIKE):
            return SPIKE_TILE_TEXTURE;
        case (TREASURE):
            return TREASURE_TEXTURE;
        case (TILE_ALPHA):
            return TILE_TEXTURE;
        case (NONE):
            return nullptr;
    }
}

}
