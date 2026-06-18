#pragma once
#include "engine/renderer/Renderer.h"

// Window wraps an SDL_Window + SDL_Renderer pair and owns the engine Renderer
// built on top of that SDL_Renderer. The rest of the engine/game never touches
// SDL_Window/SDL_Renderer directly - only Window and Renderer wrap them.
//
// [x] Constructor: SDL_CreateWindow + SDL_CreateRenderer + vsync, then wrap the
//       SDL_Renderer* into m_renderer.
// [x] Destructor: SDL_DestroyRenderer + SDL_DestroyWindow.
// [x] getRenderer(): primary draw entry point for the rest of the engine.
// [x] getWidth() / getHeight().
// [x] setFullscreen(bool) - SDL_SetWindowFullscreen.
// [x] setTitle(const char*) - SDL_SetWindowTitle.
// [x] setResizable(bool) - SDL_SetWindowResizable (replaces game-side
//       SDL_GetRenderWindow + SDL_SetWindowResizable).
// [x] setAspectRatio(min, max) - SDL_SetWindowAspectRatio.
//
// Note: getSDLWindow()/getSDLRenderer() were removed - nothing outside
// Window/Renderer should hold raw SDL handles anymore.

struct SDL_Window;
struct SDL_Renderer;

class Window
{
public:
    Window(const char *title, int w, int h);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] Renderer &getRenderer() { return m_renderer; }
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    void setFullscreen(bool enabled);
    void setVSync(bool enabled);
    void setTitle(const char *title);
    void setResizable(bool enabled);
    void setAspectRatio(float minAspect, float maxAspect);

private:
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_sdlRenderer = nullptr;
    Renderer m_renderer;

    const int m_width;
    const int m_height;
};