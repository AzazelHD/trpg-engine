#include "engine/data/TiledJsonLoader.h"
#include "engine/data/PropertyRegistry.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace
{
    int g_failures = 0;
    int g_checks = 0;

    void reportFailure(const char *expr, const char *file, int line)
    {
        ++g_checks;
        ++g_failures;
        std::cout << "FAIL " << file << ":" << line << " - " << expr << "\n";
    }

    template <typename A, typename B>
    void checkEqImpl(const A &a, const B &b, const char *ea, const char *eb, const char *file, int line)
    {
        if (!(a == b))
            reportFailure((std::string(ea) + " == " + eb).c_str(), file, line);
        else
            ++g_checks;
    }

    std::filesystem::path fixtureDir()
    {
        return std::filesystem::path(__FILE__).parent_path() / "data";
    }
}

#define CHECK(expr) reportFailureImplChecked((expr) ? 0 : 1, #expr, __FILE__, __LINE__)
#define CHECK_EQ(a, b) checkEqImpl((a), (b), #a, #b, __FILE__, __LINE__)

namespace
{
    void reportFailureImplChecked(bool failed, const char *expr, const char *file, int line)
    {
        if (failed)
            reportFailure(expr, file, line);
        else
            ++g_checks;
    }
}

static void testLoaderMinimal()
{
    TileMapData map;
    std::string err;
    CHECK(TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "minimal.tmj", map, err));

    CHECK_EQ(map.width, 3);
    CHECK_EQ(map.height, 3);
    CHECK_EQ(map.tileWidth, 16);
    CHECK_EQ(map.tileHeight, 16);
    CHECK_EQ(map.layers.size(), std::size_t{3});
    CHECK_EQ(map.tilesets.size(), std::size_t{1});

    CHECK_EQ(map.tilesets[0].firstGlobalTileId, 1);
    CHECK_EQ(map.tilesets[0].name, std::string("tiles"));
    CHECK_EQ(map.tilesets[0].imagePath, std::string("tiles.png"));
    CHECK_EQ(map.tilesets[0].tileWidth, 16);
    CHECK_EQ(map.tilesets[0].tileHeight, 16);
    CHECK_EQ(map.tilesets[0].tileCount, 4);
    CHECK_EQ(map.tilesets[0].columns, 2);
    CHECK_EQ(map.tilesets[0].tileTypes.size(), std::size_t{4});

    const TileLayerData *ground = map.findLayer("ground");
    CHECK(ground != nullptr);
    if (ground)
    {
        CHECK(ground->type == LayerType::Tile);
        CHECK_EQ(ground->width, 3);
        CHECK_EQ(ground->height, 3);
        CHECK(ground->visible);
        CHECK(ground->opacity == 1.0f);
        CHECK_EQ(ground->tiles.size(), std::size_t{9});
        CHECK_EQ(ground->tiles[0], std::uint32_t{1});
        CHECK_EQ(ground->tiles[1], std::uint32_t{2});
        CHECK_EQ(ground->tiles[8], std::uint32_t{1});
    }

    const TileLayerData *empty = map.findLayer("empty");
    CHECK(empty != nullptr);
    if (empty)
    {
        CHECK(empty->type == LayerType::Tile);
        CHECK_EQ(empty->width, 2);
        CHECK_EQ(empty->height, 2);
        CHECK(empty->tiles.empty());
    }

    const TileLayerData *spawns = map.findLayer("spawns");
    CHECK(spawns != nullptr);
    if (spawns)
    {
        CHECK(spawns->type == LayerType::Object);
        CHECK_EQ(spawns->width, 0);
        CHECK_EQ(spawns->objects.size(), std::size_t{2});

        CHECK_EQ(spawns->objects[0].id, 10);
        CHECK_EQ(spawns->objects[0].name, std::string("playerSpawn"));
        CHECK_EQ(spawns->objects[0].className, std::string("spawn_player"));
        CHECK(spawns->objects[0].isPoint);
        CHECK(spawns->objects[0].x == 16.0f);
        CHECK(spawns->objects[0].y == 32.0f);

        CHECK_EQ(spawns->objects[1].className, std::string("spawn_enemy"));
        CHECK(!spawns->objects[1].isPoint);
        CHECK(spawns->objects[1].x == 24.0f);
        CHECK(spawns->objects[1].y == 8.0f);
    }
}

static void testLoaderExternalTileset()
{
    TileMapData map;
    std::string err;
    CHECK(TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "external_tsx.tmj", map, err));

    CHECK_EQ(map.tilesets.size(), std::size_t{1});
    const TileSetData &ts = map.tilesets[0];
    CHECK_EQ(ts.firstGlobalTileId, 1);
    CHECK_EQ(ts.name, std::string("Ext"));
    CHECK_EQ(ts.tileWidth, 16);
    CHECK_EQ(ts.tileHeight, 16);
    CHECK_EQ(ts.tileCount, 4);
    CHECK_EQ(ts.columns, 2);
    CHECK_EQ(ts.tileTypes.size(), std::size_t{4});

    const TileTypeId crater = map.tileTypeId("Crater");
    const TileTypeId forest = map.tileTypeId("Forest");
    CHECK(crater != kNoTileType);
    CHECK(forest != kNoTileType);
    CHECK_EQ(ts.tileTypes[0], crater);
    CHECK_EQ(ts.tileTypes[2], forest);
    CHECK_EQ(ts.tileTypes[1], kNoTileType);
    CHECK_EQ(ts.tileTypes[3], kNoTileType);

    CHECK_EQ(map.tileTypeForGlobalTileId(1), crater);
    CHECK_EQ(map.tileTypeForGlobalTileId(0xC0000001u), crater);
    CHECK_EQ(map.tileTypeForGlobalTileId(3), forest);
    CHECK_EQ(map.tileTypeForGlobalTileId(0x20000003u), forest);
    CHECK_EQ(map.tileTypeForGlobalTileId(5), kNoTileType);
    CHECK_EQ(map.tileTypeForGlobalTileId(0), kNoTileType);
}

