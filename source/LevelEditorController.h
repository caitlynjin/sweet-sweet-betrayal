//
//  LevelEditorController.h
//  SweetSweetBetrayal
//
//  Created by Benjamin Isaacson on 3/28/25.
//

#ifndef __SSB_LEVEL_EDITOR_CONTROLLER_H__
#define __SSB_LEVEL_EDITOR_CONTROLLER_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "Constants.h"
#include "SSBInput.h"
#include "LevelGridManager.h"
#include "NetworkController.h"
#include "ObjectController.h"
#include "LevelEditorScene.h"
#include "LevelEditorUIScene.h"
#include "SoundController.h"

using namespace cugl;
using namespace Constants;
using namespace cugl::physics2::distrib;

/**
 * This class is the controller. It probably shouldn't extend Scene2, but that's what GameController does.
 */
class LevelEditorController : public scene2::Scene2 {

protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    std::shared_ptr<LevelGridManager> _gridManager;
    std::shared_ptr<ObjectController> _objectController;
    std::shared_ptr<NetworkController> _networkController;

    /** Controller for handling audio logic */
    std::shared_ptr<SoundController> _sound;
    /** The network  */
    std::shared_ptr<NetEventController> _network;
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<PlatformInput> _input;
    LevelEditorScene _levelEditorScene;
    LevelEditorUIScene _uiScene;

    std::vector<std::shared_ptr<Object>> _objects;




    // Left in here in case it helps with drawing the sprite too (e.g. position of obstacle)
    /** Reference to the goalDoor (for collision detection) */
    std::shared_ptr<Object>    _goalDoor;

    /** The selected item in build mode (new object) */
    Item _selectedItem = NONE;
    /** The selected object in build mode (object being moved) */
    std::shared_ptr<Object> _selectedObject;
    /** Previous position of object in build phase */
    Vec2 _prevPos = Vec2(0, 0);
    /** The number of items currently placed */
    int _itemsPlaced = 0;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    /** The offset from the world */
    Vec2 _offset;

    /** Reference to background scene */
    scene2::Scene2 _backgroundScene;
    /** Reference to the background */
    std::shared_ptr<scene2::PolygonNode> _background;

    /** The Box2D world */
    std::shared_ptr<cugl::physics2::distrib::NetWorld> _world;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates the controller.
     */
    LevelEditorController();


    bool init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound, bool levelEditing);
    
    bool init(const std::shared_ptr<AssetManager>& assets,
        const Rect& rect, const Vec2& gravity, const std::shared_ptr<NetworkController> networkController, std::shared_ptr<SoundController> sound, bool levelEditing);
    /**
     * Initializes the build-related controller contents.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool initBuildingLogic(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::shared_ptr<LevelGridManager> gridManager, std::shared_ptr<ObjectController> objectController, std::shared_ptr<NetworkController> networkController, std::shared_ptr<Camera> camera);

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);

    void fixedUpdate(float dt);

    void postUpdate(float dt);

    void setSpriteBatch(const shared_ptr<SpriteBatch>& batch);

    void render();

    void eraseObjects(Vec2 dragOffset);

#pragma mark -
#pragma mark Helpers

    /**
     * Creates an item of type item and places it at the grid position.
     *
     *@return the object being placed and created
     *
     * @param gridPos   The grid position to place the item at
     * @param item  The type of the item to be placed/created
     */
    std::shared_ptr<Object> placeItem(Vec2 gridPos, Item item);

    /**
     * Snaps the Box2D position to within the bounds of the build phase grid.
     *
     * @return the grid position
     *
     * @param screenPos    The screen position
     * @param item               The selected item being snapped to the grid
     */
    Vec2 snapToGrid(const Vec2& gridPos, Item item);


};

#endif /* __SSB_LEVEL_EDITOR_CONTROLLER_H__ */
