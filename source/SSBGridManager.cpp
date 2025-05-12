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
/** The key for the 1x1 tile texture in the asset manager */
#define TILE_TEXTURE   "tile"


#pragma mark -
#pragma mark Constructors

/**
 * Initializes the grid layout on the screen for build mode.
 */
void GridManager::initGrid(bool isLevelEditor) {
    _grid->removeAllChildren();

    std::shared_ptr<scene2::GridLayout> gridLayout = scene2::GridLayout::alloc();
    gridLayout->setGridSize(_columns, MAX_ROWS);

    for (int row = 0; row < MAX_ROWS; ++row) {
        for (int col = 0; col < _columns; ++col) {
            if (!isLevelEditor && (row < ROW_OFFSET_BOT || row >= MAX_ROWS - ROW_OFFSET_TOP)) {
                continue;
            }

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

    const std::shared_ptr<Texture> image = _assets->get<Texture>(TILE_TEXTURE);
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

/**
 * The method called to update the grid.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GridManager::update(float timestep) {
    // Ensure objects in the world are added to grid
    for (const auto& obs : _world->getObstacles()) {
        std::shared_ptr<Object> object = std::static_pointer_cast<Object>(obs);
        Vec2 cellPos = object->getPositionInit();
        auto originPosPair = std::make_pair(cellPos.x, cellPos.y);

        auto it = worldObjToPosMap.find(object);
        if (it == worldObjToPosMap.end()) {
            // Unable to find object on grid
            CULog("Add created object to grid");
            // TODO: Removing this fixed issue??? Need to now fix reset of moveable objects each round
//            addObject(object);
        } else if (originPosPair != worldObjToPosMap[object]) {
            CULog("originPosPair: (%f, %f)", originPosPair.first, originPosPair.second);
//            CULog("pos on worldOBjToPosMap: (%f, %f)", worldObjToPosMap[object].first, worldObjToPosMap[object].second);

            // Update object position on map
            CULog("Updating object position");
            moveWorldObject(object);
            addObject(object);
        }
    }

    // Convert Obstacles to Objects
    std::unordered_set<std::shared_ptr<Object>> objects;
    for (const auto& obs : _world->getObstacles()) {
        objects.insert(std::static_pointer_cast<Object>(obs));
    }

    // Make a copy of the keys to avoid iterator invalidation
    std::vector<std::shared_ptr<Object>> keysToCheck;
    for (const auto& objPosPair : worldObjToPosMap) {
        keysToCheck.push_back(objPosPair.first);
    }

    // Ensure objects NOT in the world are removed from the grid
    for (const auto& obj : keysToCheck) {
        if (objects.find(obj) == objects.end()) {
            CULog("Removed deleted object from grid");
            moveWorldObject(obj);
            deleteObject(obj);
        }
    }
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
        auto image = _assets->get<Texture>(itemToAssetNameMap[item]);
        if (image == nullptr) {
            CULog("You likely forgot to add this item to itemToAssetNameMap");
            return;
        }

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

/** Gets the object in the cell at this row and column .
 * @param cellPos    the cell position
 */
std::shared_ptr<Object> GridManager::getObject(Vec2 cellPos) {
    return posToObjMap[std::make_pair(cellPos.x, cellPos.y)];
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
 * Adds the non-moveable object's position to the position has object map.
 *
 *@param obj    the object
 */
void GridManager::addObject(std::shared_ptr<Object> obj) {
    Vec2 cellPos = obj->getPositionInit();
    Size size = obj->getSize();
    auto originPosPair = std::make_pair(cellPos.x, cellPos.y);

    // Add the origin position of the object
    worldObjToPosMap[obj] = originPosPair;
    CULog("updated position to (%f, %f)", originPosPair.first, originPosPair.second);

    // Add the object to every position it exists in
    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(cellPos.x + i, cellPos.y + j);
            if (itemIsArtObject(obj->getItemType())) {
                posToArtObjMap[posPair].push_back(obj);
            }
            else {
                posToWorldObjMap[posPair] = obj;
                hasObjMap[posPair] = true;
            }
        }
    }
}

/**
 * Adds the moveable object to the object map to this location.
 *
 *@param cellPos    the cell position
 *@param obj    the object
 */
void GridManager::addMoveableObject(Vec2 cellPos, std::shared_ptr<Object> obj) {
    auto originPosPair = std::make_pair(cellPos.x, cellPos.y);
    Size size = itemToGridSize(obj->getItemType());

    std::string x = std::to_string(cellPos.x);
    std::string y = std::to_string(cellPos.y);
    CULog("%s", x.c_str());
    CULog("%s", y.c_str());

    Vec2 prevCellPos = obj->getPositionInit();
    auto prevOriginPosPair = std::make_pair(prevCellPos.x, prevCellPos.y);

    // Clear all positions the object occupies
    auto originPosX = objToPosMap[obj].first;
    auto originPosY = objToPosMap[obj].second;

    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(originPosX + i, originPosY + j);

            posToWorldObjMap.erase(posPair);
            posToObjMap.erase(posPair);
            hasObjMap.erase(posPair);
        }
    }

    // Clear the origin position of the object
    objToPosMap.erase(obj);
//    worldObjToPosMap.erase(obj);




    // Add the origin position of the object
    objToPosMap[obj] = originPosPair;
    worldObjToPosMap[obj] = originPosPair;

    // Add the object to every position it exists in
    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(cellPos.x + i, cellPos.y + j);
            if (itemIsArtObject(obj->getItemType())) {
                posToArtObjMap[posPair].push_back(obj);
            }
            else {
                posToObjMap[posPair] = obj;
                posToWorldObjMap[posPair] = obj;
                hasObjMap[posPair] = true;
            }
            
        }
    }
}

