#include <cmath>
#include <algorithm>
#include "engine/math/MathUtils.h"
#include "engine/renderer/Camera.h"

namespace
{
    float applyEasing(Camera::FollowEasing easing, float t)
    {
        switch (easing)
        {
        case Camera::FollowEasing::Linear:
            return linear(t);
        case Camera::FollowEasing::EaseIn:
            return easeIn(t);
        case Camera::FollowEasing::EaseOut:
            return easeOut(t);
        case Camera::FollowEasing::EaseInOut:
            return easeInOut(t);
        case Camera::FollowEasing::EaseInCubic:
            return easeInCubic(t);
        case Camera::FollowEasing::EaseOutCubic:
            return easeOutCubic(t);
        case Camera::FollowEasing::EaseInOutCubic:
            return easeInOutCubic(t);
        case Camera::FollowEasing::Bounce:
            return bounce(t);
        }
        return t;
    }
} // namespace

// =============================================================================
// Rotation helpers
// =============================================================================

Vec2i Camera::rotateTile(Vec2i p) const
{
    // Permute logical tile coordinates before iso projection.
    // Each case rotates the entire grid around its centre so a different
    // corner faces screen-top. mapW/mapH provide the pivot bounds.
    //
    //   R0   → identity
    //   R90  → (mapH-1-y,  x          )
    //   R180 → (mapW-1-x,  mapH-1-y   )
    //   R270 → (y,          mapW-1-x   )
    //
    // Guard: if setMapSize() has not been called the pivot dimensions are 0
    // and the subtraction would wrap to a large negative number. Treat as R0
    // (identity) and fire a warning so the bug surfaces immediately.
    if (m_rotation != Rotation::R0 && (m_mapW == 0 || m_mapH == 0))
    {
        // LOG_WARN("Camera::rotateTile called with rotation != R0 but setMapSize() "
        //          "has not been called — returning identity. Call setMapSize() on map load.");
        return p;
    }

    switch (m_rotation)
    {
    case Rotation::R0:
        return p;
    case Rotation::R90:
        return {m_mapH - 1 - p.y, p.x};
    case Rotation::R180:
        return {m_mapW - 1 - p.x, m_mapH - 1 - p.y};
    case Rotation::R270:
        return {p.y, m_mapW - 1 - p.x};
    }
    return p;
}

Vec2i Camera::unrotateTile(Vec2i r) const
{
    // Inverse permutation of rotateTile — recovers the logical tile position
    // from a rotated coordinate. Used by screenToTile() after isoToTile().
    //
    // Derivation (solve rotateTile equations for the original x, y):
    //   R90  inverse: (r.y,          mapH-1-r.x)
    //   R180 inverse: (mapW-1-r.x,   mapH-1-r.y)  ← same as R180 itself
    //   R270 inverse: (mapW-1-r.y,   r.x          )
    // Same guard as rotateTile — unrotation is undefined without map dimensions.
    if (m_rotation != Rotation::R0 && (m_mapW == 0 || m_mapH == 0))
    {
        // LOG_WARN("Camera::unrotateTile called with rotation != R0 but setMapSize() "
        //          "has not been called — returning identity. Call setMapSize() on map load.");
        return r;
    }

    switch (m_rotation)
    {
    case Rotation::R0:
        return r;
    case Rotation::R90:
        return {r.y, m_mapH - 1 - r.x};
    case Rotation::R180:
        return {m_mapW - 1 - r.x, m_mapH - 1 - r.y};
    case Rotation::R270:
        return {m_mapW - 1 - r.y, r.x};
    }
    return r;
}

Vec2i Camera::getDrawOrder() const
{
    // Returns {xDir, yDir} for the tile renderer's back-to-front iteration.
    // +1 = ascending index, -1 = descending index.
    //
    // Derivation: rotateTile maps logical (tx,ty) to projected (px,py).
    // Painter's algorithm requires drawing in order of increasing (px+py).
    //
    //   R0:   px+py = tx+ty              → increasing tx+ty → x asc,  y asc  → { 1,  1}
    //   R90:  px+py = tx−ty+(H-1)        → increasing tx-ty → x asc,  y desc → { 1, -1}
    //   R180: px+py = (W+H-2)−(tx+ty)    → decreasing tx+ty → x desc, y desc → {-1, -1}
    //   R270: px+py = ty−tx+(W-1)        → increasing ty-tx → x desc, y asc  → {-1,  1}
    // Keep draw order consistent with rotateTile()/unrotateTile(). If map size
    // has not been provided yet, those helpers fall back to identity rotation.
    if (m_rotation != Rotation::R0 && (m_mapW == 0 || m_mapH == 0))
        return {1, 1};

    switch (m_rotation)
    {
    case Rotation::R0:
        return {1, 1};
    case Rotation::R90:
        return {1, -1};
    case Rotation::R180:
        return {-1, -1};
    case Rotation::R270:
        return {-1, 1};
    }
    return {1, 1};
}

