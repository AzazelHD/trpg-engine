#include "engine/renderer/Camera.h"
#include "engine/renderer/Renderer.h"
#include "engine/renderer/DebugRenderer.h"

#include <cmath>
#include <cstdlib>

void DebugRenderer::setEnabled(bool enabled)
{
#ifndef NDEBUG
    m_enabled = enabled;
#else
    (void)enabled;
#endif
}

bool DebugRenderer::isEnabled() const
{
#ifndef NDEBUG
    return m_enabled;
#else
    return false;
#endif
}

void DebugRenderer::clear()
{
#ifndef NDEBUG
    // Reset all debug draw buffers
    m_screenLines.clear();
    m_screenRects.clear();
    m_isoLines.clear();
    m_isoRects.clear();

    // Small pre-allocation to reduce allocations per frame
    m_screenLines.reserve(256);
    m_screenRects.reserve(128);
    m_isoLines.reserve(256);
    m_isoRects.reserve(128);
#endif
}

void DebugRenderer::addScreenLine(Vec2f start, Vec2f end, const Color &color)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    m_screenLines.push_back(LineCommand{start, end, color});
#endif
}

void DebugRenderer::addScreenRect(Rectf rect, const Color &color, bool filled)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    if (rect.w < 0 || rect.h < 0)
        return;

    m_screenRects.push_back(RectCommand{rect, color, filled});
#endif
}

void DebugRenderer::addScreenCircle(Vec2f center, float radius, const Color &color, bool filled)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    const int segments = 24;

    // Filled circle = horizontal scanlines
    if (filled)
    {
        int r = static_cast<int>(std::ceil(radius));

        for (int y = -r; y <= r; y++)
        {
            float dx = sqrtf(radius * radius - static_cast<float>(y * y));

            addScreenLine(
                {center.x - dx, center.y + static_cast<float>(y)},
                {center.x + dx, center.y + static_cast<float>(y)},
                color);
        }
    }
    else
    {
        // Wire circle = polygon approximation
        for (int i = 0; i < segments; i++)
        {
            float a1 = 2.0f * 3.14159f * i / segments;
            float a2 = 2.0f * 3.14159f * (i + 1) / segments;

            Vec2f p1(center.x + cosf(a1) * radius, center.y + sinf(a1) * radius);
            Vec2f p2(center.x + cosf(a2) * radius, center.y + sinf(a2) * radius);

            addScreenLine(p1, p2, color);
        }
    }
#endif
}

void DebugRenderer::addIsoLine(Vec2f start, Vec2f end, const Color &color)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    m_isoLines.push_back(LineCommand{start, end, color});
#endif
}

void DebugRenderer::addIsoRect(Rectf rect, const Color &color, bool filled)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    if (rect.w < 0 || rect.h < 0)
        return;

    m_isoRects.push_back(RectCommand{rect, color, filled});
#endif
}

void DebugRenderer::flush(Renderer *renderer, const Camera &camera)
{
#ifndef NDEBUG
    if (!renderer || !m_enabled)
        return;

    Renderer::BlendMode previousBlendMode = renderer->getBlendMode();
    renderer->setBlendMode(Renderer::BlendMode::Blend);

    const Vec2f offset = camera.getOffset();
    const float zoom = camera.getZoom();

    // ---------------- ISO LINES ----------------
    for (const LineCommand &line : m_isoLines)
    {
        Vec2f a{
            (line.a.x - offset.x) * zoom,
            (line.a.y - offset.y) * zoom};

        Vec2f b{
            (line.b.x - offset.x) * zoom,
            (line.b.y - offset.y) * zoom};

        renderer->setDrawColor(line.color);
        renderer->drawLine(a, b);
    }

    // ---------------- ISO RECTS ----------------
    for (const RectCommand &rectCmd : m_isoRects)
    {
        Recti screenRect = camera.isoSpaceRectToScreen(rectCmd.rect);
        Rectf screenRectF{
            (float)screenRect.x,
            (float)screenRect.y,
            (float)screenRect.w,
            (float)screenRect.h};

        renderer->setDrawColor(rectCmd.color);

        if (rectCmd.filled)
            renderer->fillRect(screenRectF);
        else
            renderer->drawRect(screenRectF);
    }

    // ---------------- SCREEN LINES ----------------
    for (const LineCommand &line : m_screenLines)
    {
        renderer->setDrawColor(line.color);
        renderer->drawLine(line.a, line.b);
    }

    // ---------------- SCREEN RECTS ----------------
    for (const RectCommand &rectCmd : m_screenRects)
    {
        renderer->setDrawColor(rectCmd.color);

        if (rectCmd.filled)
            renderer->fillRect(rectCmd.rect);
        else
            renderer->drawRect(rectCmd.rect);
    }

    // Clear after frame
    clear();

    renderer->setBlendMode(previousBlendMode);
#endif
}