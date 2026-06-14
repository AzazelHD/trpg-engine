#include "engine/core/Window.h"
#include <SDL3/SDL.h>
#include <string>
#include <stdexcept>

// [x]: Implement Window constructor.
//   Call SDL_CreateWindow() with the title, w, h, and 0 for flags (shown by default in SDL3).
//   Then call SDL_CreateRenderer() with nullptr for default renderer name.
//   Enable vsync via SDL_SetRenderVSync(renderer, 1).
//   If either returns nullptr, throw a std::runtime_error with SDL_GetError().

Window::Window(const char *title, int w, int h)
    : m_width(w), m_height(h)
{
    m_window = SDL_CreateWindow(
        title,
        w,
        h,
        0);

    if (!m_window)
    {
        throw std::runtime_error(
            std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    m_renderer = SDL_CreateRenderer(
        m_window,
        nullptr);

    if (!m_renderer)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;

        throw std::runtime_error(
            std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }

    SDL_SetRenderVSync(m_renderer, 1);
}

Window::~Window()
{
    if (m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

SDL_Window *Window::getSDLWindow() const
{
    return m_window;
}

SDL_Renderer *Window::getSDLRenderer() const
{
    return m_renderer;
}

int Window::getWidth() const
{
    return m_width;
}

int Window::getHeight() const
{
    return m_height;
}

void Window::setFullscreen(bool enabled)
{
    if (m_window)
    {
        if (!SDL_SetWindowFullscreen(m_window, enabled))
        {
            throw std::runtime_error(
                std::string("SDL_SetWindowFullscreen failed: ") + SDL_GetError());
        }
    }
}

void Window::setTitle(const char *title)
{
    if (m_window && title)
    {
        SDL_SetWindowTitle(m_window, title);
    }
}