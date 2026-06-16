#pragma once

#include "engine/input/KeyCode.h"
#include "engine/math/Vec2.h"
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
    // [x]: Singleton accessor.
    static Input &instance();

    // [x]: Poll SDL events and update internal input state.
    bool pollEvents();

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

private:
    // [x]: Private constructor for singleton pattern.
    Input();

    // [x]: Maximum number of keys to track (engine-defined, not SDL-dependent).
    static constexpr int MAX_KEYS = 512;

    // [x]: Current frame keyboard state (scancode indexed).
    bool m_currentKeys[MAX_KEYS] = {};

    // [x]: Previous frame keyboard state (used for edge detection).
    bool m_previousKeys[MAX_KEYS] = {};

    // [x]: Current mouse position in window coordinates.
    Vec2f m_mousePosition = {0.0f, 0.0f};

    // [x]: Bitmask of current mouse button states.
    uint32_t m_mouseButtons = 0;

    // [x]: Convert engine KeyCode → SDL scancode.
    static int keyCodeToScancode(KeyCode key);
};