#include "engine/input/Input.h"
#include <SDL3/SDL.h>
#include <cstring>

Input::Input()
{
    // Ensure key state arrays are zeroed
    std::memset(m_currentKeys, 0, sizeof(m_currentKeys));
    std::memset(m_previousKeys, 0, sizeof(m_previousKeys));
}

// [x]: Singleton access point.
Input &Input::instance()
{
    static Input s_instance;
    return s_instance;
}

// [x]: Poll SDL events and update internal input state.
bool Input::pollEvents()
{
    std::memcpy(m_previousKeys, m_currentKeys, sizeof(m_currentKeys));

    bool quitRequested = false;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            quitRequested = true;
    }

    int len = 0;
    const bool *state = SDL_GetKeyboardState(&len);

    int count = (len < MAX_KEYS) ? len : MAX_KEYS;

    for (int i = 0; i < count; i++)
        m_currentKeys[i] = state[i];

    float mouseX = 0.f, mouseY = 0.f;
    m_mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
    m_mousePosition = {mouseX, mouseY};

    return !quitRequested;
}

// [x]: True while key is held down.
bool Input::isKeyDown(KeyCode key) const
{
    int sc = keyCodeToScancode(key);
    return sc >= 0 && sc < MAX_KEYS && m_currentKeys[sc];
}

// [x]: True only on first frame key is pressed.
bool Input::isKeyPressed(KeyCode key) const
{
    int sc = keyCodeToScancode(key);
    return sc >= 0 && sc < MAX_KEYS &&
           m_currentKeys[sc] && !m_previousKeys[sc];
}

// [x]: True only on first frame key is released.
bool Input::isKeyReleased(KeyCode key) const
{
    int sc = keyCodeToScancode(key);
    return sc >= 0 && sc < MAX_KEYS &&
           !m_currentKeys[sc] && m_previousKeys[sc];
}

Vec2f Input::getMousePosition() const
{
    return m_mousePosition;
}

bool Input::isMouseButtonDown(int button) const
{
    if (button <= 0)
        return false;

    Uint32 mask = 0;

    switch (button)
    {
    case 1:
        mask = SDL_BUTTON_LMASK;
        break;
    case 2:
        mask = SDL_BUTTON_MMASK;
        break;
    case 3:
        mask = SDL_BUTTON_RMASK;
        break;
    default:
        return false;
    }

    return (m_mouseButtons & mask) != 0;
}

// [x]: Convert engine KeyCode → SDL scancode.
int Input::keyCodeToScancode(KeyCode key)
{
    switch (key)
    {
    case KeyCode::Up:
        return SDL_SCANCODE_UP;
    case KeyCode::Down:
        return SDL_SCANCODE_DOWN;
    case KeyCode::Left:
        return SDL_SCANCODE_LEFT;
    case KeyCode::Right:
        return SDL_SCANCODE_RIGHT;

    case KeyCode::W:
        return SDL_SCANCODE_W;
    case KeyCode::A:
        return SDL_SCANCODE_A;
    case KeyCode::S:
        return SDL_SCANCODE_S;
    case KeyCode::D:
        return SDL_SCANCODE_D;

    case KeyCode::Accept:
        return SDL_SCANCODE_RETURN;
    case KeyCode::Back:
        return SDL_SCANCODE_ESCAPE;
    case KeyCode::Pause:
        return SDL_SCANCODE_P;
    case KeyCode::Advance:
        return SDL_SCANCODE_SPACE;

    case KeyCode::CameraPanUp:
        return SDL_SCANCODE_KP_8;
    case KeyCode::CameraPanDown:
        return SDL_SCANCODE_KP_2;
    case KeyCode::CameraPanLeft:
        return SDL_SCANCODE_KP_4;
    case KeyCode::CameraPanRight:
        return SDL_SCANCODE_KP_6;

    case KeyCode::CameraZoomIn:
        return SDL_SCANCODE_KP_PLUS;
    case KeyCode::CameraZoomOut:
        return SDL_SCANCODE_KP_MINUS;

    case KeyCode::CameraReset:
        return SDL_SCANCODE_KP_5;

    case KeyCode::DebugToggle:
        return SDL_SCANCODE_F1;

    default:
        return SDL_SCANCODE_UNKNOWN;
    }
}