/**
 * Removes the moveable object from the object map, if it exists.
 *
 *@return   the object removed, or `nullptr` if it does not exist.
 *
 *@param cellPos    the cell position
 *@param col    the cell column to add the object to
 */
std::shared_ptr<Object> GridManager::moveObject(Vec2 cellPos) {
    // Find object in object map
    auto posPair = std::make_pair(cellPos.x, cellPos.y);
    std::shared_ptr<Object> obj;
    bool movingArtObject = false;
    auto it = posToObjMap.find(posPair);
    if (it == posToObjMap.end()) {
        // If unable to find object
        auto it = posToArtObjMap.find(posPair);
        if (it == posToArtObjMap.end()) {
            return nullptr;
        }
        else {
            obj = it->second[0];
        }
    }
    else {
        obj = it->second;
    }
//    Size size = itemToGridSize(obj->getItemType());
//
//    // Clear all positions the object occupies
//    auto originPosX = objToPosMap[obj].first;
//    auto originPosY = objToPosMap[obj].second;
//
//    for (int i = 0; i < size.getIWidth(); i++) {
//        for (int j = 0; j < size.getIHeight(); j++) {
//            // TODO: Check if the y-axis offset is positive or negative
//            auto posPair = std::make_pair(originPosX + i, originPosY + j);
//            if (movingArtObject) {
//                auto artObjs = posToArtObjMap[posPair];
//                artObjs.erase(std::remove(artObjs.begin(), artObjs.end(), obj), artObjs.end());
//                if (artObjs.size() == 0) {
//                    posToArtObjMap.erase(posPair);
//                }
//
//            }
//            else {
//                posToWorldObjMap.erase(posPair);
//                posToObjMap.erase(posPair);
//                hasObjMap.erase(posPair);
//            }
//            
//        }
//    }
//
//    // Clear the origin position of the object
//    objToPosMap.erase(obj);
////    worldObjToPosMap.erase(obj);

    return obj;
}

/**
 * Removes the world object from the object map, if it exists.
 *
 *@return   the world object removed
 *
 *@param obj    the world object
 */
