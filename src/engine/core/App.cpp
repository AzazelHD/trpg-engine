#include "engine/core/App.h"
#include "engine/core/Window.h"
#include "engine/core/Log.h"
#include "engine/renderer/Font.h"
#include "engine/renderer/Color.h"
#include "engine/renderer/Renderer.h"
#include "engine/input/Input.h"
#include "engine/input/KeyCode.h"
#include "engine/scene/Scene.h"
#include "engine/statemachine/StateMachine.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include <memory>
#include <string>

// SDL3/SDL.h is needed here for SDL_Init/SDL_Quit/SDL_ShowSimpleMessageBox
// and the frame-timing calls (SDL_GetTicksNS/SDL_DelayNS) used by the manual
// frame limiter. Frame clear/present go through Window's Renderer, not raw
// SDL_Renderer calls.

namespace
{
    Renderer *s_renderer = nullptr;
    Window *s_window = nullptr;
    StateMachine<Scene> *s_sceneStack = nullptr;
    App *s_app = nullptr;

    // Manual cap target for each preset, in seconds-per-frame. 0 means
    // "no manual cap" (VSync preset — Window's vsync paces it instead).
    float targetFrameSecondsForPreset(FrameRatePreset preset)
    {
        switch (preset)
        {
        case FrameRatePreset::Fps30:
            return 1.0f / 30.0f;
        case FrameRatePreset::Fps60:
            return 1.0f / 60.0f;
        case FrameRatePreset::Fps120:
            return 1.0f / 120.0f;
        case FrameRatePreset::VSync:
            return 0.0f;
        }
        return 1.0f / 60.0f;
    }

    // Manual FPS presets disable vsync (a software limiter + hardware vsync
    // fighting each other causes stutter); the VSync preset enables it and
    // leaves presentation pacing entirely to the display.
    VSyncMode vsyncModeForPreset(FrameRatePreset preset)
    {
        return preset == FrameRatePreset::VSync ? VSyncMode::Enabled : VSyncMode::Disabled;
    }

    const char *frameRatePresetName(FrameRatePreset preset)
    {
        switch (preset)
        {
        case FrameRatePreset::Fps30:
            return "30 FPS";
        case FrameRatePreset::Fps60:
            return "60 FPS";
        case FrameRatePreset::Fps120:
            return "120 FPS";
        case FrameRatePreset::VSync:
            return "VSync";
        }
        return "unknown";
    }

#ifdef _DEBUG
    struct LoggedKey
    {
        KeyCode code;
        const char *name;
    };

    constexpr LoggedKey kLoggedKeys[] = {
        {KeyCode::Up, "Up"},
        {KeyCode::Down, "Down"},
        {KeyCode::Left, "Left"},
        {KeyCode::Right, "Right"},
        {KeyCode::W, "W"},
        {KeyCode::A, "A"},
        {KeyCode::S, "S"},
        {KeyCode::D, "D"},
        {KeyCode::Accept, "Accept"},
        {KeyCode::Back, "Back"},
        {KeyCode::Pause, "Pause"},
        {KeyCode::Advance, "Advance"},
        {KeyCode::CameraPanUp, "CameraPanUp"},
        {KeyCode::CameraPanDown, "CameraPanDown"},
        {KeyCode::CameraPanLeft, "CameraPanLeft"},
        {KeyCode::CameraPanRight, "CameraPanRight"},
        {KeyCode::CameraZoomIn, "CameraZoomIn"},
        {KeyCode::CameraZoomOut, "CameraZoomOut"},
        {KeyCode::CameraReset, "CameraReset"},
        {KeyCode::DebugToggle, "DebugToggle"},
    };
#endif
}

// [x]: Default UI font pointer — set by game code, cleaned up in dtor.
Font *App::s_defaultFont = nullptr;

// [x]: 1. SDL_Init(VIDEO | GAMEPAD). 2. Construct Window (which owns Renderer),
//      with vsync derived from the requested frame-rate preset.
//      3. Construct SceneStack and push initial scene. Input is a singleton
//         accessed via Input::instance() in processEvents() — App owns no
//         Input pointer.
App::App(const char *title, int width, int height, SceneFactory initialSceneFactory,
         float fixedStepSeconds, FrameRatePreset frameRatePreset)
    : m_fixedStep(fixedStepSeconds), m_frameRatePreset(frameRatePreset), m_targetFrameSeconds(targetFrameSecondsForPreset(frameRatePreset))
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        throw std::runtime_error(
            std::string("SDL_Init failed: ") + SDL_GetError());
    }

    if (!TTF_Init())
    {
        SDL_Quit(); // clean up SDL before throwing
        throw std::runtime_error(
            std::string("TTF_Init failed: ") + SDL_GetError());
    }

    m_window = std::make_unique<Window>(title, width, height, vsyncModeForPreset(frameRatePreset));
    s_window = m_window.get();
    s_renderer = &m_window->getRenderer();
    m_sceneStack = std::make_unique<StateMachine<Scene>>();
    s_sceneStack = m_sceneStack.get();
    s_app = this;

    if (initialSceneFactory)
    {
        std::unique_ptr<Scene> initialScene = initialSceneFactory();
        if (!initialScene)
        {
            throw std::runtime_error("Initial scene factory returned a null scene");
        }
        m_sceneStack->push(std::move(initialScene));
    }
}

