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


#pragma mark -
#pragma mark Asset Constants


#pragma mark -
#pragma mark Identification Constants
/** The name of a wall (for object identification) */
#define WALL_NAME "wall"
/** Name of the wind object(for identification)*/
#define WIND_NAME "gust"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME "platform"


#pragma mark -
#pragma mark Object Textures

// PLAYER TEXTURES
/** The texture for the character avatar */
#define PLAYER_TEXTURE    "dude"
// RED
/** The animation texture for when the player is idle */
#define PLAYER_RED_IDLE_TEXTURE    "player-red-idle"
/** The animation texture for when the player is walking */
#define PLAYER_RED_WALK_TEXTURE    "player-red-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_RED_GLIDE_TEXTURE    "player-red-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_RED_JUMP_TEXTURE    "player-red-jump"
// BLUE
/** The animation texture for when the player is idle */
#define PLAYER_BLUE_IDLE_TEXTURE    "player-blue-idle"
/** The animation texture for when the player is walking */
#define PLAYER_BLUE_WALK_TEXTURE    "player-blue-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_BLUE_GLIDE_TEXTURE    "player-blue-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_BLUE_JUMP_TEXTURE    "player-blue-jump"
// GREEN
/** The animation texture for when the player is idle */
#define PLAYER_GREEN_IDLE_TEXTURE    "player-green-idle"
/** The animation texture for when the player is walking */
#define PLAYER_GREEN_WALK_TEXTURE    "player-green-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_GREEN_GLIDE_TEXTURE    "player-green-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_GREEN_JUMP_TEXTURE    "player-green-jump"
// YELLOW
/** The animation texture for when the player is idle */
#define PLAYER_YELLOW_IDLE_TEXTURE    "player-yellow-idle"
/** The animation texture for when the player is walking */
#define PLAYER_YELLOW_WALK_TEXTURE    "player-yellow-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_YELLOW_GLIDE_TEXTURE    "player-yellow-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_YELLOW_JUMP_TEXTURE    "player-yellow-jump"


/** The key for the tile texture in the asset manager*/
#define TILE_TEXTURE   "tile"
/** The key for the platform tile texture in the asset manager */
#define PLATFORM_TILE_TEXTURE   "platform_tile"
/** The key for the 3x0.5 platform texture in the asset manager */
#define LOG_TEXTURE   "log_obstacle"
/** The key for the moving platform texture in the asset manager*/
#define GLIDING_LOG_TEXTURE   "gliding_log_obstacle"
/** Name of the wind texture*/
#define WIND_TEXTURE "up"
/** Name of the mushroom texture*/
#define MUSHROOM_TEXTURE "mushroom"
/** The key for the spike texture in the asset manager */
#define SPIKE_TILE_TEXTURE "spike_tile"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE    "goal"
/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"
/** The key for the treasure texture in the asset manager */
#define TREASURE_TEXTURE    "treasure"
/** The key for the platform texture in the asset manager*/
#define PLATFORM_TEXTURE   "platform"
/** The key for the 3x0.5 platform texture in the asset manager */
#define PLATFORM_LONG_TEXTURE   "platform_long"
/** The key for the moving platform texture in the asset manager*/
#define MOVING_TEXTURE   "moving"


#pragma mark -
#pragma mark UI Textures
/** The image for the left dpad/joystick */
#define LEFT_IMAGE      "dpad_left"
/** The image for the empty gem */
#define EMPTY_IMAGE      "gemEmpty"
/** The image for the full gem */
#define FULL_IMAGE      "gemFull"
/** The image for the right dpad/joystick */
#define RIGHT_IMAGE "dpad_right"
/** The image for the ready button */
#define READY_BUTTON "ready_button"
/** The image for the jump button */
#define JUMP_BUTTON "jump-button"
/** The image for the glide button */
#define GLIDE_BUTTON "glide-button"
/** The image for the progress bar */
#define PROGRESS_BAR "progress-bar"
/** The image for the red icon */
#define RED_ICON "red-icon"
/** The image for the blue icon */
#define BLUE_ICON "blue-icon"
/** The image for the treasure icon */
#define TREASURE_ICON "treasure-icon"
/** The image for the trash can icon */
#define TRASH "trash"


