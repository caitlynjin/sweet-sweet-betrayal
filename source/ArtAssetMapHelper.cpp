#include "ArtAssetMapHelper.h"
#include "Constants.h"
#include "any"

using namespace cugl;
using namespace Constants;

//ArtAssetMapHelper::ArtAssetMapHelper() {};
void ArtAssetMapHelper::populateConstantsMaps()
{
    /* For each row,
        * Element 0 = jsonType (string)
        * Element 1 = texture (string)
        * Element 2 = itemType (part of Item enum, must be updated there in Constants.h as well)
        *
        *
        * Elements 3 and 4 don't do anything. I was just too lazy to remove them.
        */
    std::vector <std::vector<std::any>> vec = {
    {std::string("default"), std::string(EARTH_TEXTURE), Item::ART_OBJECT, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("tileTop"), std::string(TOP_TILE_TEXTURE), Item::TILE_TOP, cugl::Size(1, 1), cugl::Size(1, 1) },
    {std::string("tileBottom"), std::string(BOTTOM_TILE_TEXTURE), Item::TILE_BOTTOM, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("tileInner"), std::string(INNER_TILE_TEXTURE), Item::TILE_INNER, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("tileLeft"), std::string(LEFT_TILE_TEXTURE), Item::TILE_LEFT, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("tileRight"), std::string(RIGHT_TILE_TEXTURE), Item::TILE_RIGHT, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("tileTopLeft"), std::string(TOPLEFT_TILE_TEXTURE), Item::TILE_TOPLEFT, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("tileTopRight"), std::string(TOPRIGHT_TILE_TEXTURE), Item::TILE_TOPRIGHT, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("crack1"), std::string(CRACK1_TEXTURE), Item::CRACK_1, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("crack2"), std::string(CRACK2_TEXTURE), Item::CRACK_2, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("crack3"), std::string(CRACK3_TEXTURE), Item::CRACK_3, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("crack4"), std::string(CRACK4_TEXTURE), Item::CRACK_4, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("crack5"), std::string(CRACK5_TEXTURE), Item::CRACK_5, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("crackLarge1"), std::string(CRACKLARGE1_TEXTURE), Item::CRACK_LARGE_1, cugl::Size(2, 2), cugl::Size(2, 2)},
    {std::string("moss1"), std::string(MOSS1_TEXTURE), Item::MOSS_1, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("moss2"), std::string(MOSS2_TEXTURE), Item::MOSS_2, cugl::Size(1, 2), cugl::Size(1, 2)},
    {std::string("rocky1"), std::string(ROCKY1_TEXTURE), Item::ROCKY_1, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("rocky2"), std::string(ROCKY2_TEXTURE), Item::ROCKY_2, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("spikeUp"), std::string(SPIKE_UP_TEXTURE), Item::SPIKE_UP, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("spikeDown"), std::string(SPIKE_DOWN_TEXTURE), Item::SPIKE_DOWN, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("spikeLeft"), std::string(SPIKE_LEFT_TEXTURE), Item::SPIKE_LEFT, cugl::Size(1, 1), cugl::Size(1, 1)},
    {std::string("spikeRight"), std::string(SPIKE_RIGHT_TEXTURE), Item::SPIKE_RIGHT, cugl::Size(1, 1), cugl::Size(1, 1)},
    // Note that from here on out, these items likely won't ever be saved in a JSON file.
        // I am putting them here to consolidate all of this logic into one place.
        {std::string("thorn"), std::string(THORN_TEXTURE), Item::THORN},
        {std::string("wind"), std::string(FAN_TEXTURE), Item::WIND},
        {std::string("platform"), std::string(LOG_TEXTURE), Item::PLATFORM},
        {std::string("movingPlatform"), std::string(GLIDING_LOG_TEXTURE), Item::MOVING_PLATFORM},
        {std::string("spike"), std::string(SPIKE_TILE_TEXTURE), Item::SPIKE},
        {std::string("treasure"), std::string(TREASURE_TEXTURE), Item::TREASURE},
        {std::string("artObject"), std::string(EARTH_TEXTURE), Item::ART_OBJECT},
        {std::string("tileItem"), std::string(TILE_TEXTURE), Item::TILE_ITEM},
        {std::string("mushroom"), std::string(MUSHROOM_TEXTURE), Item::MUSHROOM},
        {std::string("bomb"), std::string(BOMB_TEXTURE), Item::BOMB},
        {std::string("none"), std::string(EARTH_TEXTURE), Item::NONE},
    { std::string("torchRight"), std::string("torch-right"), Item::TORCH },
    { std::string("tileInsideFilled"), std::string("tile-inside-filled"), Item::TILE_INSIDEFILLED},
    { std::string("tileInsideLeft"), std::string("tile-inside-left"), Item::TILE_INSIDELEFT},
    { std::string("tileInsideRight"), std::string("tile-inside-right"), Item::TILE_INSIDERIGHT},
    };
    std::vector<std::any> tempVec;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        tempVec = *it;
        if (std::any_cast<std::string>(tempVec[0]) == std::string("tileLeft")) {
            CULog("correct");
            if (std::any_cast<std::string>(tempVec[1]) == std::string(LEFT_TILE_TEXTURE)) {
                CULog("also correct");
            }
        }
        Constants::jsonTypeToAsset[std::any_cast<std::string>(tempVec[0])] = std::any_cast<std::string>(tempVec[1]);
        Constants::jsonTypeToItemType[std::any_cast<std::string>(tempVec[0])] = std::any_cast<Item>(tempVec[2]);
        Constants::itemToAssetNameMap[std::any_cast<Item>(tempVec[2])] = std::any_cast<std::string>(tempVec[1]);

    }
}