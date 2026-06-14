// engine/include/engine/data/TileClass.h
#pragma once
//
// TileClass — canonical terrain and object class registry.
//
// These string_view constants must match the "Class" field set on tiles
// (in the Tiled tileset editor) and on objects (in the Tiled object layer)
// exactly — comparisons are case-sensitive.
//
// The uint32_t flag constants are the in-memory bitmask equivalents used
// by gameplay systems. Each flag occupies one bit.
//
// Rule groups (kWalkable etc.) are convenience masks for movement/action
// checks — prefer them over testing individual flags where possible.
//
// To add a new terrain class:
//   1. Add a string constant below (must match Tiled exactly).
//   2. Add a Flag constant (next available bit).
//   3. Add it to any relevant rule group mask.
//   4. Add a case to toFlag().
//   5. Set the class on the tile(s) in the Tiled tileset editor.
//
// To add a new object class (spawn, trigger, zone …):
//   1. Add a string constant in the Object classes section below.
//   2. Handle it in BattleMap::buildFrom() object layer pass.
//   3. Place point objects with that class in Tiled.

#include <cstdint>
#include <string_view>

namespace TileClass
{
    // ── Terrain class strings (set on tiles in the tileset) ───────────────
    // Must match the "Class" field in Tiled exactly (lowercase + underscore).

    inline constexpr std::string_view Normal = "normal";
    inline constexpr std::string_view Grass = "grass";
    inline constexpr std::string_view GrassEva = "grass_eva"; // +evasion bonus
    inline constexpr std::string_view GrassAtk = "grass_atk"; // +attack bonus
    inline constexpr std::string_view GrassMag = "grass_mag"; // +magic bonus

    // ── Object class strings (set on objects in object layers) ────────────
    // Must match the "Class" field on Tiled point objects exactly.

    // Player (team 0) - human controlled
    inline constexpr std::string_view SpawnPlayer = "spawn_player";
    // Allies (team 1) - AI controlled, fights with player
    inline constexpr std::string_view SpawnAlly = "spawn_ally";
    // Enemies (team 2, 3, 4...) - AI controlled, hostile to everyone
    inline std::string spawnEnemyTeam(int team)
    {
        return "spawn_enemy_" + std::to_string(team);
    }

    // ── Terrain bitmask flags ─────────────────────────────────────────────
    // One bit per terrain class. uint32_t gives 32 slots.

    inline constexpr uint32_t FlagNormal = 1u << 0;
    inline constexpr uint32_t FlagGrass = 1u << 1;
    inline constexpr uint32_t FlagGrassEva = 1u << 2;
    inline constexpr uint32_t FlagGrassAtk = 1u << 3;
    inline constexpr uint32_t FlagGrassMag = 1u << 4;

    // ── Rule groups ───────────────────────────────────────────────────────

    // All terrain a unit can walk onto (height/jump checks applied separately).
    inline constexpr uint32_t kWalkable =
        FlagNormal | FlagGrass | FlagGrassEva | FlagGrassAtk | FlagGrassMag;

    // All grass variants — useful for ability/skill targeting checks.
    inline constexpr uint32_t kAnyGrass =
        FlagGrass | FlagGrassEva | FlagGrassAtk | FlagGrassMag;

    // ── Helpers ───────────────────────────────────────────────────────────

    // Convert a Tiled terrain class string to its bitmask flag.
    // Returns 0 if the name is unrecognised (wall filler tiles, etc.).
    [[nodiscard]] inline constexpr uint32_t toFlag(std::string_view name) noexcept
    {
        if (name == Normal)
            return FlagNormal;
        if (name == Grass)
            return FlagGrass;
        if (name == GrassEva)
            return FlagGrassEva;
        if (name == GrassAtk)
            return FlagGrassAtk;
        if (name == GrassMag)
            return FlagGrassMag;
        return 0u;
    }

} // namespace TileClass