static void testLoaderTileTypes()
{
    TileMapData map;
    std::string err;
    CHECK(TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "tile_types.tmj", map, err));

    CHECK_EQ(map.tilesets.size(), std::size_t{1});
    const TileSetData &ts = map.tilesets[0];
    CHECK_EQ(ts.tileTypes.size(), std::size_t{6}); // tilecount 2, tile id 5 grows the vector

    const TileTypeId grass = map.tileTypeId("GrassAtk");
    const TileTypeId crater = map.tileTypeId("Crater");
    CHECK(grass != kNoTileType);
    CHECK(crater != kNoTileType);
    CHECK_EQ(ts.tileTypes[0], grass); // shared type name interns to the same id
    CHECK_EQ(ts.tileTypes[1], grass);
    CHECK_EQ(ts.tileTypes[5], crater);
    CHECK_EQ(ts.tileTypes[2], kNoTileType);

    CHECK_EQ(map.tileTypeName(grass), std::string_view("GrassAtk"));
    CHECK_EQ(map.tileTypeName(crater), std::string_view("Crater"));
    CHECK_EQ(map.tileTypeId("Nope"), kNoTileType);
}

static void testLoaderErrors()
{
    TileMapData map;
    std::string err;

    CHECK(!TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "missing_fields.tmj", map, err));
    CHECK(err.find("width/height") != std::string::npos);

    err.clear();
    CHECK(!TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "garbage.json", map, err));
    CHECK(err.find("Parse error") != std::string::npos);
    CHECK(!TiledJsonLoader{}.loadFromFile(fixtureDir() / "garbage.json", err).has_value());

    err.clear();
    CHECK(!TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "does_not_exist.tmj", map, err));
    CHECK(err.find("Failed to open file") != std::string::npos);

    err.clear();
    CHECK(!TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "bad_layer.tmj", map, err));
    CHECK(err.find("Layer tile data must be an array") != std::string::npos);

    err.clear();
    CHECK(!TiledJsonLoader{}.tryLoadFromFile(fixtureDir() / "bad_layer_entry.tmj", map, err));
    CHECK(err.find("Layer entry must be an object") != std::string::npos);
}

static void testTileMapData()
{
    TileMapData map;
    CHECK(map.isEmpty());
    CHECK_EQ(map.findLayer("ground"), nullptr);
    CHECK_EQ(map.tileTypeId("GrassAtk"), kNoTileType);
    CHECK(map.tileTypeName(kNoTileType).empty());

    map.width = 4;
    map.height = 4;
    CHECK(map.isEmpty()); // no layers yet

    TileLayerData layer;
    layer.name = "ground";
    map.layers.push_back(std::move(layer));
    CHECK(!map.isEmpty());
    CHECK(map.findLayer("ground") != nullptr);
    CHECK_EQ(map.findLayer("nope"), nullptr);

    map.clear();
    CHECK(map.isEmpty());
    CHECK(map.layers.empty());
    CHECK(map.tileTypeNames.empty());

    std::vector<TileProperty> props;
    props.push_back(TileProperty{.id = PropertyId::Solid, .value = true});
    props.push_back(TileProperty{.id = PropertyId::Damage, .value = 5});
    props.push_back(TileProperty{.id = PropertyId::SpawnPoint, .value = 2.5f});
    props.push_back(TileProperty{.id = PropertyId::Unknown, .value = std::string("tag")});

    CHECK(findProperty(props, "Solid") == nullptr); // string lookup intentionally unsupported

    CHECK(std::holds_alternative<bool>(props[0].value));
    CHECK(std::get<bool>(props[0].value));
    CHECK_EQ(std::get<int>(props[1].value), 5);
    CHECK(std::holds_alternative<float>(props[2].value));
    CHECK(std::get<float>(props[2].value) == 2.5f);
    CHECK(std::holds_alternative<std::string>(props[3].value));
    CHECK_EQ(std::get<std::string>(props[3].value), std::string("tag"));
}

static void testPropertyRegistry()
{
    PropertyRegistry registry;
    CHECK(registry.get("Solid") == PropertyId::Unknown); // unregistered before use

    registry.registerProperty("Solid", PropertyId::Solid);
    registry.registerProperty("Damage", PropertyId::Damage);
    registry.registerProperty("SpawnPoint", PropertyId::SpawnPoint);

    CHECK(registry.get("Solid") == PropertyId::Solid);
    CHECK(registry.get("Damage") == PropertyId::Damage);
    CHECK(registry.get("SpawnPoint") == PropertyId::SpawnPoint);
    CHECK(registry.get("Nope") == PropertyId::Unknown);
    CHECK(registry.get("") == PropertyId::Unknown);
}

int main()
{
    testLoaderMinimal();
    testLoaderExternalTileset();
    testLoaderTileTypes();
    testLoaderErrors();
    testTileMapData();
    testPropertyRegistry();

    std::cout << (g_failures == 0 ? "PASS" : "FAIL") << " - "
              << (g_checks - g_failures) << "/" << g_checks << " checks\n";
    return g_failures == 0 ? 0 : 1;
}

#undef CHECK
#undef CHECK_EQ