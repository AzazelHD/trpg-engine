#pragma once

// Window wraps an SDL_Window + SDL_Renderer pair.
// The rest of the engine never touches SDL directly — they go through Window and Renderer wrappers.
//
// [x]: Declare the class with:
//   - Constructor: Window(const char* title, int w, int h)
//   - Destructor: must call SDL_DestroyRenderer and SDL_DestroyWindow
//   - Getters: getSDLWindow(), getSDLRenderer(), getWidth(), getHeight()
//   - Optional: setFullscreen(bool), setTitle(const char*)
//
// Important: do NOT include SDL.h in this header. Forward-declare SDL_Window and SDL_Renderer
// as struct pointers so callers don't pull in all of SDL just from this header.
// (You'll need SDL.h only in Window.cpp)

struct SDL_Window;
struct SDL_Renderer;

class Window
{
public:
    Window(const char *title, int w, int h);
    ~Window();

    // Disable copy
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    // Disable move (simplest safe option)
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] SDL_Window *getSDLWindow() const;
    [[nodiscard]] SDL_Renderer *getSDLRenderer() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    void setFullscreen(bool enabled);
    void setTitle(const char *title);

private:
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;

    const int m_width;
    const int m_height;
};