#pragma mark -
#pragma mark Text Constants
/** The message for winning the game */
#define WIN_MESSAGE "VICTORY!"
/** The color of the win message */
#define WIN_COLOR Color4::YELLOW
/** The message for losing the game */
#define LOSE_MESSAGE "FAILURE!"
/** The color of the lose message */
#define LOSE_COLOR      Color4::RED
/** The color of the info labels */
#define INFO_COLOR      Color4::WHITE
// Fonts
/** The font for victory/failure messages */
#define MESSAGE_FONT    "retro"
/** The font for Round and Gem info */
#define INFO_FONT    "marker"


#pragma mark -
#pragma mark Environment Textures
/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "gray"


#pragma mark -
#pragma mark Physics Constants
/** The new heavier gravity for this world (so it is not so floaty) */
#define DEFAULT_GRAVITY -28.9f
/** The density for most physics objects */
#define BASIC_DENSITY 0.0f
/** The density for a bullet */
#define HEAVY_DENSITY 10.0f
/** Friction of most platforms */
#define BASIC_FRICTION 0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION 0.1f


#pragma mark -
#pragma mark Timer Constants
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT 240
/** The number of frame to wait before reinitializing the game */
#define SCOREBOARD_COUNT 150

#pragma mark -
#pragma mark Build Phase Constants
/** The maximum number of rows on the grid */
#define MAX_ROWS 9
/** The number of rows on the bottom of the grid removed for build phase */
#define ROW_OFFSET_BOT 2
/** The number of rows on the top of the grid removed for build phase */
#define ROW_OFFSET_TOP 1


#pragma mark -
#pragma mark Audio Constants
/** The key the basic game music */
#define GAME_MUSIC "game"
/** The key the victory game music */
#define WIN_MUSIC "win"
/** The key the failure game music */
#define LOSE_MUSIC "lose"
/** The sound effect for firing a bullet */
#define PEW_EFFECT "pew"
/** The sound effect for a bullet collision */
#define POP_EFFECT "pop"
/** The sound effect for jumping */
#define JUMP_EFFECT "jump"
/** The volume for the music */
#define MUSIC_VOLUME 0.7f
/** The volume for sound effects */
#define EFFECT_VOLUME 0.8f


#pragma mark -
#pragma mark Debug Constants
/** Color to outline the physics nodes */
#define DEBUG_COLOR Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY 192


#pragma mark -
#pragma mark Collision Filtering Bits

#define CATEGORY_DEFAULT    0x0001  // Every object has this automatically as default
#define CATEGORY_PLAYER     0x0002




#pragma mark -
#pragma mark ENUMS


namespace Constants {

enum JsonType {
    /** 1x1 tile */
    TILE,
    /** 3x0.5 platform tile */
    PLATFORM_TILE,
    /** 3x0.5 log obstacle */
    LOG,
    /** 3x0.5 gliding log obstacle */
    GLIDING_LOG,
    /** 1x1 spike tile */
    SPIKE_TILE
};

/**
 * Convert a JsonType to the corresponding string.
 */
std::string jsonTypeToString(JsonType type);

/*
    * Returns the additional scale factor needed to differentiate Box2D scaling on mobile and desktop.
    *
    * @return The scale difference for mobile vs desktop
    */
float getSystemScale();

enum Item {
    /** A standard platform */
    PLATFORM,
    /** A moving platform */
    MOVING_PLATFORM,
    /** A wind object */
    WIND,
    /** A spike */
    SPIKE,
    /** A treasure */
    TREASURE,
    /** A tile, representing a 1x1 platform. There should eventually be multiple tiles. */
    TILE_ITEM,
    /** A mushroom */
    MUSHROOM,
    /** No type */
    NONE
};



#pragma mark -
#pragma mark Identification Functions

/**
 * Convert an Item enum to the corresponding string.
 */
std::string itemToString(Item item);

/**
 * Gets the default size of this Item..
 */
cugl::Size itemToSize(Item item);

/**
 * Gets the grid size of this item.
 * e.g. if the obstacle moves, the size will account for entire width/height of its movement as well.
 */
cugl::Size itemToGridSize(Item item);

/**
 * Returns the corresponding asset name to the item.
 *
 * @return the item's asset name
 *
 * @param item The item
 */
std::string itemToAssetName(Item item);

/**
 Returns whether a tag contains the player keyword.
 */
bool tagContainsPlayer(std::string tag);

}



#endif /* __CONSTANTS_H__ */
