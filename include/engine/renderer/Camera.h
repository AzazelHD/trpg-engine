#pragma once
#include "engine/math/Vec2.h"
#include "engine/math/Rect.h"

// Camera owns the viewport transform: isometric projection + world offset + zoom + rotation.
//
// --- ISOMETRIC COORDINATE SYSTEM ---
// Tiles live on a logical grid (tile x, tile y).
// Isometric projection maps them to screen pixels like this:
//
//       screen.x = (tileX - tileY) * (tileW / 2)
//       screen.y = (tileX + tileY) * (tileH / 2)
//
// where tileW/tileH are the pixel dimensions of one tile (tileH is usually tileW/2).
// The camera offset then shifts the entire projected world so the right part is visible.
//
// Why isometric? It gives a 3D-feeling top-down view without a real 3D engine.
// The grid is still 2D data — only the display transform changes.
//
// --- BATTLEFIELD ROTATION ---
// Like Final Fantasy Tactics Advance and Tactics Ogre, the battlefield can be rotated
// 90° at a time (four cardinal orientations: R0, R90, R180, R270).
//
// Rotation is NOT applied in screen space. Instead, each tile's logical (x, y) is
// permuted before the iso projection formula runs. This means:
//   - The projection math stays unchanged.
//   - The tile renderer must change its draw order when rotation changes (back-to-front
//     depends on which corner of the diamond is currently "south").
//   - screenToTile() must apply the inverse permutation to recover the logical tile.
//
// Permutation table (mapW = map width in tiles, mapH = map height in tiles):
//
//   R0   → (x,  y)
//   R90  → (mapH-1-y,  x)
//   R180 → (mapW-1-x,  mapH-1-y)
//   R270 → (y,          mapW-1-x)
//
// setMapSize() must be called (on map load) so these pivot calculations are correct.
// rebuildBoundsCache() is called automatically whenever rotation or tile size changes.
//
// --- CAMERA PAN ---
// The player can pan the camera manually with dedicated keys (see KeyCode::CameraPan*).
// Pan speed should be a constant tiles-per-second value, scaled by dt each frame.
// The cursor can also trigger automatic camera following when it nears the screen edge.
//
// --- FULL TRANSFORM ORDER ---
// Given a logical tile position T:
//   1. rotateTile(T)         → permuted tile coordinate T'
//   2. tileToIso(T')         → iso-space position (pre-zoom)
//   3. subtract m_offset      → camera-relative iso position
//   4. multiply by m_zoom     → final screen position
//
// [x]: Declare the class with:
//   - tileToScreen(Vec2i tilePos) const → Vec2f
//       Apply rotation permutation, then iso formula, then subtract m_offset,
//       then apply m_zoom. This is the only place the full transform should live.
//
//   - screenToTile(Vec2f screenPos) const → Vec2i
//       Inverse of tileToScreen. Undo zoom and offset to get iso space, call
//       isoToTile() for the rotated tile coord, then apply unrotateTile() to
//       recover the logical tile position.
//       Formula (reverse the iso math, then round to the nearest tile center):
//           world.x = (screenPos.x / zoom + offset.x) / (tileW / 2)
//           world.y = (screenPos.y / zoom + offset.y) / (tileH / 2)
//           tileX = (world.x + world.y) / 2
//           tileY = (world.y - world.x) / 2
//
//   - pan(Vec2f delta)
//       Move the camera offset by delta (pixels). Called by input handling each frame.
//       Example: if CameraPanRight is held, call pan({panSpeed * dt, 0}).
//
//   - follow(Vec2f cursorScreenPos, float edgeThreshold, Vec2f screenSize, float dt)
//       Auto-pan when the cursor is within edgeThreshold pixels of a screen edge.
//       Uses a velocity model with dt-scaled motion and capped diagonal speed.
//
//   - trackTarget(Vec2f targetIsoPos, Vec2f screenSize, float dt)
//       Smoothly center an iso-space target on screen using the camera's current zoom.
//
//   - setMapBounds(Rectf mapBounds) / clampToBounds()
//       Cache and apply loose floating-island bounds in the same pre-zoom iso
//       space used by m_offset.
//
//   - setZoom(float zoom, Vec2f focalScreenPos)
//       Zoom anchored to a focal point: the iso-space point under focalScreenPos
//       is preserved after the zoom, preventing the disorienting drift that occurs
//       when zooming toward the world origin.
//
//   - setTileSize(int tileW, int tileH)  ← needed for the projection formulas
//   - setMapSize(int mapW, int mapH)     ← needed for rotation pivot math
//   - getOffset() const → Vec2f
//
// Members: m_offset (Vec2f), m_zoom (float), m_tileW (int), m_tileH (int),
//          m_mapW (int), m_mapH (int), m_rotation (Rotation).
class Camera
{
public:
    // -------------------------------------------------------------------------
    // FollowEasing — shapes the per-frame blend factor used by trackTarget().
    // -------------------------------------------------------------------------
    enum class FollowEasing
    {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
        Bounce
    };

