#pragma once
#include "engine/core/Timer.h"

#include <functional>
#include <memory>

// App is the top-level owner of the engine.
// It initialises SDL, creates the Window (which owns the engine::Renderer),
// owns the main loop, and shuts everything down cleanly.
//
// [x]: Constructor takes title/width/height + optional SceneFactory for the
//       consumer's first scene.
// [x]: run() drives the fixed-timestep accumulator loop and never returns
//       until the window is closed.
// [x]: getRenderer() returns engine::Renderer* (NOT SDL_Renderer*) — no SDL
//       types are visible through App's public API.
// [x]: showErrorDialog(title, message) — static helper wrapping
//       SDL_ShowSimpleMessageBox, so callers (e.g. main.cpp) don't need SDL.
// [x]: Private methods processEvents(), update(dt), render(alpha).
// [x]: Private members Window, Input, SceneStack — only forward-declared here.

class Window;
class Input;
class Scene;
class Renderer;
template <typename T>
class StateMachine;

class App
{
public:
    using SceneFactory = std::function<std::unique_ptr<Scene>()>;

    App(const char *title, int width, int height, SceneFactory initialSceneFactory = {});
    ~App();

    void run();

    // Returns the engine Renderer owned by the window.
    // Valid after construction; null before construction completes.
    [[nodiscard]] static Renderer *getRenderer() noexcept;

    // Returns the Window owning the renderer (for setResizable/setAspectRatio etc.)
    // Valid after construction; null before construction completes.
    [[nodiscard]] static Window *getWindow() noexcept;

    // Valid after construction; null before construction completes.
    [[nodiscard]] static StateMachine<Scene> *getSceneStack() noexcept;

    // Native OS error dialog. Safe to call even if no App instance exists yet
    // (e.g. from main()'s top-level catch block).
    static void showErrorDialog(const char *title, const char *message);

    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

private:
    void processEvents();
    void update(float dt);
    void render(float alpha);

private:
    bool m_running = true;

    float m_fixedStep = 1.0f / 60.0f;
    Timer m_timer;

    std::unique_ptr<Window> m_window;
    Input *m_input = nullptr;
    std::unique_ptr<StateMachine<Scene>> m_sceneStack;
};