#include "engine/renderer/Camera.h"
#include "engine/math/MathUtils.h"
#include <cmath>
#include <algorithm>

// =============================================================================
// Internal safety utilities
// =============================================================================

static inline float safeInv(float v)
{
    return (std::fabs(v) < 1e-6f) ? 1e-6f : v;
}

static inline float clamp01(float v)
{
    return std::clamp(v, 0.0f, 1.0f);
}

static float applyEasing(Camera::FollowEasing e, float t)
{
    t = clamp01(t);

    switch (e)
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
    default:
        return t;
    }
}

// =============================================================================
// Rotation
// =============================================================================

Vec2i Camera::rotateTile(Vec2i p) const
{
    const int w = (int)m_mapBounds.w;
    const int h = (int)m_mapBounds.h;

    if (w <= 0 || h <= 0)
        return p;

    switch (m_rotation)
    {
    case Rotation::R0:
        return p;
    case Rotation::R90:
        return {h - 1 - p.y, p.x};
    case Rotation::R180:
        return {w - 1 - p.x, h - 1 - p.y};
    case Rotation::R270:
        return {p.y, w - 1 - p.x};
    }
    return p;
}

Vec2i Camera::unrotateTile(Vec2i r) const
{
    const int w = (int)m_mapBounds.w;
    const int h = (int)m_mapBounds.h;

    if (w <= 0 || h <= 0)
        return r;

    switch (m_rotation)
    {
    case Rotation::R0:
        return r;
    case Rotation::R90:
        return {r.y, h - 1 - r.x};
    case Rotation::R180:
        return {w - 1 - r.x, h - 1 - r.y};
    case Rotation::R270:
        return {h - 1 - r.y, w - 1 - r.x};
    }
    return r;
}

Vec2i Camera::getDrawOrder() const
{
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
    Vec2f iso = tileToIso(rotateTile(tilePos), m_tileW, m_tileH);

    return {
        (iso.x - m_offset.x) * m_zoom,
        (iso.y - m_offset.y) * m_zoom};
}

Vec2i Camera::screenToTile(Vec2f screenPos) const
{
    float invZoom = safeInv(m_zoom);

    Vec2f iso{
        screenPos.x * invZoom + m_offset.x,
        screenPos.y * invZoom + m_offset.y};

    return unrotateTile(isoToTile(iso, m_tileW, m_tileH));
}

Recti Camera::isoSpaceRectToScreen(Rectf r) const
{
    return {
        (int)std::round((r.x - m_offset.x) * m_zoom),
        (int)std::round((r.y - m_offset.y) * m_zoom),
        (int)std::round(r.w * m_zoom),
        (int)std::round(r.h * m_zoom)};
}

// =============================================================================
// Control
// =============================================================================

void Camera::pan(Vec2f d)
{
    m_offset += d;
    clampToBounds();
}

void Camera::follow(Vec2f cursor, float threshold, Vec2f size, float dt)
{
    if (threshold <= 0.f || dt <= 0.f || size.x <= 0.f || size.y <= 0.f)
        return;

    Vec2f dir{0.f, 0.f};
    float inv = 1.f / std::max(threshold, 0.0001f);

    if (cursor.x < threshold)
        dir.x = -(threshold - cursor.x) * inv;
    else if (cursor.x > size.x - threshold)
        dir.x = (cursor.x - (size.x - threshold)) * inv;

    if (cursor.y < threshold)
        dir.y = -(threshold - cursor.y) * inv;
    else if (cursor.y > size.y - threshold)
        dir.y = (cursor.y - (size.y - threshold)) * inv;

    float len2 = dir.x * dir.x + dir.y * dir.y;
    if (len2 > 1e-6f)
    {
        float invLen = 1.f / std::sqrt(len2);
        dir.x *= invLen;
        dir.y *= invLen;
    }

    m_offset += dir * m_edgePanSpeed * dt;
}

void Camera::trackTarget(Vec2f target, Vec2f screenSize, float dt)
{
    if (dt <= 0.f)
        return;

    float z = std::max(m_zoom, 0.0001f);

    Vec2f desired{
        target.x - (screenSize.x * 0.5f) / z,
        target.y - (screenSize.y * 0.5f) / z};

    float tau = std::max(m_followDuration, 0.0001f);
    float a = clamp01(applyEasing(m_followEasing, 1.f - std::exp(-dt / tau)));

    m_offset.x = lerp(m_offset.x, desired.x, a);
    m_offset.y = lerp(m_offset.y, desired.y, a);
}

