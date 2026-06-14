#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "engine/renderer/Camera.h"
#include "engine/renderer/DebugRenderer.h"

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
    m_screenLines.clear();
    m_screenRects.clear();
    m_isoLines.clear();
    m_isoRects.clear();
    m_screenLines.reserve(256);
    m_screenRects.reserve(128);
    m_isoLines.reserve(256);
    m_isoRects.reserve(128);
#endif
}

void DebugRenderer::addScreenLine(Vec2f start, Vec2f end, const SDL_Color &color)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    m_screenLines.push_back(LineCommand{start, end, color});
#else
    (void)start;
    (void)end;
    (void)color;
#endif
}

void DebugRenderer::addScreenRect(Rectf rect, const SDL_Color &color, bool filled)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    if (rect.w < 0 || rect.h < 0)
        return;
    m_screenRects.push_back(RectCommand{rect, color, filled});
#else
    (void)rect;
    (void)color;
    (void)filled;
#endif
}

void DebugRenderer::addScreenCircle(Vec2f center, float radius, const SDL_Color &color, bool filled)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    const int segments = 24;

    if (filled)
    {
        int radiusInt = static_cast<int>(std::ceil(radius));
        for (int y = -radiusInt; y <= radiusInt; y++)
        {
            float dx = sqrtf(radius * radius - static_cast<float>(y * y));
            float x1 = center.x - dx;
            float x2 = center.x + dx;
            addScreenLine(Vec2f(x1, center.y + static_cast<float>(y)), Vec2f(x2, center.y + static_cast<float>(y)), color);
        }
    }
    else
    {
        for (int i = 0; i < segments; i++)
        {
            float angle1 = 2.0f * 3.14159f * static_cast<float>(i) / static_cast<float>(segments);
            float angle2 = 2.0f * 3.14159f * static_cast<float>(i + 1) / static_cast<float>(segments);

            Vec2f p1(center.x + cosf(angle1) * radius, center.y + sinf(angle1) * radius);
            Vec2f p2(center.x + cosf(angle2) * radius, center.y + sinf(angle2) * radius);

            addScreenLine(p1, p2, color);
        }
    }
#else
    (void)center;
    (void)radius;
    (void)color;
    (void)filled;
#endif
}

void DebugRenderer::addIsoLine(Vec2f start, Vec2f end, const SDL_Color &color)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    m_isoLines.push_back(LineCommand{start, end, color});
#else
    (void)start;
    (void)end;
    (void)color;
#endif
}

void DebugRenderer::addIsoRect(Rectf rect, const SDL_Color &color, bool filled)
{
#ifndef NDEBUG
    if (!m_enabled)
        return;

    if (rect.w < 0 || rect.h < 0)
        return;
    m_isoRects.push_back(RectCommand{rect, color, filled});
#else
    (void)rect;
    (void)color;
    (void)filled;
#endif
}

void DebugRenderer::flush(SDL_Renderer *renderer, const Camera &camera)
{
#ifndef NDEBUG
    if (!renderer || !m_enabled)
        return;

    SDL_BlendMode previousBlendMode = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(renderer, &previousBlendMode);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    const Vec2f offset = camera.getOffset();
    const float zoom = camera.getZoom();

    for (const LineCommand &line : m_isoLines)
    {
        const Vec2f a{
            (line.a.x - offset.x) * zoom,
            (line.a.y - offset.y) * zoom};
        const Vec2f b{
            (line.b.x - offset.x) * zoom,
            (line.b.y - offset.y) * zoom};

        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
        SDL_RenderLine(
            renderer,
            std::clamp(a.x, -32768.f, 32767.f),
            std::clamp(a.y, -32768.f, 32767.f),
            std::clamp(b.x, -32768.f, 32767.f),
            std::clamp(b.y, -32768.f, 32767.f));
    }

    for (const RectCommand &rectCmd : m_isoRects)
    {
        const Recti screenRect = camera.isoSpaceRectToScreen(rectCmd.rect);
        SDL_FRect sdlRect{(float)screenRect.x, (float)screenRect.y, (float)screenRect.w, (float)screenRect.h};

        SDL_SetRenderDrawColor(renderer, rectCmd.color.r, rectCmd.color.g, rectCmd.color.b, rectCmd.color.a);
        if (rectCmd.filled)
            SDL_RenderFillRect(renderer, &sdlRect);
        else
            SDL_RenderRect(renderer, &sdlRect);
    }

    for (const LineCommand &line : m_screenLines)
    {
        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
        SDL_RenderLine(
            renderer,
            std::clamp(line.a.x, -32768.f, 32767.f),
            std::clamp(line.a.y, -32768.f, 32767.f),
            std::clamp(line.b.x, -32768.f, 32767.f),
            std::clamp(line.b.y, -32768.f, 32767.f));
    }

    for (const RectCommand &rectCmd : m_screenRects)
    {
        SDL_FRect sdlRect{
            std::clamp(rectCmd.rect.x, -32768.f, 32767.f),
            std::clamp(rectCmd.rect.y, -32768.f, 32767.f),
            std::clamp(rectCmd.rect.w, -32768.f, 32767.f),
            std::clamp(rectCmd.rect.h, -32768.f, 32767.f)};

        SDL_SetRenderDrawColor(renderer, rectCmd.color.r, rectCmd.color.g, rectCmd.color.b, rectCmd.color.a);
        if (rectCmd.filled)
            SDL_RenderFillRect(renderer, &sdlRect);
        else
            SDL_RenderRect(renderer, &sdlRect);
    }

    // Note: We clear the command buffers after rendering for next frame.
    clear();
    SDL_SetRenderDrawBlendMode(renderer, previousBlendMode);
#else
    (void)renderer;
    (void)camera;
#endif
}
