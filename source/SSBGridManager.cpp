//
//  SSBGridManager.cpp
//  SweetSweetBetrayal
//
//  Created by Caitlyn Jin on 2/22/25.
//
#include "SSBGridManager.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace Constants;

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

    // Set grid to be shown initially
    _grid->setVisible(true);

    // Initialize sprite node
    Vec2 cellPos(0, 0);

    const std::shared_ptr<Texture> image = _assets->get<Texture>(PLATFORM_TEXTURE);
    float textureWidth = image->getWidth();
    float textureHeight = image->getHeight();

    _spriteNode = scene2::SpriteNode::allocWithSheet(image, 1, 1);
    _spriteNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _spriteNode->setContentSize(image->getSize());
    _spriteNode->setScale(CELL_SIZE / textureWidth, CELL_SIZE / textureHeight);
    _spriteNode->setPosition(cellPos);
    _spriteNode->setVisible(false);

    _grid->addChild(_spriteNode);
}

#pragma mark -
#pragma mark Attribute Properties

/**
 * Adds an object to the corresponding cell at this row and column.
 *
 * @param cellPos   the cell position
 * @param item          the item of the corresponding object
 */
void GridManager::setObject(Vec2 cellPos, Item item) {
    if (_spriteNode) {
        auto image = _assets->get<Texture>(itemToAssetName(item));

        float textureWidth = image->getWidth();
        float textureHeight = image->getHeight();
        Size itemSize = itemToSize(item);

        _spriteNode->setPosition(cellPos);
        _spriteNode->setTexture(image);
        _spriteNode->setContentSize(image->getSize());
        _spriteNode->setScale(itemSize.width / textureWidth, itemSize.height / textureHeight);
        _spriteNode->setVisible(true);
    }
}

/**
 * Sets the sprite node's visibility to false
 */
void GridManager::setSpriteInvisible(){
    _spriteNode->setVisible(false);
}

#pragma mark -
#pragma mark Object Handling

/**
 * Adds the object to the object map to this location.
 *
 *@param cellPos    the cell position
 *@param obj    the object
 */
void GridManager::addObject(Vec2 cellPos, std::shared_ptr<Object> obj) {
    auto originPosPair = std::make_pair(cellPos.x, cellPos.y);

    // Add the origin position of the object
    objToOriginPosMap[obj] = originPosPair;

    // Add the object to every position it exists in
    for (int i = 0; i < obj->getSize().getIWidth(); i++) {
        for (int j = 0; j < obj->getSize().getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(cellPos.x + i, cellPos.y + j);
            posToObjMap[posPair] = obj;
        }
    }
};

/**
 * Removes the object from the object map, if it exists.
 *
 *@return   the object removed, or `nullptr` if it does not exist.
 *
 *@param cellPos    the cell position
 *@param col    the cell column to add the object to
 */
std::shared_ptr<Object> GridManager::removeObject(Vec2 cellPos) {
    // Find object in object map
    auto posPair = std::make_pair(cellPos.x, cellPos.y);

    auto it = posToObjMap.find(posPair);
    if (it == posToObjMap.end()) {
        // If unable to find object
        return nullptr;
    }

    std::shared_ptr<Object> obj = it->second;
    // TODO: Remove this
//    posToObjectMap.erase(it);

    // Clear all positions the object occupies
    auto originPosX = objToOriginPosMap[obj].first;
    auto originPosY = objToOriginPosMap[obj].second;

    for (int i = 0; i < obj->getSize().getIWidth(); i++) {
        for (int j = 0; j < obj->getSize().getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(originPosX + i, originPosY + j);
            posToObjMap.erase(posPair);
        }
    }

    // Clear the origin position of the object
    objToOriginPosMap.erase(obj);

    return obj;
};

/**
 * Checks if there's an object in this grid position.
 *
 * @return true if there exists an object
 *
 * @param cellPos    the cell position
 */
bool GridManager::hasObject(Vec2 cellPos) {
    // Find object in object map
    auto posPair = std::make_pair(cellPos.x, cellPos.y);

    auto it = posToObjMap.find(posPair);
    if (it == posToObjMap.end()) {
        // If unable to find object
        return false;
    }

    return true;
};
