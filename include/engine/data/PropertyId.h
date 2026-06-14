#pragma once
#include <cstdint>

// [x]: Internal engine IDs used instead of strings after Tiled import.
// Strings are only used during loading; runtime uses these enums.
enum class PropertyId : std::uint32_t
{
    Unknown = 0,
    Solid,
    Damage,
    SpawnPoint
    // Add more in the future
};