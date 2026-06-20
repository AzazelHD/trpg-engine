#include "engine/core/Window.h"
#include <SDL3/SDL.h>
#include <string>
#include <stdexcept>

// [x] Constructor: SDL_CreateWindow + SDL_CreateRenderer (default backend) +
//     vsync (caller-supplied, default Enabled), then wrap m_sdlRenderer
//     into m_renderer.
Window::Window(const char *title, int w, int h, VSyncMode vsync)
    : m_width(w), m_height(h), m_vsync(vsync)
{
    m_window = SDL_CreateWindow(title, w, h, 0);
    if (!m_window)
    {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }
    m_sdlRenderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_sdlRenderer)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }
    if (!SDL_SetRenderVSync(m_sdlRenderer, static_cast<int>(m_vsync)))
    {
        std::string err = std::string("SDL_SetRenderVSync failed: ") + SDL_GetError();
        SDL_DestroyRenderer(m_sdlRenderer);
        m_sdlRenderer = nullptr;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        throw std::runtime_error(err);
    }
    m_renderer = Renderer(m_sdlRenderer);
}

// [x] Destructor: SDL_DestroyRenderer + SDL_DestroyWindow.
Window::~Window()
{
    if (m_sdlRenderer)
    {
        SDL_DestroyRenderer(m_sdlRenderer);
        m_sdlRenderer = nullptr;
    }
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

int Window::getWidth() const { return m_width; }
int Window::getHeight() const { return m_height; }

// [x] setFullscreen(): SDL_SetWindowFullscreen.
void Window::setFullscreen(bool enabled)
{
    if (m_window)
    {
        if (!SDL_SetWindowFullscreen(m_window, enabled))
        {
            throw std::runtime_error(std::string("SDL_SetWindowFullscreen failed: ") + SDL_GetError());
        }
    }
}

// [x] setTitle(): SDL_SetWindowTitle.
void Window::setTitle(const char *title)
{
    if (m_window && title)
    {
        SDL_SetWindowTitle(m_window, title);
    }
}

// [x] setResizable(): SDL_SetWindowResizable.
void Window::setResizable(bool enabled)
{
    if (m_window)
    {
        SDL_SetWindowResizable(m_window, enabled);
    }
}

// [x] setAspectRatio(): SDL_SetWindowAspectRatio.
void Window::setAspectRatio(float minAspect, float maxAspect)
{
    if (m_window)
    {
        SDL_SetWindowAspectRatio(m_window, minAspect, maxAspect);
    }
}

// [x] setVSync(): runtime toggle, kept in sync with m_vsync so getVSync()
//     reflects the renderer's actual state.
void Window::setVSync(VSyncMode mode)
{
    if (m_sdlRenderer)
    {
        if (!SDL_SetRenderVSync(m_sdlRenderer, static_cast<int>(mode)))
        {
            throw std::runtime_error(std::string("SDL_SetRenderVSync failed: ") + SDL_GetError());
        }
    }
    m_vsync = mode;
}