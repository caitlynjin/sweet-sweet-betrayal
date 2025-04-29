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
/** The animation texture for when the player is dying */
#define PLAYER_RED_DEATH_TEXTURE    "player-red-death"
// BLUE
/** The animation texture for when the player is idle */
#define PLAYER_BLUE_IDLE_TEXTURE    "player-blue-idle"
/** The animation texture for when the player is walking */
#define PLAYER_BLUE_WALK_TEXTURE    "player-blue-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_BLUE_GLIDE_TEXTURE    "player-blue-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_BLUE_JUMP_TEXTURE    "player-blue-jump"
/** The animation texture for when the player is dying */
#define PLAYER_BLUE_DEATH_TEXTURE    "player-blue-death"
// GREEN
/** The animation texture for when the player is idle */
#define PLAYER_GREEN_IDLE_TEXTURE    "player-green-idle"
/** The animation texture for when the player is walking */
#define PLAYER_GREEN_WALK_TEXTURE    "player-green-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_GREEN_GLIDE_TEXTURE    "player-green-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_GREEN_JUMP_TEXTURE    "player-green-jump"
/** The animation texture for when the player is dying */
#define PLAYER_GREEN_DEATH_TEXTURE    "player-green-death"
// YELLOW
/** The animation texture for when the player is idle */
#define PLAYER_YELLOW_IDLE_TEXTURE    "player-yellow-idle"
/** The animation texture for when the player is walking */
#define PLAYER_YELLOW_WALK_TEXTURE    "player-yellow-walk"
/** The animation texture for when the player is gliding */
#define PLAYER_YELLOW_GLIDE_TEXTURE    "player-yellow-glide"
/** The animation texture for when the player is jumping */
#define PLAYER_YELLOW_JUMP_TEXTURE    "player-yellow-jump"
/** The animation texture for when the player is dying */
#define PLAYER_YELLOW_DEATH_TEXTURE    "player-yellow-death"


/** The key for the tile texture in the asset manager*/
#define TILE_TEXTURE   "tile"
/** The key for the platform tile texture in the asset manager */
#define PLATFORM_TILE_TEXTURE   "platform_tile"
/** The key for the 3x0.5 platform texture in the asset manager */
#define LOG_TEXTURE   "log_obstacle"
/** The key for the inventory icon for the log */
#define LOG_ICON   "icon-log"
/** The key for the moving platform texture in the asset manager*/
#define GLIDING_LOG_TEXTURE   "gliding_log_obstacle"
/** Name of the fan texture for windobstacle*/
#define FAN_TEXTURE "static_fan"
/** Name of the gust texture for windobstacle animated*/
#define FAN_TEXTURE_ANIMATED "leaf_fan_spritesheet"
/** Name of the gust texture for windobstacle*/
#define GUST_TEXTURE "static_gust"
/**Name of the animated gust texture at various levels*/
#define WIND_LVL_1 "wind_blocked_at_2_spritesheet"
#define WIND_LVL_2 "wind_blocked_at_3_spritesheet"
#define WIND_LVL_3 "wind_blocked_at_4_spritesheet"
#define WIND_LVL_4 "wind_unobstructed_spritesheet"
/** The key for the inventory icon for the gliding log */
#define GLIDING_LOG_ICON   "icon-moving-log"
/** Name of the wind icon*/
#define WIND_ICON "icon-wind"
/** Name of the mushroom texture*/
#define MUSHROOM_TEXTURE "mushroom"
/** The key for the inventory icon for the mushroom */
#define MUSHROOM_ICON   "icon-mushroom"
/** The key for the spike texture in the asset manager */
#define SPIKE_TILE_TEXTURE "spike_tile"
/** The key for the thorn texture in the asset manager */
#define THORN_TEXTURE "thorns_obstacle"
/** The key for the inventory icon for the thorns */
#define THORN_TILE_ICON "icon-thorn"
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
/** The image for the green icon */
#define GREEN_ICON "green-icon"
/** The image for the yellow icon */
#define YELLOW_ICON "yellow-icon"
/** The image for the treasure icon */
#define TREASURE_ICON "treasure-icon"
/** The image for checkmark */
#define CHECKMARK "checkmark"
/** The image for the trash can closed icon */
#define TRASH_CLOSED "trash-closed"
/** The image for the trash can open icon */
#define TRASH_OPEN "trash-open"
/** The image for the top frame */
#define TOP_FRAME "top-frame"
/** The image for the left frame */
#define LEFT_FRAME "left-frame"
/** The image for the bottom frame */
#define BOTTOM_FRAME "bottom-frame"
/** The image for the timer */
#define TIMER "timer"


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
/** The font for the timer */
#define TIMER_FONT  "yeasty flavorsRegular66.53518676757812"


#pragma mark -
#pragma mark Environment Textures
/** The key for the background texture in the asset manager */
#define BACKGROUND_TEXTURE    "background"
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"

