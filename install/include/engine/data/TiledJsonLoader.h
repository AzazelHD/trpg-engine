#pragma once

// [x]: TiledJsonLoader declares the loader API that reads a Tiled JSON file,
// validates its structure, and converts it into engine TileMapData.
// Loader does not throw; all failures are reported via outError.
// Returned TileMapData is a value copy; no caching or shared ownership.
#include "engine/data/TileMapData.h"
#include <filesystem>
#include <optional>
#include <string>
#include <nlohmann/json_fwd.hpp>

class TiledJsonLoader
{
public:
    // Convenience wrapper. Returns std::nullopt on failure.
    // outError is populated on failure, unchanged on success.
    [[nodiscard]] std::optional<TileMapData> loadFromFile(
        const std::filesystem::path &filePath,
        std::string &outError) const noexcept;

    // Primary load path. Returns true on success.
    // outError is populated on failure, unchanged on success.
    [[nodiscard]] bool tryLoadFromFile(
        const std::filesystem::path &filePath,
        TileMapData &outTileMap,
        std::string &outError) const noexcept;

private:
    using json = nlohmann::json;

    [[nodiscard]] bool parseLayer(
        const json &j,
        TileLayerData &outLayer,
        std::string &outError) const;

    [[nodiscard]] bool parseTileset(
        const json &j,
        TileSetData &outTileset,
        TileMapData &tmp,
        std::string &outError) const;

    [[nodiscard]] bool internTileType(
        std::string_view name,
        TileMapData &tmp,
        TileTypeId &outId,
        std::string &outError) const;
};