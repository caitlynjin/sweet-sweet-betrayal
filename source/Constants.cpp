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
        case TILE_ITEM:
            return "tile";
        case MUSHROOM:
            return "mushroom";
        case ART_OBJECT:
            return "art object";
        default:
            return "default";
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
        case TILE_ITEM:
            return cugl::Size(1, 1);
        case MUSHROOM:
            return cugl::Size(2, 1);
        case ART_OBJECT:
            return cugl::Size(1, 1);
        case NONE:
            return cugl::Size(1, 1);
        default:
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
        case TILE_ITEM:
            return cugl::Size(1, 1);
        case MUSHROOM:
            return cugl::Size(2, 1);
        case ART_OBJECT:
            return cugl::Size(1, 1);
        case NONE:
            return cugl::Size(1, 1);
        default:
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
            return FAN_TEXTURE;
        case (SPIKE):
            return SPIKE_TILE_TEXTURE;
        case (TREASURE):
            return TREASURE_TEXTURE;
        case (TILE_ITEM):
            return TILE_TEXTURE;
        case (MUSHROOM):
            return MUSHROOM_TEXTURE;
        case (ART_OBJECT):
            return EARTH_TEXTURE; // for debugging - this is not sustainable
        case (TILE_TOP):
            return TOP_TILE_TEXTURE;
        case (TILE_BOTTOM):
            return BOTTOM_TILE_TEXTURE;
        case (TILE_INNER):
            return INNER_TILE_TEXTURE;
        case (TILE_LEFT):
            return LEFT_TILE_TEXTURE;
        case (TILE_RIGHT):
            return RIGHT_TILE_TEXTURE;
        case (TILE_TOPLEFT):
            return TOPLEFT_TILE_TEXTURE;
        case (TILE_TOPRIGHT):
            return TOPRIGHT_TILE_TEXTURE;
        case (CRACK_1):
            return CRACK1_TEXTURE;
        case (CRACK_2):
            return CRACK2_TEXTURE;
        case (CRACK_3):
            return CRACK3_TEXTURE;
        case (CRACK_4):
            return CRACK4_TEXTURE;
        case (CRACK_5):
            return CRACK5_TEXTURE;
        case (CRACK_LARGE_1):
            return CRACKLARGE1_TEXTURE;
        case (MOSS_1):
            return MOSS1_TEXTURE;
        case (MOSS_2):
            return MOSS2_TEXTURE;
        case (ROCKY_1):
            return ROCKY1_TEXTURE;
        case (ROCKY_2):
            return ROCKY2_TEXTURE;
        case (NONE):
            return nullptr;
    }
}
bool itemIsArtObject(Item item)
{
    std::vector<Item> items = {
        ART_OBJECT,
        CRACK_1,
    CRACK_2,
    CRACK_3,
    CRACK_4,
    CRACK_5,
    CRACK_LARGE_1,
    MOSS_1,
    MOSS_2,
    ROCKY_1,
    ROCKY_2,
    };
    return std::find(items.begin(), items.end(), item) != items.end();
}



/**
 Returns whether a tag contains the player keyword.
 */
bool tagContainsPlayer(std::string tag){
    return tag.find("player") != std::string::npos;
}

}
