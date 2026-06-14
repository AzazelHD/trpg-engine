#pragma once

#include "engine/input/KeyCode.h"
#include "engine/math/Vec2.h"
#include <SDL3/SDL.h>
#include <cstdint>

// Input polls SDL events and exposes a clean query API.
// The game never calls SDL_PollEvent — only Input does.
//
// [x]: Declare the class with:
//   - pollEvents(): call once per frame at the top of the loop. Reads all SDL events.
//                  Returns false if the user closed the window (quit event).
//   - isKeyDown(KeyCode key): true every frame the key is held.
//   - isKeyPressed(KeyCode key): true only on the first frame the key goes down.
//   - isKeyReleased(KeyCode key): true only on the frame the key goes up.
//   - getMousePosition(): current mouse position in window/screen pixels.
//   - isMouseButtonDown(button): true while the requested SDL mouse button is held.
//
// Implementation hint: SDL gives you a snapshot via SDL_GetKeyboardState().
// To detect "pressed this frame", compare current state vs. previous frame state.
// Store two bool arrays: currentKeys[] and previousKeys[], swap after each poll.
//
// [x]: Singleton pattern added:
//   - instance() provides global access to the single Input system.
//   - Avoids passing Input reference through all game systems.
//   - Created lazily on first use (static local instance).

class Input
{
public:
    Input();

    // [x]: Singleton accessor.
    // Returns the single Input instance used by the engine.
    static Input &instance();

    // [x]: Poll SDL events and update internal input state.
    // Must be called once per frame before game update logic.
    bool pollEvents();

    // [x]: Optional event hook.
    // Currently unused; reserved for future extensions:
    // - text input
    // - mouse wheel
    // - controller input
    void handleEvent(const SDL_Event &event);

    // [x]: True while key is held down.
    bool isKeyDown(KeyCode key) const;

    // [x]: True only on first frame key is pressed.
    bool isKeyPressed(KeyCode key) const;

    // [x]: True only on first frame key is released.
    bool isKeyReleased(KeyCode key) const;

    // [x]: Returns current mouse position in window space.
    Vec2f getMousePosition() const;

    // [x]: True while mouse button is held.
    bool isMouseButtonDown(int button) const;

    // Clears one-shot key press latches after at least one game update tick.
    void clearPressedLatches();

private:
    // [x]: Current frame keyboard state (SDL scancode indexed).
    bool m_currentKeys[SDL_SCANCODE_COUNT] = {};

    // [x]: Previous frame keyboard state (used for edge detection).
    bool m_previousKeys[SDL_SCANCODE_COUNT] = {};

    // Key down events latched until consumed after an update tick.
    bool m_pressedLatches[SDL_SCANCODE_COUNT] = {};

    // [x]: Current mouse position in screen/window coordinates.
    Vec2f m_mousePosition = {0.0f, 0.0f};

    // [x]: Bitmask of current mouse button states.
    uint32_t m_mouseButtons = 0;

    // [x]: Convert engine KeyCode → SDL scancode.
    // Used internally by all key query functions.
    static SDL_Scancode keyCodeToScancode(KeyCode key)
    {
        switch (key)
        {
        // Navigation
        case KeyCode::Up:
            return SDL_SCANCODE_UP;
        case KeyCode::Down:
            return SDL_SCANCODE_DOWN;
        case KeyCode::Left:
            return SDL_SCANCODE_LEFT;
        case KeyCode::Right:
            return SDL_SCANCODE_RIGHT;

        // WASD
        case KeyCode::W:
            return SDL_SCANCODE_W;
        case KeyCode::A:
            return SDL_SCANCODE_A;
        case KeyCode::S:
            return SDL_SCANCODE_S;
        case KeyCode::D:
            return SDL_SCANCODE_D;

        // Actions
        case KeyCode::Accept:
            return SDL_SCANCODE_RETURN;
        case KeyCode::Back:
            return SDL_SCANCODE_ESCAPE;
        case KeyCode::Pause:
            return SDL_SCANCODE_P;
        case KeyCode::Advance:
            return SDL_SCANCODE_SPACE;

        // Camera pan
        case KeyCode::CameraPanUp:
            return SDL_SCANCODE_KP_8;
        case KeyCode::CameraPanDown:
            return SDL_SCANCODE_KP_2;
        case KeyCode::CameraPanLeft:
            return SDL_SCANCODE_KP_4;
        case KeyCode::CameraPanRight:
            return SDL_SCANCODE_KP_6;

        // Camera zoom
        case KeyCode::CameraZoomIn:
            return SDL_SCANCODE_KP_PLUS;
        case KeyCode::CameraZoomOut:
            return SDL_SCANCODE_KP_MINUS;

        // Camera reset
        case KeyCode::CameraReset:
            return SDL_SCANCODE_KP_5;

        // Debug
        case KeyCode::DebugToggle:
            return SDL_SCANCODE_F1;

        default:
            return SDL_SCANCODE_UNKNOWN;
        }
    }
};