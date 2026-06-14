#include "engine/input/Input.h"
#include <SDL3/SDL.h>
#include <cstring>

Input::Input()
{
    // Ensure key state arrays are zeroed
    std::memset(m_currentKeys, 0, sizeof(m_currentKeys));
    std::memset(m_previousKeys, 0, sizeof(m_previousKeys));
    std::memset(m_pressedLatches, 0, sizeof(m_pressedLatches));
}

// [x]: Singleton access point.
//   This allows global-style access without passing Input everywhere.
Input &Input::instance()
{
    static Input s_instance;
    return s_instance;
}

// [x]: Implement pollEvents().
//   1. Copy m_currentKeys into m_previousKeys.
//   2. Call SDL_PollEvent() loop.
//   3. Update keyboard state via SDL_GetKeyboardState().
//   4. Update mouse position and buttons.
//   5. Return false if SDL_QUIT is received.

bool Input::pollEvents()
{
    std::memcpy(m_previousKeys, m_currentKeys, sizeof(m_currentKeys));
    bool quitRequested = false;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        handleEvent(event);

        if (event.type == SDL_EVENT_QUIT)
            quitRequested = true;
    }

    SDL_PumpEvents();

    const bool *state = SDL_GetKeyboardState(nullptr);
    std::memcpy(m_currentKeys, state, sizeof(m_currentKeys));

    float mouseX = 0.f, mouseY = 0.f;
    m_mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
    m_mousePosition = {mouseX, mouseY};

    return !quitRequested;
}

// [x]: Implement isKeyDown(KeyCode key).
//   Translate KeyCode → SDL_Scancode (a switch or array lookup).
//   Return m_currentKeys[scancode] != 0.

bool Input::isKeyDown(KeyCode key) const
{
    SDL_Scancode sc = keyCodeToScancode(key);
    return sc != SDL_SCANCODE_UNKNOWN && m_currentKeys[sc];
}

// [x]: Implement isKeyPressed(KeyCode key).
//   Return m_currentKeys[scancode] && !m_previousKeys[scancode].

bool Input::isKeyPressed(KeyCode key) const
{
    SDL_Scancode sc = keyCodeToScancode(key);
    return sc != SDL_SCANCODE_UNKNOWN &&
           ((m_currentKeys[sc] && !m_previousKeys[sc]) || m_pressedLatches[sc]);
}

// [x]: Implement isKeyReleased(KeyCode key).
//   Return !m_currentKeys[scancode] && m_previousKeys[scancode].

bool Input::isKeyReleased(KeyCode key) const
{
    SDL_Scancode sc = keyCodeToScancode(key);
    return sc != SDL_SCANCODE_UNKNOWN &&
           !m_currentKeys[sc] &&
           m_previousKeys[sc];
}

Vec2f Input::getMousePosition() const
{
    return m_mousePosition;
}

bool Input::isMouseButtonDown(int button) const
{
    if (button <= 0)
        return false;

    return (m_mouseButtons & SDL_BUTTON_MASK(button)) != 0;
}

void Input::clearPressedLatches()
{
    std::memset(m_pressedLatches, 0, sizeof(m_pressedLatches));
}

// [x]: Handle raw SDL events if needed in future.
//   Currently unused, but required for future features like:
//   - text input
//   - mouse wheel
//   - controller events
void Input::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
    {
        const SDL_Scancode sc = event.key.scancode;
        if (sc >= 0 && sc < SDL_SCANCODE_COUNT)
        {
            m_pressedLatches[sc] = true;
        }
    }
}