// =============================================================================
// Bounds (ONLY mapW/mapH system)
// =============================================================================

void Camera::rebuildBoundsCache()
{
    if (!hasMapBounds())
        return;

    const int w = (int)m_mapBounds.w;
    const int h = (int)m_mapBounds.h;

    if (w <= 0 || h <= 0)
        return;

    Vec2f c00 = tileToIso(rotateTile({0, 0}), m_tileW, m_tileH);
    Vec2f c10 = tileToIso(rotateTile({w - 1, 0}), m_tileW, m_tileH);
    Vec2f c01 = tileToIso(rotateTile({0, h - 1}), m_tileW, m_tileH);
    Vec2f c11 = tileToIso(rotateTile({w - 1, h - 1}), m_tileW, m_tileH);

    m_boundsMin.x = std::min({c00.x, c10.x, c01.x, c11.x});
    m_boundsMin.y = std::min({c00.y, c10.y, c01.y, c11.y});
    m_boundsMax.x = std::max({c00.x, c10.x, c01.x, c11.x});
    m_boundsMax.y = std::max({c00.y, c10.y, c01.y, c11.y});
}

void Camera::clampToBounds()
{
    if (!hasMapBounds())
        return;

    const float mx = m_boundsMarginPixels / std::max(m_zoom, 0.0001f);

    m_offset.x = std::clamp(m_offset.x, m_boundsMin.x - mx, m_boundsMax.x + mx);
    m_offset.y = std::clamp(m_offset.y, m_boundsMin.y - mx, m_boundsMax.y + mx);
}

// =============================================================================
// Zoom
// =============================================================================

void Camera::setZoom(float zoom, Vec2f focalScreenPos)
{
    zoom = std::clamp(zoom, 0.25f, 4.0f);

    const float oldZoom = std::max(m_zoom, 0.001f);

    Vec2f focalIso{
        focalScreenPos.x / oldZoom + m_offset.x,
        focalScreenPos.y / oldZoom + m_offset.y};

    m_zoom = zoom;

    m_offset = {
        focalIso.x - focalScreenPos.x / m_zoom,
        focalIso.y - focalScreenPos.y / m_zoom};
}

void Camera::zoomIn(float amount, Vec2f focalScreenPos)
{
    if (amount <= 0.f)
        return;
    setZoom(m_zoom + amount, focalScreenPos);
}

void Camera::zoomOut(float amount, Vec2f focalScreenPos)
{
    if (amount <= 0.f)
        return;
    setZoom(m_zoom - amount, focalScreenPos);
}

// =============================================================================
// Configuration
// =============================================================================

void Camera::setTileSize(int w, int h)
{
    m_tileW = std::max(1, w);
    m_tileH = std::max(1, h);
    rebuildBoundsCache();
}

void Camera::setMapSize(int mapWidth, int mapHeight)
{
    m_mapBounds = {0.f, 0.f, (float)mapWidth, (float)mapHeight};

    if (m_mapBounds.w <= 0.f || m_mapBounds.h <= 0.f)
    {
        m_boundsMin = {0.f, 0.f};
        m_boundsMax = {0.f, 0.f};
        return;
    }

    rebuildBoundsCache();
}

void Camera::setRotation(Rotation r)
{
    if (r == m_rotation)
        return;
    m_rotation = r;
    rebuildBoundsCache();
}

void Camera::rotateCW()
{
    m_rotation = static_cast<Rotation>((int(m_rotation) + 1) % 4);
    rebuildBoundsCache();
}

void Camera::rotateCCW()
{
    m_rotation = static_cast<Rotation>((int(m_rotation) + 3) % 4);
    rebuildBoundsCache();
}

void Camera::setFollowDuration(float s)
{
    m_followDuration = std::max(s, 0.01f);
}

void Camera::setFollowEasing(FollowEasing e)
{
    m_followEasing = e;
}

void Camera::setEdgePanSpeed(float s)
{
    m_edgePanSpeed = std::max(0.f, s);
}

void Camera::setMapBoundsMargin(float p)
{
    m_boundsMarginPixels = std::max(0.f, p);
}

bool Camera::isTileInsideMap(Vec2i tile) const
{
    const int w = (int)m_mapBounds.w;
    const int h = (int)m_mapBounds.h;

    return tile.x >= 0 &&
           tile.y >= 0 &&
           tile.x < w &&
           tile.y < h;
}