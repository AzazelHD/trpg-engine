#pragma once
#include <functional>
#include <memory>
#include "engine/core/Timer.h"

// App is the top-level owner of the engine.
// It initialises SDL, creates the Window, owns the main loop, and shuts everything down cleanly.
//
// [x]: Declare the class with:
//   - A constructor that takes a window title, width, and height.
//     Consumers may also pass a SceneFactory to provide the first scene without
//     modifying engine internals.
//   - A run() method: this is the entry point called from main(). It should not return until
//     the user closes the game.
//   - Private methods: processEvents(), update(float dt), render().
//   - Private members: Window, Input, SceneStack (or similar) — only forward-declare here,
//     include their headers in App.cpp to keep compile times low.
//
// Key concept: App drives the fixed-timestep loop. Look up the "accumulator pattern" —
// you simulate the world in fixed 16ms steps, then render with an interpolation alpha
// so motion is smooth even if render FPS differs from sim FPS.

struct SDL_Renderer;

class Window;
class Input;
class Scene;
template <typename T>
class StateMachine;

class App
{
public:
    using SceneFactory = std::function<std::unique_ptr<Scene>()>;

    // Pass an optional factory when the consumer wants App to bootstrap directly
    // into its first game-owned scene.
    App(const char *title, int width, int height, SceneFactory initialSceneFactory = {});
    ~App();

    // Main application loop
    void run();

    // Returns the SDL_Renderer owned by the window.
    // Valid after construction; null before construction completes.
    [[nodiscard]] static SDL_Renderer *getRenderer() noexcept;

    // Returns the global scene stack used by the running app instance.
    // Valid after construction; null before construction completes.
    [[nodiscard]] static StateMachine<Scene> *getSceneStack() noexcept;

    // Disable copy/move
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

    // Simulation timing defaults to 60 updates per second.
    // Keeping these as members makes runtime tuning (settings menu) straightforward later.
    float m_targetFps = 60.0f;
    float m_fixedStep = 1.0f / 60.0f;
    Timer m_timer;

    std::unique_ptr<Window> m_window;
    Input *m_input = nullptr;
    std::unique_ptr<StateMachine<Scene>> m_sceneStack;
};