// =============================================================================
// Projection
// =============================================================================

Vec2f Camera::tileToScreen(Vec2i tilePos) const
{
    // Full transform:
    //   1. rotateTile  — permute logical coords for current battlefield rotation.
    //   2. tileToIso   — apply isometric projection formula.
    //   3. subtract m_offset — shift so the right region of iso space is visible.
    //   4. multiply by m_zoom — scale to screen pixels.
    Vec2f iso = tileToIso(rotateTile(tilePos), m_tileW, m_tileH);
    return Vec2f{
        (iso.x - m_offset.x) * m_zoom,
        (iso.y - m_offset.y) * m_zoom};
}

Vec2i Camera::screenToTile(Vec2f screenPos) const
{
    // Full inverse:
    //   1. Undo zoom and offset to recover the iso-space coordinate.
    //   2. isoToTile  — snap to nearest tile on the rotated grid.
    //   3. unrotateTile — recover the logical tile position.
    Vec2f iso{
        screenPos.x / m_zoom + m_offset.x,
        screenPos.y / m_zoom + m_offset.y};

    return unrotateTile(isoToTile(iso, m_tileW, m_tileH));
}

Recti Camera::isoSpaceRectToScreen(Rectf isoRect) const
{
    return Recti{
        static_cast<int>(std::round((isoRect.x - m_offset.x) * m_zoom)),
        static_cast<int>(std::round((isoRect.y - m_offset.y) * m_zoom)),
        static_cast<int>(std::round(isoRect.w * m_zoom)),
        static_cast<int>(std::round(isoRect.h * m_zoom))};
}

// =============================================================================
// Control
// =============================================================================

void Camera::pan(Vec2f delta)
{
    m_offset += delta;
}

void Camera::follow(Vec2f cursorScreenPos, float edgeThreshold, Vec2f screenSize, float dt)
{
    // Edge-triggered pan — velocity model.
    //
    // Normalized penetration depth in [0, 1]: 0 at the zone boundary, 1 at screen edge.
    // Camera speed = penetration * m_edgePanSpeed, dt-scaled → frame-independent.
    // Diagonal penetration is normalized so corners do not exceed the configured max speed.
    //
    // Deliberately no lerp: lerp on top of velocity is double-damping (rubber band + lag).
    // For smooth unit tracking use trackTarget().
    //
    // WARNING: do not call both follow() and trackTarget() in the same frame.
    // Both write to m_offset — follow() will fight trackTarget()'s smoothing and
    // produce erratic camera movement. Use one or the other per frame depending
    // on whether the player is panning or a unit is being tracked.

    if (edgeThreshold <= 0.0f || dt <= 0.0f || screenSize.x <= 0.0f || screenSize.y <= 0.0f)
        return;

    const float invThreshold = 1.0f / edgeThreshold;
    Vec2f direction = {0.f, 0.f};

    if (cursorScreenPos.x < edgeThreshold)
        direction.x = -std::clamp((edgeThreshold - cursorScreenPos.x) * invThreshold, 0.0f, 1.0f);
    else if (cursorScreenPos.x > screenSize.x - edgeThreshold)
        direction.x = std::clamp((cursorScreenPos.x - (screenSize.x - edgeThreshold)) * invThreshold, 0.0f, 1.0f);

    if (cursorScreenPos.y < edgeThreshold)
        direction.y = -std::clamp((edgeThreshold - cursorScreenPos.y) * invThreshold, 0.0f, 1.0f);
    else if (cursorScreenPos.y > screenSize.y - edgeThreshold)
        direction.y = std::clamp((cursorScreenPos.y - (screenSize.y - edgeThreshold)) * invThreshold, 0.0f, 1.0f);

    // Normalize diagonal so corner speed matches cardinal speed.
    const float lengthSq = direction.x * direction.x + direction.y * direction.y;
    if (lengthSq > 0.0f)
    {
        const float length = std::sqrt(lengthSq);
        const float scale = std::min(length, 1.0f) / length;
        direction.x *= scale;
        direction.y *= scale;
    }

    m_offset.x += direction.x * m_edgePanSpeed * dt;
    m_offset.y += direction.y * m_edgePanSpeed * dt;
}

