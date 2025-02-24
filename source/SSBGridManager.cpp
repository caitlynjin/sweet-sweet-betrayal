//
//  SSBGridManager.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 2/22/25.
//
#include "SSBGridManager.h"

using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Asset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"


#pragma mark -
#pragma mark Constructors

/**
 * Initializes the grid layout on the screen for build mode.
 */
void GridManager::initGrid() {
    const std::shared_ptr<Texture> EARTH_IMAGE = _assets->get<Texture>(EARTH_TEXTURE);

    _grid->removeAllChildren();

    std::shared_ptr<scene2::GridLayout> gridLayout = scene2::GridLayout::alloc();
    gridLayout->setGridSize(_columns, _rows);

    for (int row = 0; row < _rows; ++row) {
        for (int col = 0; col < _columns; ++col) {
            Vec2 cellPos(col * CELL_SIZE, row * CELL_SIZE);

            std::shared_ptr<scene2::WireNode> cellNode = scene2::WireNode::allocWithPath(Rect(cellPos, Size(CELL_SIZE, CELL_SIZE)));
            cellNode->setColor(Color4::WHITE);
            cellNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
            cellNode->setPosition(cellPos);

            _grid->addChild(cellNode);
        }
    }

    // TODO: Remove later, testing with adding a sprite node to a cell
    Vec2 cellPos(3 * 1, 3 * 1);

    float textureWidth = EARTH_IMAGE->getWidth();
    float textureHeight = EARTH_IMAGE->getHeight();

    _spriteNode = scene2::SpriteNode::allocWithSheet(EARTH_IMAGE, 1, 1);
    _spriteNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _spriteNode->setScale(CELL_SIZE / textureWidth, CELL_SIZE / textureHeight);
    _spriteNode->setPosition(Vec2(0,0));
    _spriteNode->setVisible(false);

    _grid->addChild(_spriteNode);
}

#pragma mark -
#pragma mark Attribute Properties

/**
 * Adds an object to the corresponding cell at this row and column.
 */
void GridManager::setObject(Vec2 cellPos) {
    if (_spriteNode) {
        _spriteNode->setPosition(cellPos);
        _spriteNode->setVisible(true);
    }
}

/**
 * Sets the sprite node's visibility to false
 */
void GridManager::setSpriteInvisible(){
    _spriteNode->setVisible(false);
}
