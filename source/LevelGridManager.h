//
//  LevelGridManager.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 5/12/25.
//
#ifndef __LEVEL_GRID_MANAGER_H__
#define __LEVEL_GRID_MANAGER_H__

#include <cugl/cugl.h>
#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Grid Manager
/**
* The grid manager for the grid used in building mode.
*/
class LevelGridManager {
public:
    /** Maps grid positions to all world objects */
    std::map<std::pair<int, int>, std::shared_ptr<Object>> posToWorldObjMap;
    /** Maps grid positions to moveable world objects */
    std::map<std::pair<int, int>, std::shared_ptr<Object>> posToObjMap;
    /** Maps grid positions to moveable art objects */
    std::map<std::pair<int, int>, std::vector<std::shared_ptr<Object>>> posToArtObjMap;
    /** Maps grid positions to whether there exists an object (moveable or non-moveable) that contains the grid box */
    std::map<std::pair<int, int>, bool> hasObjMap;
    /** Maps all world objects to bottom left position of objects */
    std::map<std::shared_ptr<Object>, std::pair<int, int>> worldObjToPosMap;
    /** Maps moveable world objects to bottom left position of objects */
    std::map<std::shared_ptr<Object>, std::pair<int, int>> objToPosMap;

private:
    /** Reference to building mode grid */
    std::shared_ptr<scene2::SceneNode> _grid;
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    /** The offset of the grid. */
    Vec2 _offset;
    /** The number of columns in the grid */
    float _columns;
    /** The size of the cell in Box2d units */
    const float CELL_SIZE = 1.0f;


protected:
    /** The sprite node that moves between grid cells */
    std::shared_ptr<scene2::SpriteNode> _spriteNode;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a grid manager with a grid.
     *
     * Each grid cell is a Box2d unit.
     *
     * @return  A newly allocated LevelGridManager
     */
    static std::shared_ptr<LevelGridManager> alloc(bool isLevelEditor, int columns, float scale, Vec2 offset, const std::shared_ptr<AssetManager>& assets) {
        auto manager = std::make_shared<LevelGridManager>();

        manager->_assets = assets;
        manager->_scale = scale;
        manager->_offset = offset;
        manager->_columns = columns;

        manager->_grid = scene2::SceneNode::alloc();
        manager->_grid->setScale(scale);
        manager->_grid->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        manager->_grid->setPosition(offset);
        // Vec2(0, 64.0f * 2)
        manager->_grid->setVisible(false);

        manager->initGrid(isLevelEditor);

        return manager;
    }

    /**
     * Initializes the grid layout on the screen for build mode.
     */
    void initGrid(bool isEditor);

    /** Clears the object maps */
    void clear();

#pragma mark -
#pragma mark Attribute Properties
    /**
     * Returns the entire grid node.
     *
     * @return the grid node
     */
    std::shared_ptr<scene2::SceneNode> getGridNode() {
        return _grid;
    }

    /**
     * Returns the number of columns in the grid.
     *
     * @return the number of columns
     */
    float getNumColumns() {
        return _columns;
    }

    /**
     * Adds an object to the corresponding cell at this row and column.
     *
     * @param cellPos   the cell position
     * @param item          the item of the corresponding object
     */
    void setObject(Vec2 cellPos, Item item);

    /** Gets the object in the cell at this row and column.
     * Does NOT work for ArtObjects - use a separate method for that.
     * @param cellPos    the cell position
     */
    std::shared_ptr<Object> getObject(Vec2 cellPos);
    /**
     * Sets the sprite node's visibility to false
     */
    void setSpriteInvisible();

#pragma mark -
#pragma mark Object Handling
    /**
     * Adds the non-moveable object's position to the position has object map.
     *
     *@param obj    the object
     */
    void addObject(std::shared_ptr<Object> obj);

    /**
     * Adds the object to the object map.
     *
     *@param cellPos    the cell position
     *@param obj    the object
     */
    void addMoveableObject(Vec2 cellPos, std::shared_ptr<Object> obj);

    /**
     * Removes the object from the object map, if it exists.
     *
     *@return   the object removed
     *
     *@param cellPos    the cell position
     */
    std::shared_ptr<Object> moveObject(Vec2 cellPos);

    /**
     * Removes the object from the world object map, if it exists.
     *
     *@return   the world object removed
     *
     *@param cellPos    the cell position
     */
    std::shared_ptr<Object> removeWorldObject(Vec2 cellPos);

    /**
     * Checks whether we can place the object in the cell position.
     *
     * @return false if there exists an object
     *
     * @param cellPos    the cell position
     * @param size          the amount of area this object takes up (including its movement)
     * @param item      the item type
     */
    bool canPlace(Vec2 cellPos, Size size, Item item);

    /**
     * Deletes the object at this cell position from the world.
     *
     * @param obj    the object
     */
    void deleteObject(std::shared_ptr<Object> obj);
};

#endif /* __LEVEL_GRID_MANAGER_H__ */