#define TOP_TILE_TEXTURE "tile-top-edge"
#define BOTTOM_TILE_TEXTURE "tile-bottom-edge"
#define INNER_TILE_TEXTURE "tile-inner"
#define LEFT_TILE_TEXTURE "tile-left-edge"
#define RIGHT_TILE_TEXTURE "tile-right-edge"
#define TOPLEFT_TILE_TEXTURE "tile-topleft-corner"
#define TOPRIGHT_TILE_TEXTURE "tile-topright-corner"
#define CRACK1_TEXTURE "decoration-crack-1"
#define CRACK2_TEXTURE "decoration-crack-2"
#define CRACK3_TEXTURE "decoration-crack-3"
#define CRACK4_TEXTURE "decoration-crack-4"
#define CRACK5_TEXTURE "decoration-crack-5"
#define CRACKLARGE1_TEXTURE "decoration-crack-large-1"
#define MOSS1_TEXTURE "decoration-moss-1"
#define MOSS2_TEXTURE "decoration-moss-2"
#define ROCKY1_TEXTURE "decoration-rocky-1"
#define ROCKY2_TEXTURE "decoration-rocky-2"


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
#define SCOREBOARD_COUNT 200

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
        /** The thorn */
        THORN,
        /** A treasure */
        TREASURE,
        /** A tile, representing a 1x1 platform. There should eventually be multiple tiles. */
        TILE_ITEM,
        /** A mushroom */
        MUSHROOM,
        /** An art object */
        ART_OBJECT,

        TILE_TOP,
        TILE_BOTTOM,
        TILE_INNER,
        TILE_LEFT,
        TILE_RIGHT,
        TILE_TOPLEFT,
        TILE_TOPRIGHT,
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

    /** Gets whether or not this item is an art object. */
    bool itemIsArtObject(Item item);

    /**
     * Returns the corresponding asset name to the item.
     *
     * @return the item's asset name
     *
     * @param item The item
     */
    std::string itemToAssetName(Item item);

    static std::vector<std::string> xOffsetArtObjects{
        "crackLarge1"
    };

        static std::vector<std::string> yOffsetArtObjects{
            "crack1",
            "crack2",
            "crack3",
            "crack4",
            "crack5",
            "crackLarge1",
            "moss1",
            "moss2"
    };

static std::map<std::string, int> jsonTypeToLayer {
    {"default", 1},
    {"crack1", 1},
    {"crack2", 1},
    {"crack3", 1},
    {"crack4", 1},
    {"crack5", 1},
    {"crackLarge1", 1},
    {"moss1", 1},
    {"moss2", 1},
    {"rocky1", 1},
    {"rocky2",1}
};

static std::map<std::string, std::string> jsonTypeToAsset
{
    {"default", EARTH_TEXTURE},
    { "tileTop", TOP_TILE_TEXTURE },
    {"tileBottom", BOTTOM_TILE_TEXTURE},
    {"tileInner", INNER_TILE_TEXTURE},
    {"tileLeft", LEFT_TILE_TEXTURE},
    {"tileRight", RIGHT_TILE_TEXTURE},
    {"tileTopLeft", TOPLEFT_TILE_TEXTURE},
    {"tileTopRight", TOPRIGHT_TILE_TEXTURE},
    {"crack1", CRACK1_TEXTURE},
    {"crack2", CRACK2_TEXTURE},
    {"crack3", CRACK3_TEXTURE},
    {"crack4", CRACK4_TEXTURE},
    {"crack5", CRACK5_TEXTURE},
    {"crackLarge1", CRACKLARGE1_TEXTURE},
    {"moss1", MOSS1_TEXTURE},
    {"moss2", MOSS2_TEXTURE},
    {"rocky1", ROCKY1_TEXTURE},
    {"rocky2", ROCKY2_TEXTURE}
};

static std::map<std::string, Item> jsonTypeToItemType
{
    {"default", Item::ART_OBJECT},
    {"tileTop", Item::TILE_TOP},
    {"tileBottom", Item::TILE_BOTTOM},
    {"tileInner", Item::TILE_INNER},
    {"tileLeft", Item::TILE_LEFT},
    {"tileRight", Item::TILE_RIGHT},
    {"tileTopLeft", Item::TILE_TOPLEFT},
    {"tileTopRight", Item::TILE_TOPRIGHT},
    {"crack1", Item::CRACK_1},
    {"crack2", Item::CRACK_2},
    {"crack3", Item::CRACK_3},
    {"crack4", Item::CRACK_4},
    {"crack5", Item::CRACK_5},
    {"crackLarge1", Item::CRACK_LARGE_1},
    {"moss1", Item::MOSS_1},
    {"moss2", Item::MOSS_2},
    {"rocky1", Item::ROCKY_1},
    {"rocky2", Item::ROCKY_2}
};

/**
 Returns whether a tag contains the player keyword.
 */
bool tagContainsPlayer(std::string tag);

}



#endif /* __CONSTANTS_H__ */
