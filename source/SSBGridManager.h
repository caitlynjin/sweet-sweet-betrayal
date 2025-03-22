//
//  SSBGridManager.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 2/22/25.
//
#ifndef __SSB_GRID_MANAGER_H__
#define __SSB_GRID_MANAGER_H__

#include <cugl/cugl.h>
#include "Object.h"


using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Grid Manager
/**
* The grid manager for the grid used in building mode.
*/
class GridManager {
public:
    /** Maps grid positions to world objects */
    std::map<std::pair<int, int>, std::shared_ptr<Object>> posToObjMap;
    /** Maps world objects to bottom left position of objects */
    std::map<std::shared_ptr<Object>, std::pair<int, int>> objToOriginPosMap;

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
    /** The number of rows in the grid */
    float _rows;
    /** The size of the cell in Box2d units */
#ifndef CU_TOUCH_SCREEN
    const float CELL_SIZE = 2.0f;
#else
    const float CELL_SIZE = 1.0f;
#endif


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
     * @return  A newly allocated GridManager
     */
    static std::shared_ptr<GridManager> alloc(int rows, int columns, float scale, Vec2 offset, const std::shared_ptr<AssetManager>& assets) {
        auto manager = std::make_shared<GridManager>();

        manager->_assets = assets;
        manager->_scale = scale;
        manager->_offset = offset;
        manager->_rows = rows;
        manager->_columns = columns;

        manager->_grid = scene2::SceneNode::alloc();
        manager->_grid->setScale(scale);
        manager->_grid->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        manager->_grid->setPosition(offset);
        manager->_grid->setVisible(false);

        manager->initGrid();

        return manager;
    }

    /**
     * Initializes the grid layout on the screen for build mode.
     */
    void initGrid();

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
     * Returns the number of rows in the grid.
     *
     * @return the number of rows
     */
    float getNumRows() {
        return _rows;
    }

    /**
     * Adds an object to the corresponding cell at this row and column.
     *
     * @param cellPos   the cell position
     * @param item          the item of the corresponding object
     */
    void setObject(Vec2 cellPos, Item item);

    /**
     * Sets the sprite node's visibility to false
     */
    void setSpriteInvisible();

#pragma mark -
#pragma mark Object Handling

    /**
     * Adds the object to the object map.
     *
     *@param cellPos    the cell position
     *@param obj    the object
     */
    void addObject(Vec2 cellPos, std::shared_ptr<Object> obj);

    /**
     * Removes the object from the object map, if it exists.
     *
     *@return   the object removed
     *
     *@param cellPos    the cell position
     */
    std::shared_ptr<Object> removeObject(Vec2 cellPos);

    /**
     * Checks if there's an object in this grid position.
     *
     * @return true if there exists an object
     *
     * @param cellPos    the cell position
     */
    bool hasObject(Vec2 cellPos);

};

#endif /* __SSB_GRID_MANAGER_H__ */