void Camera::trackTarget(Vec2f targetIsoPos, Vec2f screenSize, float dt)
{
    // Exponential smoothing toward a centered iso-space target — position model.
    // targetIsoPos and m_offset are both expressed in pre-zoom iso space.
    //
    // alpha = 1 - exp(-dt / tau): frame-independent convergence.
    // m_followDuration is tau (time constant, not a hard deadline):
    //   ~63% closed after 1×tau, ~95% after 3×tau, ~99% after 5×tau.
    // Easing shapes alpha: slightly tweaks the per-frame blend factor.

    if (dt <= 0.0f || screenSize.x <= 0.0f || screenSize.y <= 0.0f)
        return;

    const float safeZoom = std::max(m_zoom, 0.001f);
    const Vec2f desiredOffset{
        targetIsoPos.x - (screenSize.x * 0.5f) / safeZoom,
        targetIsoPos.y - (screenSize.y * 0.5f) / safeZoom};
    const float tau = std::max(m_followDuration, 0.001f);
    const float alpha = std::clamp(applyEasing(m_followEasing, 1.0f - std::exp(-dt / tau)), 0.0f, 1.0f);

    m_offset.x = lerp(m_offset.x, desiredOffset.x, alpha);
    m_offset.y = lerp(m_offset.y, desiredOffset.y, alpha);
}

// =============================================================================
// Map bounds
// =============================================================================

void Camera::rebuildBoundsCache()
{
    // Project all four tile-space corners of the map through the current
    // rotation + iso transform, then take the axis-aligned min/max.
    //
    // Called on map load, tile size change, and rotation change.
    // Rotation changes which corner ends up at which iso extreme, so the
    // cache must be rebuilt every time m_rotation changes.
    if (!m_hasMapBounds)
        return;

    const int x0 = static_cast<int>(m_mapBounds.x);
    const int y0 = static_cast<int>(m_mapBounds.y);
    const int x1 = static_cast<int>(m_mapBounds.x + std::max(0.0f, m_mapBounds.w - 1.0f));
    const int y1 = static_cast<int>(m_mapBounds.y + std::max(0.0f, m_mapBounds.h - 1.0f));

    // Rotate each corner before projecting — bounds must reflect the rotated layout.
    const Vec2f c00 = tileToIso(rotateTile(Vec2i{x0, y0}), m_tileW, m_tileH);
    const Vec2f c10 = tileToIso(rotateTile(Vec2i{x1, y0}), m_tileW, m_tileH);
    const Vec2f c01 = tileToIso(rotateTile(Vec2i{x0, y1}), m_tileW, m_tileH);
    const Vec2f c11 = tileToIso(rotateTile(Vec2i{x1, y1}), m_tileW, m_tileH);

    m_boundsMin.x = std::min({c00.x, c10.x, c01.x, c11.x});
    m_boundsMax.x = std::max({c00.x, c10.x, c01.x, c11.x});
    m_boundsMin.y = std::min({c00.y, c10.y, c01.y, c11.y});
    m_boundsMax.y = std::max({c00.y, c10.y, c01.y, c11.y});
}

void Camera::setMapBounds(Rectf mapBounds)
{
    // Compute and cache the projected iso corners of the map once (call on map load).
    // Floating-island style: allows background to show, but prevents infinite drift.
    // If tile size or rotation changes later, both setTileSize() and setRotation()
    // rebuild this cache automatically.
    if (mapBounds.w <= 0.0f || mapBounds.h <= 0.0f)
    {
        m_mapBounds = {};
        m_hasMapBounds = false;
        return;
    }

    m_mapBounds = mapBounds;
    m_hasMapBounds = true;
    rebuildBoundsCache();
}

