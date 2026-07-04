#include "engine/input/Input.h"
#include "engine/core/Log.h"

#include <SDL3/SDL.h>
#include <cstring>

Input::Input()
{
    // With the m_currentKeys[MAX_KEYS] = {} initializers in the .h file,
    // the constructor already zeroes out the memory automatically. We leave
    // the constructor body open here in case you need to initialize anything else later.
}

Input &Input::instance()
{
    static Input s_instance;
    return s_instance;
}

bool Input::pollEvents()
{
    // 1. Snapshot previous frame state and clear frame accumulator arrays
    std::memcpy(m_previousKeys, m_currentKeys, sizeof(m_currentKeys));
    std::memset(m_pressedThisFrame, 0, sizeof(m_pressedThisFrame));
    std::memset(m_releasedThisFrame, 0, sizeof(m_releasedThisFrame));
    std::memset(m_repeatedThisFrame, 0, sizeof(m_repeatedThisFrame));
    std::memset(m_consumedThisFrame, 0, sizeof(m_consumedThisFrame));

    bool quitRequested = false;
    SDL_Event event;

    // 2. Process the SDL event queue atomically
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quitRequested = true;
            break;

        case SDL_EVENT_KEY_DOWN:
        {
            int sc = event.key.scancode;
            LOG_INFO("Input", "Key down scancode = %d", sc);
            if (sc >= 0 && sc < MAX_KEYS)
            {
                m_currentKeys[sc] = true;

                if (event.key.repeat)
                    m_repeatedThisFrame[sc] = true;
                else
                    m_pressedThisFrame[sc] = true;
            }
        }
        break;

        case SDL_EVENT_KEY_UP:
        {
            int sc = event.key.scancode;
            if (sc >= 0 && sc < MAX_KEYS)
            {
                m_currentKeys[sc] = false;
                m_releasedThisFrame[sc] = true;
            }
        }
        break;

        case SDL_EVENT_MOUSE_MOTION:
            m_mousePosition = {event.motion.x, event.motion.y};
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            m_mouseButtons |= (1 << (event.button.button - 1));
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            m_mouseButtons &= ~(1 << (event.button.button - 1));
            break;
        }
    }

    return !quitRequested;
}

// ── Key Queries ──────────────────────────────────────────────────────────────

bool Input::isKeyDown(KeyCode key) const
{
    int sc = keyCodeToScancode(key);
    return sc >= 0 && sc < MAX_KEYS && m_currentKeys[sc];
}

bool Input::isKeyPressed(KeyCode key, bool allowRepeat) const
{
    int sc = keyCodeToScancode(key);

    if (sc < 0 || sc >= MAX_KEYS)
        return false;

    if (m_consumedThisFrame[sc])
        return false;

    if (allowRepeat)
        return m_pressedThisFrame[sc] || m_repeatedThisFrame[sc];

    return m_pressedThisFrame[sc];
}

void Input::consumeKey(KeyCode key)
{
    int sc = keyCodeToScancode(key);
    if (sc >= 0 && sc < MAX_KEYS)
        m_consumedThisFrame[sc] = true;
}

bool Input::isKeyReleased(KeyCode key) const
{
    int sc = keyCodeToScancode(key);
    return sc >= 0 && sc < MAX_KEYS && m_releasedThisFrame[sc];
}

// ── Mouse Queries ────────────────────────────────────────────────────────────

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

// ── Internal Helpers ─────────────────────────────────────────────────────────

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
    case KeyCode::Q:
        return SDL_SCANCODE_Q;
    case KeyCode::E:
        return SDL_SCANCODE_E;

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
        return SDL_SCANCODE_F12;

    default:
        return SDL_SCANCODE_UNKNOWN;
    }
}