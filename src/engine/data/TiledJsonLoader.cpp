#include "engine/data/TiledJsonLoader.h"
#include <exception>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ─────────────────────────────────────────────────────────────────────────────
// Public
// ─────────────────────────────────────────────────────────────────────────────

std::optional<TileMapData> TiledJsonLoader::loadFromFile(
    const std::filesystem::path &filePath,
    std::string &outError) const noexcept
{
    TileMapData tmp;
    if (!tryLoadFromFile(filePath, tmp, outError))
        return std::nullopt;
    return tmp;
}

bool TiledJsonLoader::tryLoadFromFile(
    const std::filesystem::path &filePath,
    TileMapData &outTileMap,
    std::string &outError) const noexcept
{
    try
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            outError = "Failed to open file: " + filePath.string();
            return false;
        }

        json j;
        file >> j;

        if (!j.contains("width") || !j.contains("height"))
        {
            outError = "Missing required map fields (width/height)";
            return false;
        }

        TileMapData tmp;
        tmp.tileTypeNames.emplace_back(); // index 0 reserved for kNoTileType

        tmp.width = j["width"];
        tmp.height = j["height"];
        tmp.tileWidth = j.value("tilewidth", 0);
        tmp.tileHeight = j.value("tileheight", 0);

        // Keep map directory so we can resolve external tileset paths.
        const std::filesystem::path mapDir = filePath.parent_path();

        for (const auto &layerJson : j.value("layers", json::array()))
        {
            TileLayerData layer;
            if (!parseLayer(layerJson, layer, outError))
                return false;
            tmp.layers.push_back(std::move(layer));
        }

        for (const auto &tilesetJson : j.value("tilesets", json::array()))
        {
            TileSetData tileset;
            if (!parseTileset(tilesetJson, tileset, tmp, mapDir, outError))
                return false;
            tmp.tilesets.push_back(std::move(tileset));
        }

        outTileMap = std::move(tmp);
        return true;
    }
    catch (const std::exception &ex)
    {
        outError = std::string("Parse error: ") + ex.what();
        return false;
    }
    catch (...)
    {
        outError = "Unknown parse error";
        return false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Private — layer parsing
// ─────────────────────────────────────────────────────────────────────────────

bool TiledJsonLoader::parseLayer(
    const json &j,
    TileLayerData &outLayer,
    std::string &outError) const
{
    if (!j.is_object())
    {
        outError = "Layer entry must be an object";
        return false;
    }

    outLayer.name = j.value("name", "");
    if (j.contains("class") && j["class"].is_string())
        outLayer.className = j["class"].get<std::string>();
    else
        outLayer.className.clear();
    outLayer.visible = j.value("visible", true);
    outLayer.opacity = j.value("opacity", 1.0f);
    outLayer.offsetX = j.value("offsetx", 0.0f);
    outLayer.offsetY = j.value("offsety", 0.0f);

    const std::string layerType = j.value("type", "tilelayer");

    // ── Object layer ──────────────────────────────────────────────────────
    if (layerType == "objectgroup")
    {
        outLayer.type = LayerType::Object;

        // width/height are not meaningful for object layers.
        outLayer.width = 0;
        outLayer.height = 0;

        for (const auto &objJson : j.value("objects", json::array()))
        {
            MapObject obj;
            if (!parseObject(objJson, obj, outError))
                return false;
            outLayer.objects.push_back(std::move(obj));
        }

        return true;
    }

    // ── Tile layer (default) ──────────────────────────────────────────────
    outLayer.type = LayerType::Tile;
    outLayer.width = j.value("width", 0);
    outLayer.height = j.value("height", 0);

    if (!j.contains("data"))
        return true;

    const auto &data = j["data"];
    if (!data.is_array())
    {
        outError = "Layer tile data must be an array";
        return false;
    }

    outLayer.tiles.reserve(data.size());
    for (const auto &v : data)
        outLayer.tiles.push_back(v.get<std::uint32_t>());

    return true;
}

bool TiledJsonLoader::parseObject(
    const json &j,
    MapObject &outObject,
    std::string &outError) const
{
    if (!j.is_object())
    {
        outError = "Object entry must be a JSON object";
        return false;
    }

    outObject.id = j.value("id", 0);
    outObject.name = j.value("name", "");
    outObject.x = j.value("x", 0.f);
    outObject.y = j.value("y", 0.f);
    outObject.isPoint = j.value("point", false);

    // Tiled stores the object class in the "class" field (Tiled 1.9+).
    // Older versions used "type" — fall back to that for compatibility.
    if (j.contains("class") && j["class"].is_string())
        outObject.className = j["class"].get<std::string>();
    else
        outObject.className = j.value("type", "");

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Private — tileset parsing
// ─────────────────────────────────────────────────────────────────────────────

bool TiledJsonLoader::parseTileset(
    const json &j,
    TileSetData &outTileset,
    TileMapData &tmp,
    const std::filesystem::path &mapDir,
    std::string &outError) const
{
    if (!j.is_object())
    {
        outError = "Tileset entry must be an object";
        return false;
    }

    // firstgid is always present in the map's tileset reference.
    outTileset.firstGlobalTileId = j.value("firstgid", 0);

    // ── External tileset (.tsx referenced by "source") ────────────────────
    // When a tileset is external, the map JSON only has firstgid + source.
    // Load and parse the .tsx file to get the actual tile data.
    if (j.contains("source") && j["source"].is_string())
    {
        const std::filesystem::path tsxPath =
            mapDir / j["source"].get<std::string>();

        return loadExternalTileset(tsxPath, outTileset, tmp, outError);
    }

    // ── Embedded tileset ──────────────────────────────────────────────────
    outTileset.name = j.value("name", "");
    outTileset.imagePath = j.value("image", "");
    outTileset.tileWidth = j.value("tilewidth", 0);
    outTileset.tileHeight = j.value("tileheight", 0);
    outTileset.tileCount = j.value("tilecount", 0);
    outTileset.columns = j.value("columns", 0);
    outTileset.spacing = j.value("spacing", 0);
    outTileset.margin = j.value("margin", 0);

    if (outTileset.tileCount > 0)
        outTileset.tileTypes.resize(
            static_cast<std::size_t>(outTileset.tileCount), kNoTileType);

    if (!j.contains("tiles"))
        return true;

    const auto &tiles = j["tiles"];
    if (!tiles.is_array())
    {
        outError = "Tileset tiles must be an array";
        return false;
    }

    for (const auto &tile : tiles)
    {
        if (!tile.is_object())
        {
            outError = "Tileset tile entry must be an object";
            return false;
        }

        const int localId = tile.value("id", -1);
        if (localId < 0)
        {
            outError = "Tileset tile entry missing valid id";
            return false;
        }

        const auto idx = static_cast<std::size_t>(localId);
        if (idx >= outTileset.tileTypes.size())
            outTileset.tileTypes.resize(idx + 1, kNoTileType);

        // FIX: Tiled saves tile class as top-level "type" field on the tile,
        // NOT as a custom property. Read it directly here.
        if (!tile.contains("type"))
            continue;

        const auto &typeVal = tile["type"];
        if (!typeVal.is_string())
            continue;

        TileTypeId id = kNoTileType;
        if (!internTileType(typeVal.get_ref<const std::string &>(), tmp, id, outError))
            return false;

        outTileset.tileTypes[idx] = id;
    }

    return true;
}

// ── XML helpers ───────────────────────────────────────────────────────────────
// Minimal XML attribute extractor — no dependencies beyond <string>.
// Reads the value of a named attribute from a single XML tag string.
// e.g. extractAttr(`<tile id="3" type="grass"/>`, "type") → "grass"
// Returns empty string if the attribute is not found.
namespace
{
    std::string extractAttr(const std::string &tag, const std::string &attr)
    {
        // Look for attr=" or attr='
        for (char quote : {'"', '\''})
        {
            const std::string needle = attr + "=" + quote;
            const auto pos = tag.find(needle);
            if (pos == std::string::npos)
                continue;

            const auto start = pos + needle.size();
            const auto end = tag.find(quote, start);
            if (end == std::string::npos)
                continue;

            return tag.substr(start, end - start);
        }
        return {};
    }
} // namespace

bool TiledJsonLoader::loadExternalTileset(
    const std::filesystem::path &tsxPath,
    TileSetData &outTileset,
    TileMapData &tmp,
    std::string &outError) const
{
    // Tiled .tsx files are XML. We parse them with a minimal line-by-line
    // attribute extractor — no XML library needed for Tiled's simple format.
    std::ifstream file(tsxPath);
    if (!file.is_open())
    {
        outError = "Failed to open external tileset: " + tsxPath.string();
        return false;
    }

    std::string line;
    bool inTileTag = false;
    int currentId = -1;

    while (std::getline(file, line))
    {
        // Trim leading whitespace.
        const auto first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
            continue;
        const std::string tag = line.substr(first);

        // ── <tileset …> ───────────────────────────────────────────────────
        if (tag.find("<tileset") == 0)
        {
            outTileset.name = extractAttr(tag, "name");
            outTileset.tileWidth = std::stoi(extractAttr(tag, "tilewidth").empty() ? "0" : extractAttr(tag, "tilewidth"));
            outTileset.tileHeight = std::stoi(extractAttr(tag, "tileheight").empty() ? "0" : extractAttr(tag, "tileheight"));
            outTileset.tileCount = std::stoi(extractAttr(tag, "tilecount").empty() ? "0" : extractAttr(tag, "tilecount"));
            outTileset.columns = std::stoi(extractAttr(tag, "columns").empty() ? "0" : extractAttr(tag, "columns"));
            outTileset.spacing = std::stoi(extractAttr(tag, "spacing").empty() ? "0" : extractAttr(tag, "spacing"));
            outTileset.margin = std::stoi(extractAttr(tag, "margin").empty() ? "0" : extractAttr(tag, "margin"));

            if (outTileset.tileCount > 0)
                outTileset.tileTypes.resize(
                    static_cast<std::size_t>(outTileset.tileCount), kNoTileType);

            continue;
        }

        // ── <image …> ─────────────────────────────────────────────────────
        if (tag.find("<image") == 0)
        {
            outTileset.imagePath = extractAttr(tag, "source");
            continue;
        }

        // ── <tile …> ──────────────────────────────────────────────────────
        if (tag.find("<tile") == 0 && tag.find("</tile") == std::string::npos)
        {
            const std::string idStr = extractAttr(tag, "id");
            if (idStr.empty())
                continue;

            currentId = std::stoi(idStr);
            inTileTag = true;

            // Tile class is stored as "type" attribute in Tiled XML.
            const std::string typeName = extractAttr(tag, "type");
            if (!typeName.empty())
            {
                const auto idx = static_cast<std::size_t>(currentId);
                if (idx >= outTileset.tileTypes.size())
                    outTileset.tileTypes.resize(idx + 1, kNoTileType);

                TileTypeId id = kNoTileType;
                if (!internTileType(typeName, tmp, id, outError))
                    return false;

                outTileset.tileTypes[idx] = id;
            }

            // Self-closing tag — no children to parse.
            if (tag.find("/>") != std::string::npos)
            {
                inTileTag = false;
                currentId = -1;
            }

            continue;
        }

        // ── </tile> ───────────────────────────────────────────────────────
        if (tag.find("</tile") == 0)
        {
            inTileTag = false;
            currentId = -1;
            continue;
        }
    }

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Private — type interning
// ─────────────────────────────────────────────────────────────────────────────

bool TiledJsonLoader::internTileType(
    std::string_view name,
    TileMapData &tmp,
    TileTypeId &outId,
    std::string &outError) const
{
    if (name.empty())
    {
        outId = kNoTileType;
        return true;
    }

    const TileTypeId existing = tmp.tileTypeId(name);
    if (existing != kNoTileType)
    {
        outId = existing;
        return true;
    }

    constexpr auto maxId =
        static_cast<std::size_t>(std::numeric_limits<TileTypeId>::max());

    if (tmp.tileTypeNames.size() > maxId)
    {
        outError = "Too many distinct tile types";
        return false;
    }

    tmp.tileTypeNames.emplace_back(name);
    outId = static_cast<TileTypeId>(tmp.tileTypeNames.size() - 1);
    return true;
}