void Camera::clampToBounds()
{
    // Apply cached bounds every frame. Cheap: no projection math here.
    // m_offset lives in pre-zoom iso space, so cached bounds and clamp math do too.
    if (!m_hasMapBounds)
        return;

    const float safeZoom = std::max(m_zoom, 0.001f);
    // Convert screen-space margin into pre-zoom iso space so the visible
    // background strip stays constant regardless of zoom level.
    const float marginX = m_boundsMarginPixels / safeZoom;
    const float marginY = m_boundsMarginPixels / safeZoom;

    m_offset.x = std::clamp(m_offset.x, m_boundsMin.x - marginX, m_boundsMax.x + marginX);
    m_offset.y = std::clamp(m_offset.y, m_boundsMin.y - marginY, m_boundsMax.y + marginY);
}

// =============================================================================
// Configuration
// =============================================================================

void Camera::setZoom(float newZoom, Vec2f focalScreenPos)
{
    newZoom = std::clamp(newZoom, 0.25f, 4.0f);
    if (newZoom == m_zoom)
        return;

    // Pin the iso-space point under focalScreenPos so it does not drift.
    // At old zoom:  focalIso = focalScreenPos / oldZoom + m_offset
    // At new zoom:  focalIso = focalScreenPos / newZoom + new_offset
    // Solving for new_offset preserves the focal point on screen.
    const Vec2f focalIso{
        focalScreenPos.x / m_zoom + m_offset.x,
        focalScreenPos.y / m_zoom + m_offset.y};

    m_zoom = newZoom;

    m_offset = {
        focalIso.x - focalScreenPos.x / m_zoom,
        focalIso.y - focalScreenPos.y / m_zoom};

    if (m_hasMapBounds)
        clampToBounds();
}

void Camera::zoomIn(float amount, Vec2f focalScreenPos)
{
    setZoom(m_zoom + amount, focalScreenPos);
}

void Camera::zoomOut(float amount, Vec2f focalScreenPos)
{
    setZoom(m_zoom - amount, focalScreenPos);
}

void Camera::setTileSize(int tileW, int tileH)
{
    // Prevent invalid tile sizes — isoToTile() divides by tile dimensions internally.
    if (tileW <= 0)
        tileW = 1;
    if (tileH <= 0)
        tileH = 1;

    if (tileW == m_tileW && tileH == m_tileH)
        return;

    m_tileW = tileW;
    m_tileH = tileH;

    if (m_hasMapBounds)
        rebuildBoundsCache();
}

void Camera::setMapSize(int mapW, int mapH)
{
    // Store tile-grid dimensions for rotation pivot math.
    // Must be called on map load before any rotation is applied.
    // Values of 0 are allowed (no-op rotation for empty maps).
    m_mapW = std::max(mapW, 0);
    m_mapH = std::max(mapH, 0);

    if (m_hasMapBounds)
        rebuildBoundsCache();
}

void Camera::setRotation(Rotation rotation)
{
    if (rotation == m_rotation)
        return;

    m_rotation = rotation;

    // Bounds are cached in iso space and rotation changes the iso extents,
    // so the cache must be rebuilt before the next clampToBounds() call.
    if (m_hasMapBounds)
        rebuildBoundsCache();
}

void Camera::rotateCW()
{
    // Advance one step clockwise: R0 → R90 → R180 → R270 → R0.
    // Cast to int to avoid UB on enum arithmetic.
    const int next = (static_cast<int>(m_rotation) + 1) % 4;
    setRotation(static_cast<Rotation>(next));
}

void Camera::rotateCCW()
{
    // Retreat one step counter-clockwise: R0 → R270 → R180 → R90 → R0.
    const int next = (static_cast<int>(m_rotation) + 3) % 4;
    setRotation(static_cast<Rotation>(next));
}

void Camera::setFollowDuration(float seconds)
{
    m_followDuration = std::max(seconds, 0.001f);
}

void Camera::setFollowEasing(FollowEasing easing)
{
    m_followEasing = easing;
}

void Camera::setEdgePanSpeed(float pixelsPerSecond)
{
    m_edgePanSpeed = std::max(pixelsPerSecond, 0.0f);
}

void Camera::setMapBoundsMargin(float screenPixels)
{
    m_boundsMarginPixels = std::max(screenPixels, 0.0f);
}