App::~App()
{
    delete s_defaultFont;
    s_defaultFont = nullptr;

    s_app = nullptr;
    s_sceneStack = nullptr;
    s_renderer = nullptr;
    s_window = nullptr;
    TTF_Quit();
    SDL_Quit();
}

Renderer *App::getRenderer() noexcept
{
    return s_renderer;
}

Window *App::getWindow() noexcept
{
    return s_window;
}

StateMachine<Scene> *App::getSceneStack() noexcept
{
    return s_sceneStack;
}

App *App::getInstance() noexcept
{
    return s_app;
}

// [x]: Returns the default UI font pointer, or nullptr if not set.
Font *App::getDefaultFont() noexcept
{
    return s_defaultFont;
}

// [x]: Sets the default UI font. Takes ownership; previous font deleted.
void App::setDefaultFont(Font *font) noexcept
{
    delete s_defaultFont;
    s_defaultFont = font;
}

// [x]: Native error dialog, no App instance required.
void App::showErrorDialog(const char *title, const char *message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
}

void App::setFrameRatePreset(FrameRatePreset preset)
{
    m_frameRatePreset = preset;
    m_targetFrameSeconds = targetFrameSecondsForPreset(preset);
    if (m_window)
    {
        m_window->setVSync(vsyncModeForPreset(preset));
    }
    LOG_INFO("App", "Frame rate preset changed to %s", frameRatePresetName(preset));
}

// [x]: Fixed-timestep accumulator loop. Frame clear/present go through
//      Window's Renderer instead of raw SDL_Renderer calls. After present(),
//      a manual frame limiter (SDL_GetTicksNS/SDL_DelayNS) caps the loop to
//      m_targetFrameSeconds when set; for the VSync preset
//      (m_targetFrameSeconds == 0) this is skipped and Window's vsync paces
//      presentation instead.
void App::run()
{
    Renderer &renderer = m_window->getRenderer();
    unsigned long long frame = 0;

    LOG_INFO("App", "Entering main loop (fixed step = %.4f, render preset = %s)",
             m_fixedStep, frameRatePresetName(m_frameRatePreset));

    float accumulator = 0.0f;
    m_timer.start();

    while (m_running)
    {
        LOG_SET_FRAME(frame);

        const Uint64 frameStartNs = SDL_GetTicksNS();

        float deltaTime = m_timer.tick();
        accumulator += deltaTime;

        // 1. Collect OS inputs and write them into the snapshot arrays
        processEvents();

        // 2. Fixed timestep loop: For simulation, physics, and gameplay updates only.
        while (accumulator >= m_fixedStep)
        {
            update(m_fixedStep);
            accumulator -= m_fixedStep;
        }

        // 3. Render pass
        float alpha = accumulator / m_fixedStep;

        renderer.clear(Color::black());
        render(alpha);
        renderer.present();

        // 4. Manual frame cap. Skipped for the VSync preset, where Window's
        //    vsync (set in the constructor / setFrameRatePreset) already
        //    paces the present() call above.
        if (m_targetFrameSeconds > 0.0f)
        {
            const Uint64 elapsedNs = SDL_GetTicksNS() - frameStartNs;
            const Uint64 targetNs = static_cast<Uint64>(m_targetFrameSeconds * 1'000'000'000.0f);
            if (elapsedNs < targetNs)
            {
                SDL_DelayNS(targetNs - elapsedNs);
            }
        }

        ++frame;
    }
}

void App::processEvents()
{
    Input &input = Input::instance();

    // 1. Poll OS events first. This is the single frame-input entry point —
    //    pollEvents() refreshes the current/previous key-state snapshots.
    if (!input.pollEvents())
    {
        m_running = false;
        return;
    }

    // 2. Logging and System checks:
    // Perform these BEFORE we pass input to the SceneStack.

#ifdef _DEBUG
    for (const auto &key : kLoggedKeys)
    {
        if (input.isKeyPressed(key.code))
        {
            LOG_INFO("Input", "Key pressed: %s (state=%s)", key.name,
                     m_sceneStack ? m_sceneStack->currentStateDebugName() : "<no-scene-stack>");
        }
    }
#endif

    // QUIT GAME
    // if (input.isKeyPressed(KeyCode::Back))
    // {
    //     LOG_INFO("Input", "Back pressed (Escape) -> exiting app");
    //     m_running = false;
    // }

    // 3. LAST: Route input to the active scene.
    // Regression guard: this single call must remain the only path from
    // the main loop to the scene’s handleInput() (and ultimately to
    // MenuPanel::handleInput()). Bypassing it breaks all menu navigation.
    if (m_sceneStack)
    {
        m_sceneStack->handleInput();
    }
}

void App::update(float dt)
{
    if (m_sceneStack)
        m_sceneStack->update(dt);
}

void App::render(float alpha)
{
    if (m_sceneStack)
        m_sceneStack->render(alpha);
}