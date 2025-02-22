//
//  SSBGridManager.h
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 2/22/25.
//
#define __SSB_GRID_MANAGER_H__

#include <cugl/cugl.h>

using namespace cugl;

#pragma mark -
#pragma mark Grid Manager
/**
* The grid manager for the grid used in building mode.
*/
class GridManager {
private:
    /** Reference to building mode grid */
    std::shared_ptr<scene2::SceneNode> _grid;
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

protected:

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

        manager->_grid = scene2::SceneNode::alloc();
        manager->_grid->setScale(scale);
        manager->_grid->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        manager->_grid->setPosition(offset);
        manager->_grid->setVisible(false);

        manager->initGrid(rows, columns);

        return manager;
    }

    /**
     * Initializes the grid layout on the screen for build mode.
     */
    void initGrid(int rows, int columns);

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
     * Adds an object to the corresponding cell at this row and column.
     */
    void setObject(int row, int col);
};
