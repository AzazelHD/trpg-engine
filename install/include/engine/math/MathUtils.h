#pragma once
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "engine/math/Vec2.h"

// Free utility functions for math used throughout the engine and game.
// All functions are inline — bodies live right here, no .cpp needed.

// [x]: Implement lerp
//   float lerp(float a, float b, float t)
//   Returns a + (b - a) * t. t should be in [0, 1].
//   Used for smooth camera follow and the render interpolation alpha.

// --- EASING FUNCTIONS ---
// t should be in [0, 1]
inline float linear(float t) { return t; }
inline float easeIn(float t) { return t * t; }
inline float easeOut(float t) { return t * (2 - t); }
inline float easeInOut(float t) { return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t; }
inline float easeInCubic(float t) { return t * t * t; }
inline float easeOutCubic(float t)
{
    t -= 1.0f;
    return t * t * t + 1.0f;
}
inline float easeInOutCubic(float t) { return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1; }
inline float bounce(float t)
{
    if (t < 1 / 2.75f)
        return 7.5625f * t * t;
    else if (t < 2 / 2.75f)
    {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    }
    else if (t < 2.5f / 2.75f)
    {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    }
    else
    {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

// [x]: Implement Manhattan distance (used by Pathfinder heuristic and EnemyAI)
//   int manhattanDistance(Vec2i a, Vec2i b)
//   Returns abs(a.x - b.x) + abs(a.y - b.y).

inline int manhattanDistance(Vec2i a, Vec2i b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

// --- ISOMETRIC PROJECTION HELPERS ---
// These implement the standard 2:1 isometric formula.
// tileW and tileH are the pixel dimensions of one isometric tile (tileH = tileW / 2 typically).
//
// [x]: Implement tileToIso
//   Vec2f tileToIso(Vec2i tile, int tileW, int tileH)
//   Converts a logical tile coordinate to an isometric screen position (before camera offset).
//   Formula:
//       x = (tile.x - tile.y) * (tileW / 2)
//       y = (tile.x + tile.y) * (tileH / 2)

inline Vec2f tileToIso(Vec2i tile, int tileW, int tileH)
{
    const float halfTileW = tileW * 0.5f;
    const float halfTileH = tileH * 0.5f;
    const float x = (tile.x - tile.y) * halfTileW;
    const float y = (tile.x + tile.y) * halfTileH;
    return Vec2f{x, y};
}

// [x]: Implement isoToTile
//   Vec2i isoToTile(Vec2f isoPos, int tileW, int tileH)
//   Inverse of tileToIso at tile centers — converts a projected position back to a
//   tile coordinate using round-to-nearest semantics.
//   Formula (derive by solving the two equations above for tile.x and tile.y):
//       tile.x = round( isoPos.x / halfW + isoPos.y / halfH ) / 2
//       tile.y = round(-isoPos.x / halfW + isoPos.y / halfH ) / 2
//   where halfW = tileW / 2, halfH = tileH / 2.
//   Note: this inverse is approximate away from tile centers due to rounding.
//   Camera::screenToTile() currently forwards the recovered iso coordinate directly
//   to this helper.

inline Vec2i isoToTile(Vec2f iso, int tileW, int tileH)
{
    const float halfW = tileW * 0.5f;
    const float halfH = tileH * 0.5f;

    const float x = (iso.x / halfW + iso.y / halfH) * 0.5f;
    const float y = (iso.y / halfH - iso.x / halfW) * 0.5f;

    return Vec2i{
        static_cast<int>(std::round(x)),
        static_cast<int>(std::round(y))};
}
