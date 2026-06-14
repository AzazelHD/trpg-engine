#include "engine/data/TileMapData.h"

namespace
{
    constexpr std::uint32_t kTiledFlipFlagsMask = 0xE0000000u;
}

// [x]: Reset map state for reuse before loading a new Tiled map.
void TileMapData::clear() noexcept
{
    width = 0;
    height = 0;
    tileWidth = 0;
    tileHeight = 0;

    tilesets.clear();
    layers.clear();
    properties.clear();
    tileTypeNames.clear();
}

// [x]: True if map has no usable data.
bool TileMapData::isEmpty() const noexcept
{
    return layers.empty() || width == 0 || height == 0;
}

// [x]: Find layer by name (linear search, no ownership transfer).
const TileLayerData *TileMapData::findLayer(std::string_view layerName) const noexcept
{
    for (const auto &layer : layers)
    {
        if (layer.name == layerName)
            return &layer;
    }
    return nullptr;
}

TileTypeId TileMapData::tileTypeId(std::string_view tileTypeName) const noexcept
{
    if (tileTypeName.empty())
        return kNoTileType;

    for (std::size_t index = 1; index < tileTypeNames.size(); ++index)
    {
        if (tileTypeNames[index] == tileTypeName)
            return static_cast<TileTypeId>(index);
    }

    return kNoTileType;
}

std::string_view TileMapData::tileTypeName(TileTypeId tileTypeId) const noexcept
{
    const auto index = static_cast<std::size_t>(tileTypeId);
    if (index == 0 || index >= tileTypeNames.size())
        return {};

    return tileTypeNames[index];
}

TileTypeId TileMapData::tileTypeForGlobalTileId(std::uint32_t globalTileId) const noexcept
{
    const std::uint32_t normalizedGlobalTileId = globalTileId & ~kTiledFlipFlagsMask;
    if (normalizedGlobalTileId == 0)
        return kNoTileType;

    for (auto it = tilesets.rbegin(); it != tilesets.rend(); ++it)
    {
        const auto &tileset = *it;
        if (tileset.firstGlobalTileId <= 0)
            continue;

        const auto firstGlobalTileId = static_cast<std::uint32_t>(tileset.firstGlobalTileId);
        if (normalizedGlobalTileId < firstGlobalTileId)
            continue;

        const auto localTileId = static_cast<std::size_t>(normalizedGlobalTileId - firstGlobalTileId);
        if (localTileId < tileset.tileTypes.size())
            return tileset.tileTypes[localTileId];

        return kNoTileType;
    }

    return kNoTileType;
}

// Helper: Find property by name (deprecated conceptually, ID-based system preferred)
const TileProperty *
findProperty(const std::vector<TileProperty> &props, std::string_view name) noexcept
{
    (void)props;
    (void)name;

    // Runtime properties are currently ID-based, so string lookup is unsupported.
    return nullptr;
}