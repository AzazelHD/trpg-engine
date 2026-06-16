#pragma once
#include "engine/math/Vec2.h"
#include "engine/math/Rect.h"

//
// =============================================================================
// Camera — deterministic 2D isometric viewport controller
// =============================================================================
//
// Owns the viewport transform for tile-based maps.
//
// Responsibilities:
//   - Tile ↔ screen coordinate conversion.
//   - Zooming around a focal point.
//   - Manual and automatic panning.
//   - Target tracking.
//   - Battlefield rotation.
//   - Optional camera bounds.
//
// Coordinate spaces:
//
//   Tile space:
//     Logical grid coordinates.
//
//   Iso space:
//     Projected world coordinates before camera offset and zoom.
//
//   Screen space:
//     Final pixel coordinates after camera transform.
//
// Rotation affects projection, tile picking and draw order.
// setMapBounds() must be called before using rotated views.
// =============================================================================
class Camera
{
public:
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

    enum class Rotation
    {
        R0,
        R90,
        R180,
        R270
    };

    Camera() = default;

    // Projection
    Vec2f tileToScreen(Vec2i tilePos) const;
    Vec2i screenToTile(Vec2f screenPos) const;
    Recti isoSpaceRectToScreen(Rectf isoRect) const;

    // Rotation
    Vec2i rotateTile(Vec2i tilePos) const;
    Vec2i unrotateTile(Vec2i rotatedPos) const;
    Vec2i getDrawOrder() const;

    // Control
    void pan(Vec2f delta);
    void follow(Vec2f cursorScreenPos, float edgeThreshold, Vec2f screenSize, float dt);
    void trackTarget(Vec2f targetIsoPos, Vec2f screenSize, float dt);

    // Bounds (single source of truth)
    // mapWidth = tile columns, mapHeight = tile rows.
    void setMapSize(int mapWidth, int mapHeight);
    void clampToBounds();

    // Zoom
    void setZoom(float zoom, Vec2f focalScreenPos);
    void zoomIn(float amount, Vec2f focalScreenPos);
    void zoomOut(float amount, Vec2f focalScreenPos);

    // Config
    void setTileSize(int tileW, int tileH);

    void setRotation(Rotation rotation);
    void rotateCW();
    void rotateCCW();

    void setFollowDuration(float seconds);
    void setFollowEasing(FollowEasing easing);
    void setEdgePanSpeed(float pixelsPerSecond);
    void setMapBoundsMargin(float screenPixels);

    // Access
    float getZoom() const { return m_zoom; }
    Vec2f getOffset() const { return m_offset; }
    Rotation getRotation() const { return m_rotation; }

    Vec2f getMapOrigin() const { return {m_mapBounds.x, m_mapBounds.y}; }
    Vec2f getMapSize() const { return {m_mapBounds.w, m_mapBounds.h}; }

    bool hasMapBounds() const { return m_mapBounds.w > 0.f && m_mapBounds.h > 0.f; }

    bool isTileInsideMap(Vec2i tile) const;

private:
    void rebuildBoundsCache();

private:
    // Camera transform
    Vec2f m_offset{0.f, 0.f};
    float m_zoom = 1.f;

    int m_tileW = 64;
    int m_tileH = 32;

    Rotation m_rotation = Rotation::R0;

    // Follow
    float m_followDuration = 0.15f;
    FollowEasing m_followEasing = FollowEasing::EaseOut;

    float m_edgePanSpeed = 300.f;
    float m_boundsMarginPixels = 256.f;

    // SINGLE SOURCE OF TRUTH
    Rectf m_mapBounds{0.f, 0.f, 0.f, 0.f};

    // Derived cache (iso-space)
    Vec2f m_boundsMin{0.f, 0.f};
    Vec2f m_boundsMax{0.f, 0.f};
};