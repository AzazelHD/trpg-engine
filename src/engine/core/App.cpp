#include "engine/core/App.h"
#include <SDL3/SDL.h>

// [x]: Implement App::App(title, w, h)
//   1. Call SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD).
//   2. Construct m_window (title, w, h).
//   3. Construct m_input.
//   4. Construct your scene stack, push the initial scene (BootScene or whatever starts things).

// [x]: Implement App::run()
//   This is the heart of the engine. Use the accumulator / fixed-timestep pattern:
//
//   m_timer.start();
//   while (!quit)
//   {
//       float dt = m_timer.tick();
//       accumulator += dt;
//
//       quit = !m_input.pollEvents();   // false if window was closed
//
//       while (accumulator >= FIXED_STEP)   // FIXED_STEP = 1.0f / 60.0f
//       {
//           m_sceneStack.update(FIXED_STEP);
//           accumulator -= FIXED_STEP;
//       }
//
//       float alpha = accumulator / FIXED_STEP;  // interpolation factor for rendering
//       SDL_RenderClear(renderer);
//       m_sceneStack.render();   // scenes can use alpha for smooth motion
//       SDL_RenderPresent(renderer);
//   }
//
//   SDL_Quit() at the end.

#include "engine/core/Window.h"
#include "engine/core/Log.h"
#include "engine/input/Input.h"
#include "engine/input/KeyCode.h"
#include "engine/scene/Scene.h"
#include "engine/statemachine/StateMachine.h"

#include <SDL3/SDL.h>
#include <memory>
#include <stdexcept>
#include <string>

namespace
{
    constexpr float FIXED_STEP = 1.0f / 60.0f;
    SDL_Renderer *s_renderer = nullptr;
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

App::App(const char *title, int width, int height, SceneFactory initialSceneFactory)
    : m_fixedStep(FIXED_STEP)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        throw std::runtime_error(
            std::string("SDL_Init failed: ") + SDL_GetError());
    }

    m_window = std::make_unique<Window>(title, width, height);
    s_renderer = m_window->getSDLRenderer();
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
    SDL_Quit();
}

SDL_Renderer *App::getRenderer() noexcept
{
    return s_renderer;
}

StateMachine<Scene> *App::getSceneStack() noexcept
{
    return s_sceneStack;
}

void App::run()
{
    SDL_Renderer *renderer = m_window->getSDLRenderer();
    unsigned long long frame = 0;

    LOG_INFO("App", "Entering main loop (fixed step = %.4f)", m_fixedStep);

    float accumulator = 0.0f;
    m_timer.start();

    while (m_running)
    {
        LOG_SET_FRAME(frame);

        float deltaTime = m_timer.tick();

        accumulator += deltaTime;

        processEvents();

        bool didUpdate = false;

        while (accumulator >= m_fixedStep)
        {
            update(m_fixedStep);
            accumulator -= m_fixedStep;
            didUpdate = true;
        }

        if (didUpdate)
        {
            m_input->clearPressedLatches();
        }

        float alpha = accumulator / m_fixedStep;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render(alpha);

        SDL_RenderPresent(renderer);
        ++frame;
    }
}

void App::processEvents()
{
    if (!m_input->pollEvents())
    {
        LOG_INFO("Input", "Window close requested (SDL_QUIT)");
        m_running = false;
        return;
    }

    static bool s_wasDown[sizeof(kLoggedKeys) / sizeof(kLoggedKeys[0])] = {};

    for (std::size_t i = 0; i < (sizeof(kLoggedKeys) / sizeof(kLoggedKeys[0])); ++i)
    {
        const LoggedKey &key = kLoggedKeys[i];
        const bool down = m_input->isKeyDown(key.code);

        if (down && !s_wasDown[i])
        {
            LOG_INFO(
                "Input",
                "Key pressed: %s (state=%s)",
                key.name,
                m_sceneStack ? m_sceneStack->currentStateDebugName() : "<no-scene-stack>");
        }

        s_wasDown[i] = down;
    }

    if (m_input->isKeyPressed(KeyCode::Back))
    {
        LOG_INFO("Input", "Back pressed (Escape) -> exiting app");
        m_running = false;
        return;
    }
}

void App::update(float dt)
{
    m_sceneStack->update(dt);
}

void App::render(float alpha)
{
    m_sceneStack->render(alpha);
}