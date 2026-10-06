// engine/include/engine/data/TiledJsonLoader.h
#pragma once
//
// TiledJsonLoader — reads a Tiled JSON map file (.tmj / .json) and converts
// it into engine TileMapData.
//
// Does not throw; all failures are reported via outError.
// Returned TileMapData is a value copy — no caching or shared ownership.
//
// Supported layer types:
//   tilelayer   → TileLayerData with tiles (GID array)
//   objectgroup → TileLayerData with objects (MapObject list)
//
// External tilesets (.tsx) are loaded relative to the map file path.

#include "engine/data/TileMapData.h"
#include <filesystem>
#include <optional>
#include <string>
#include <nlohmann/json_fwd.hpp>

class TiledJsonLoader
{
public:
    // Convenience wrapper — returns std::nullopt on failure.
    // outError is populated on failure, unchanged on success.
    [[nodiscard]] std::optional<TileMapData> loadFromFile(
        const std::filesystem::path &filePath,
        std::string &outError) const noexcept;

    // Primary load path — returns true on success.
    // outError is populated on failure, unchanged on success.
    [[nodiscard]] bool tryLoadFromFile(
        const std::filesystem::path &filePath,
        TileMapData &outTileMap,
        std::string &outError) const noexcept;

private:
    using json = nlohmann::json;

    // Parse one layer entry (tilelayer or objectgroup).
    [[nodiscard]] bool parseLayer(
        const json &j,
        TileLayerData &outLayer,
        std::string &outError) const;

    // Parse one object entry inside an objectgroup layer.
    [[nodiscard]] bool parseObject(
        const json &j,
        MapObject &outObject,
        std::string &outError) const;

    // Parse tileset data — handles both embedded and external (.tsx) tilesets.
    // mapDir is used to resolve external tileset paths.
    [[nodiscard]] bool parseTileset(
        const json &j,
        TileSetData &outTileset,
        TileMapData &tmp,
        const std::filesystem::path &mapDir,
        std::string &outError) const;

    // Load and parse an external .tsx tileset file.
    [[nodiscard]] bool loadExternalTileset(
        const std::filesystem::path &tsxPath,
        TileSetData &outTileset,
        TileMapData &tmp,
        std::string &outError) const;

    // Intern a tile type name string into TileMapData's type registry.
    // Returns kNoTileType if name is empty.
    [[nodiscard]] bool internTileType(
        std::string_view name,
        TileMapData &tmp,
        TileTypeId &outId,
        std::string &outError) const;
};