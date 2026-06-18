#include "engine/core/App.h"
#include "engine/core/Window.h"
#include "engine/core/Log.h"
#include "engine/renderer/Renderer.h"
#include "engine/renderer/Color.h"
#include "engine/input/Input.h"
#include "engine/input/KeyCode.h"
#include "engine/scene/Scene.h"
#include "engine/statemachine/StateMachine.h"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <memory>
#include <string>

// SDL3/SDL.h is only needed here for SDL_Init/SDL_Quit/SDL_ShowSimpleMessageBox.
// Frame rendering goes through Window's Renderer, not raw SDL calls.

namespace
{
    constexpr float FIXED_STEP = 1.0f / 60.0f;
    Renderer *s_renderer = nullptr;
    Window *s_window = nullptr;
    StateMachine<Scene> *s_sceneStack = nullptr;

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
}

// [x]: 1. SDL_Init(VIDEO | GAMEPAD). 2. Construct Window (which owns Renderer).
//      3. Construct Input. 4. Construct SceneStack and push initial scene.
App::App(const char *title, int width, int height, SceneFactory initialSceneFactory)
    : m_fixedStep(FIXED_STEP)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        throw std::runtime_error(
            std::string("SDL_Init failed: ") + SDL_GetError());
    }

    m_window = std::make_unique<Window>(title, width, height);
    s_window = m_window.get();
    s_renderer = &m_window->getRenderer();
    m_input = &Input::instance();
    m_sceneStack = std::make_unique<StateMachine<Scene>>();
    s_sceneStack = m_sceneStack.get();

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
    s_sceneStack = nullptr;
    s_renderer = nullptr;
    s_window = nullptr;
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

// [x]: Native error dialog, no App instance required.
void App::showErrorDialog(const char *title, const char *message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
}

// [x]: Fixed-timestep accumulator loop. Frame clear/present go through
//      Window's Renderer instead of raw SDL_Renderer calls.
void App::run()
{
    Renderer &renderer = m_window->getRenderer();
    unsigned long long frame = 0;

    LOG_INFO("App", "Entering main loop (fixed step = %.4f)", m_fixedStep);

    float accumulator = 0.0f;
    m_timer.start();

    while (m_running)
    {
        LOG_SET_FRAME(frame);

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

        ++frame;
    }
}

void App::processEvents()
{
    // 1. Poll OS events first
    if (!m_input->pollEvents())
    {
        m_running = false;
        return;
    }

    // 2. Logging and System checks:
    // Perform these BEFORE we pass input to the SceneStack.
    Input &input = Input::instance();

    for (const auto &key : kLoggedKeys)
    {
        if (input.isKeyPressed(key.code))
        {
            LOG_INFO("Input", "Key pressed: %s (state=%s)", key.name,
                     m_sceneStack ? m_sceneStack->currentStateDebugName() : "<no-scene-stack>");
        }
    }

    if (input.isKeyPressed(KeyCode::Back))
    {
        LOG_INFO("Input", "Back pressed (Escape) -> exiting app");
        m_running = false;
    }

    // 3. LAST: Route input to the active scene.
    // By doing this last, we ensure the logs accurately reflect the input
    // that the game states are about to process.
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