std::shared_ptr<Object> GridManager::moveWorldObject(std::shared_ptr<Object> obj) {
    Size size = obj->getSize();

    // Clear all positions the object occupies
    auto originPosX = worldObjToPosMap[obj].first;
    auto originPosY = worldObjToPosMap[obj].second;

    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(originPosX + i, originPosY + j);

            posToWorldObjMap.erase(posPair);
            hasObjMap.erase(posPair);
        }
    }

    // DO NOT remove the object from the `worldObjToPos` map

    return obj;
}

/**
 * Checks whether we can place the object in the cell position.
 *
 * @return false if there exists an object
 *
 * @param cellPos    the cell position
 * @param size          the amount of area this object takes up (including its movement)
 * @param item      the item type
 */
bool GridManager::canPlace(Vec2 cellPos, Size size, Item item) {
    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // Find object in object map
            auto posPair = std::make_pair(cellPos.x + i, cellPos.y + j);

            if (posToWorldObjMap[posPair] != nullptr && !itemIsArtObject(item)) {
//                CULog("posToWorldObjMap[posPair] object type: %s", posToWorldObjMap[posPair]->getName().c_str());
                return false;   // Object exists in position
                // TODO : if it's an art object, instead check if artobjmap has that itemtype in it at that spot
            }
            
            if (posToArtObjMap.find(posPair) != posToArtObjMap.end()) { // there is an art object there
                // loop through objs and make sure none of them have same itemType
                auto objs = posToArtObjMap[posPair];
                for (auto it = objs.begin(); it != objs.end(); ++it) {
                    if ((*it)->getItemType() == item) {
                        return false;
                    }
                    if ((*it)->getItemType() == Item::ART_OBJECT) {
                        auto e = 4;
                    }
                    if (item == Item::ART_OBJECT) {
                        auto e = 5;
                    }
                }
            }
        }
    }

    return true;
};

/**
 * Checks whether we can place an existing object in the cell position.
 *
 * @return false if there exists an object
 *
 * @param cellPos    the cell position
 */
bool GridManager::canPlaceExisting(Vec2 cellPos, std::shared_ptr<Object> obj) {
    Size size = obj->getSize();

    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // Find object in object map
            auto posPair = std::make_pair(cellPos.x + i, cellPos.y + j);

            // Check if object exists in the position AND it's not the same object being moved
            if (posToWorldObjMap[posPair] != nullptr && posToWorldObjMap[posPair] != obj) {
//                CULog("posToWorldObjMap[posPair] object type: %s", posToWorldObjMap[posPair]->getName().c_str());
                return false;
            }
        }
    }

    return true;
}

void GridManager::clear() {
    posToArtObjMap.clear();
    posToObjMap.clear();
    posToWorldObjMap.clear();
    objToPosMap.clear();
    worldObjToPosMap.clear();
}

void GridManager::clearRound() {
    posToObjMap.clear();
    objToPosMap.clear();
}

/**
 * Deletes the object at this cell position from the world.
 *
 * @param obj    the object
 */
void GridManager::deleteObject(std::shared_ptr<Object> obj) {
    Size size = obj->getSize();
    
    // Clear all positions the object occupies
    auto originPosX = objToPosMap[obj].first;
    auto originPosY = objToPosMap[obj].second;

    for (int i = 0; i < size.getIWidth(); i++) {
        for (int j = 0; j < size.getIHeight(); j++) {
            // TODO: Check if the y-axis offset is positive or negative
            auto posPair = std::make_pair(originPosX + i, originPosY + j);

            posToWorldObjMap.erase(posPair);
            posToObjMap.erase(posPair);
            hasObjMap.erase(posPair);
        }
    }

    // Clears maps and returns the object
    objToPosMap.erase(obj);
    worldObjToPosMap.erase(obj);

    if (obj) {
        obj->dispose();
        obj = nullptr;
    }
}
