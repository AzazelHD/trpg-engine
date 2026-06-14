// engine/include/engine/data/TileMapData.h
#pragma once
//
// TileMapData — engine-owned runtime structures loaded from Tiled.
// Rendering and gameplay systems consume this instead of raw JSON.
//
// Tile layers   → TileLayerData with tiles vector (GIDs)
// Object layers → TileLayerData with objects vector (MapObject)
// Both live in the same layers vector; check LayerType to know which to use.

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

// One object placed in a Tiled object layer.
// Only point objects are used for now (spawns, triggers).
// x/y are raw Tiled pixel coordinates — convert to grid coords via
// tileWidth/tileHeight when needed (see BattleMap::buildFrom).
struct MapObject
{
    int id = 0;            // Tiled object id (unique within the map)
    std::string name;      // optional label set in Tiled
    std::string className; // "spawn_player", "spawn_enemy" etc. — matches TileClass strings
    float x = 0.f;         // pixel position (raw from Tiled)
    float y = 0.f;
    bool isPoint = false; // true for point objects; false for rectangles etc.
};

struct TileLayerData
{
    std::string name;
    std::string className;
    LayerType type = LayerType::Tile;
    std::int32_t width = 0;
    std::int32_t height = 0;
    bool visible = true;
    float opacity = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    // Populated for LayerType::Tile — GIDs in row-major order.
    std::vector<std::uint32_t> tiles;

    // Populated for LayerType::Object — all objects in this layer.
    std::vector<MapObject> objects;

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

    // Find a layer by name — linear scan, returns nullptr if not found.
    [[nodiscard]] const TileLayerData *findLayer(std::string_view layerName) const noexcept;

    [[nodiscard]] TileTypeId tileTypeId(std::string_view tileTypeName) const noexcept;
    [[nodiscard]] std::string_view tileTypeName(TileTypeId tileTypeId) const noexcept;
    [[nodiscard]] TileTypeId tileTypeForGlobalTileId(std::uint32_t globalTileId) const noexcept;
};

// Helper: O(n) linear scan, returns nullptr if not found.
[[nodiscard]] const TileProperty *
findProperty(const std::vector<TileProperty> &props, std::string_view name) noexcept;