    // -------------------------------------------------------------------------
    // [x]: Rotation — four cardinal orientations for FFTA / Tactics Ogre style views.
    //
    // Each value represents a clockwise rotation of the battlefield by that many
    // degrees. R0 is the default (north tile corner at screen top).
    //
    // Changing rotation requires:
    //   1. Re-permuting tile coordinates in tileToScreen / screenToTile.
    //   2. Rebuilding the iso bounds cache (rebuildBoundsCache).
    //   3. The tile renderer updating its back-to-front draw order.
    // -------------------------------------------------------------------------
    enum class Rotation
    {
        R0,   // Default orientation — (x, y) unchanged
        R90,  // 90° clockwise  — (mapH-1-y, x)
        R180, // 180°           — (mapW-1-x, mapH-1-y)
        R270, // 270° clockwise — (y, mapW-1-x)
    };

    Camera() = default;

    // --- Projection ---------------------------------------------------------

    // Full transform: rotateTile → tileToIso → subtract offset → apply zoom.
    Vec2f tileToScreen(Vec2i tilePos) const;

    // Full inverse: undo zoom+offset → isoToTile → unrotateTile.
    Vec2i screenToTile(Vec2f screenPos) const;

    // Flat 2D rect to screen (iso-space input only — input must already be in
    // iso space, not tile space; does not apply the rotation permutation).
    // Prefer tileToScreen() for tile positions.
    Recti isoSpaceRectToScreen(Rectf isoRect) const;

    // --- Rotation helpers ---------------------------------------------------

    // Apply the current rotation permutation to a logical tile coordinate.
    // Returns the permuted coordinate used for iso projection and draw ordering.
    Vec2i rotateTile(Vec2i tilePos) const;

    // Inverse of rotateTile — recovers the logical tile from a permuted coordinate.
    // Used by screenToTile() after isoToTile().
    Vec2i unrotateTile(Vec2i rotatedPos) const;

    // --- Control ------------------------------------------------------------

    void pan(Vec2f delta);

    // Edge-triggered pan: velocity proportional to cursor penetration depth.
    void follow(Vec2f cursorScreenPos, float edgeThreshold, Vec2f screenSize, float dt);

    // Smooth exponential lock toward an iso-space target centered on screen.
    void trackTarget(Vec2f targetIsoPos, Vec2f screenSize, float dt);

    void setMapBounds(Rectf mapBounds);
    void clampToBounds();

    // --- Configuration ------------------------------------------------------

    // Zoom anchored to focalScreenPos: the iso-space point under the focal
    // position is preserved, so the view does not drift toward the origin.
    // Pass screen centre for keyboard zoom, mouse position for scroll-wheel zoom.
    void setZoom(float zoom, Vec2f focalScreenPos);

    void zoomIn(float amount, Vec2f focalScreenPos);
    void zoomOut(float amount, Vec2f focalScreenPos);

    void setTileSize(int tileW, int tileH);

    // Must be called on map load so rotation pivot math uses the correct extents.
    // mapW and mapH are the tile-grid dimensions (not pixel dimensions).
    void setMapSize(int mapW, int mapH);

    // Rotate the battlefield by one step clockwise (R0→R90→R180→R270→R0).
    // Rebuilds the bounds cache and does NOT snap the camera — callers should
    // call trackTarget() immediately after to re-centre on the active unit.
    void rotateCW();

    // Rotate the battlefield by one step counter-clockwise.
    void rotateCCW();

    // Set rotation directly (e.g. when loading a saved game state).
    void setRotation(Rotation rotation);

    void setFollowDuration(float seconds); // trackTarget time constant (seconds)
    void setFollowEasing(FollowEasing easing);
    void setEdgePanSpeed(float pixelsPerSecond); // follow() max speed at full penetration
    void setMapBoundsMargin(float screenPixels); // visible background slack around island

    // --- Accessors ----------------------------------------------------------

    float getZoom() const { return m_zoom; }
    Vec2f getOffset() const { return m_offset; }
    Rotation getRotation() const { return m_rotation; }

    // Returns the draw-order axis for the tile renderer given the current rotation.
    // The tile layer must iterate tiles back-to-front along this axis so that
    // nearer tiles paint over farther ones correctly.
    //
    //   R0   → iterate x ascending,  y ascending
    //   R90  → iterate x ascending,  y descending
    //   R180 → iterate x descending, y descending
    //   R270 → iterate x descending, y ascending
    //
    // Returns {xDir, yDir} where +1 = ascending, -1 = descending.
    Vec2i getDrawOrder() const;

private:
    void rebuildBoundsCache();

    // Camera position stored in pre-zoom iso space.
    Vec2f m_offset = {0.f, 0.f};

    int m_tileW = 64;
    int m_tileH = 32;
    float m_zoom = 1.f;
    float m_followDuration = 0.15f; // trackTarget time constant (seconds)
    FollowEasing m_followEasing = FollowEasing::EaseOut;
    float m_edgePanSpeed = 300.0f;       // iso-space pixels/sec at full edge penetration
    float m_boundsMarginPixels = 256.0f; // screen pixels of slack beyond the island boundary

    // Tile-grid dimensions — required for rotation pivot calculations.
    // Set via setMapSize() on map load; both default to 0 (no pivot correction).
    int m_mapW = 0;
    int m_mapH = 0;

    // Current battlefield rotation. Affects tileToScreen(), screenToTile(),
    // rotateTile(), unrotateTile(), getDrawOrder(), and rebuildBoundsCache().
    Rotation m_rotation = Rotation::R0;

    Rectf m_mapBounds = {};
    Vec2f m_boundsMin = {0.f, 0.f};
    Vec2f m_boundsMax = {0.f, 0.f};
    bool m_hasMapBounds = false;
};