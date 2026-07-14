#pragma once
#include "engine/core/Timer.h"
#include <functional>
#include <memory>
// App is the top-level owner of the engine.
// It initialises SDL, creates the Window (which owns the engine::Renderer),
// owns the main loop, and shuts everything down cleanly.
//
// [x]: Constructor takes title/width/height + optional SceneFactory for the
//       consumer's first scene, plus an explicit fixed-timestep rate and
//       render frame-rate preset.
// [x]: run() drives the fixed-timestep accumulator loop and never returns
//       until the window is closed. A manual frame limiter caps render/
//       present rate for the FPS presets; the VSync preset instead lets the
//       display's vsync pace presentation (see Window::VSyncMode).
// [x]: setFrameRatePreset()/getFrameRatePreset() — runtime-changeable so a
//       future options menu can flip between 30/60/120/VSync without
//       recreating the window.
// [x]: getRenderer() returns engine::Renderer* (NOT SDL_Renderer*) — no SDL
//       types are visible through App's public API.
// [x]: showErrorDialog(title, message) — static helper wrapping
//       SDL_ShowSimpleMessageBox, so callers (e.g. main.cpp) don't need SDL.
// [x]: Private methods processEvents(), update(dt), render(alpha).
// [x]: Private members Window, SceneStack — only forward-declared here.
//       Input is a singleton (Input::instance()), so App does not own it.
// [x]: Default UI font support — App stores a single Font pointer that
//       all scenes can share, loaded once and cleaned up automatically.

class Window;
class Scene;
class Renderer;
class Font;
template <typename T>
class StateMachine;

// User-facing render frame-rate presets.
// Fps30/Fps60/Fps120 use a manual frame limiter in App::run() and disable
// Window vsync, giving an exact, monitor-independent cap (mixing a manual
// limiter with hardware vsync causes double-pacing stutter, so the two are
// kept mutually exclusive). VSync disables the manual limiter and enables
// Window vsync instead, so presentation is paced by the display's refresh
// rate (which is not guaranteed to be 60 — see Window::VSyncMode).
enum class FrameRatePreset
{
    Fps30,
    Fps60,
    Fps120,
    VSync,
};

class App
{
public:
    static constexpr float kDefaultFixedStepSeconds = 1.0f / 60.0f;
    static constexpr FrameRatePreset kDefaultFrameRatePreset = FrameRatePreset::Fps60;

    using SceneFactory = std::function<std::unique_ptr<Scene>()>;
    App(const char *title, int width, int height, SceneFactory initialSceneFactory = {},
        float fixedStepSeconds = kDefaultFixedStepSeconds,
        FrameRatePreset frameRatePreset = kDefaultFrameRatePreset);
    ~App();
    void run();

    // Changes the render frame-rate preset at runtime (e.g. from a future
    // options menu). Updates both the manual limiter target and Window vsync.
    void setFrameRatePreset(FrameRatePreset preset);
    [[nodiscard]] FrameRatePreset getFrameRatePreset() const noexcept { return m_frameRatePreset; }

    // Returns the engine Renderer owned by the window.
    // Valid after construction; null before construction completes.
    [[nodiscard]] static Renderer *getRenderer() noexcept;
    // Returns the Window owning the renderer (for setResizable/setAspectRatio etc.)
    // Valid after construction; null before construction completes.
    [[nodiscard]] static Window *getWindow() noexcept;
    // Valid after construction; null before construction completes.
    [[nodiscard]] static StateMachine<Scene> *getSceneStack() noexcept;
    // Returns the live App instance, e.g. so a future menu scene can reach
    // setFrameRatePreset(). Valid after construction; null before
    // construction completes or after destruction.
    [[nodiscard]] static App *getInstance() noexcept;

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
    float m_fixedStep;
    FrameRatePreset m_frameRatePreset;
    float m_targetFrameSeconds; // 0 == uncapped manual (VSync preset); vsync paces it instead
    Timer m_timer;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<StateMachine<Scene>> m_sceneStack;
};