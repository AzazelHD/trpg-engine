#pragma once

// [x]: TileMapData defines engine-owned runtime structures loaded from Tiled.
// Rendering and gameplay systems consume this instead of raw JSON.

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include "engine/data/PropertyId.h"

struct TileProperty
{
    PropertyId id;
    std::variant<bool, int, float, std::string> value; // NOTE: std::visit required at each use
};

using TileTypeId = std::uint16_t;
inline constexpr TileTypeId kNoTileType = 0;

struct TileSetData
{
    std::string name;
    std::string imagePath;
    std::int32_t firstGlobalTileId = 0;
    std::int32_t tileWidth = 0;
    std::int32_t tileHeight = 0;
    std::int32_t tileCount = 0;
    std::int32_t columns = 0;
    std::int32_t spacing = 0;
    std::int32_t margin = 0;

    // Indexed by local tile id within this tileset; missing entries default to kNoTileType.
    std::vector<TileTypeId> tileTypes;
};

enum class LayerType
{
    Tile,
    Object,
    Image
};

struct TileLayerData
{
    std::string name;
    LayerType type = LayerType::Tile;

    std::int32_t width = 0;
    std::int32_t height = 0;

    bool visible = true;
    float opacity = 1.0f;

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    std::vector<std::uint32_t> tiles;
    std::vector<TileProperty> properties;
};

struct TileMapData
{
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t tileWidth = 0;
    std::int32_t tileHeight = 0;

    std::vector<TileSetData> tilesets;
    std::vector<TileLayerData> layers;
    std::vector<TileProperty> properties;
    // Index 0 is reserved for kNoTileType; remaining entries are user-defined Tiled type strings.
    std::vector<std::string> tileTypeNames;

    void clear() noexcept;

    [[nodiscard]] bool isEmpty() const noexcept;
    [[nodiscard]] const TileLayerData *findLayer(std::string_view layerName) const noexcept;
    [[nodiscard]] TileTypeId tileTypeId(std::string_view tileTypeName) const noexcept;
    [[nodiscard]] std::string_view tileTypeName(TileTypeId tileTypeId) const noexcept;
    [[nodiscard]] TileTypeId tileTypeForGlobalTileId(std::uint32_t globalTileId) const noexcept;
};

// Helper: O(n) linear scan, returns nullptr if not found
[[nodiscard]] const TileProperty *
findProperty(const std::vector<TileProperty> &props, std::string_view name) noexcept;