#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"

class Camera;

// DebugRenderer queues temporary debug primitives and draws them as an overlay.
// Intended for development-time visualization (bounds, triggers, culling, etc.).
//
// Coordinate spaces:
// - Screen primitives use post-camera pixel coordinates.
// - Iso primitives use the same pre-zoom iso space as Camera::m_offset.
class DebugRenderer
{
public:
    static DebugRenderer &instance()
    {
        static DebugRenderer renderer;
        return renderer;
    }

    void setEnabled(bool enabled);
    bool isEnabled() const;

    // Call once at the start of each frame to clear previous debug commands.
    // Also called internally after flush().
    void clear();

    void addScreenLine(Vec2f start, Vec2f end, const SDL_Color &color = SDL_Color{255, 64, 64, 255});
    void addScreenRect(Rectf rect, const SDL_Color &color = SDL_Color{64, 255, 64, 255}, bool filled = false);
    void addScreenCircle(Vec2f center, float radius, const SDL_Color &color, bool filled = true);

    void addIsoLine(Vec2f start, Vec2f end, const SDL_Color &color = SDL_Color{255, 192, 64, 255});
    void addIsoRect(Rectf rect, const SDL_Color &color = SDL_Color{64, 160, 255, 255}, bool filled = false);

    // Draw queued commands. In Release builds this becomes a no-op.
    void flush(SDL_Renderer *renderer, const Camera &camera);

private:
#ifndef NDEBUG
    struct LineCommand
    {
        Vec2f a;
        Vec2f b;
        SDL_Color color;
    };

    struct RectCommand
    {
        Rectf rect;
        SDL_Color color;
        bool filled;
    };

    bool m_enabled = true;
    std::vector<LineCommand> m_screenLines;
    std::vector<RectCommand> m_screenRects;
    std::vector<LineCommand> m_isoLines;
    std::vector<RectCommand> m_isoRects;
#endif
};
