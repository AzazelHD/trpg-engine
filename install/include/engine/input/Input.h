#pragma once

#include "engine/math/Vec2.h"
#include "engine/input/KeyCode.h"
#include <cstdint>

// =============================================================================
// Input — Singleton input manager using SDL events.
// =============================================================================
//
// Responsibilities:
//   - Poll SDL events each frame.
//   - Maintain current key state (held down).
//   - Accumulate key press, release, and repeat events per frame.
//   - Expose clean query API: isKeyDown, isKeyPressed, isKeyReleased.
//
// Design notes:
//   - Events are accumulated during pollEvents() into dedicated arrays.
//   - isKeyPressed(KeyCode, bool allowRepeat) reads from accumulation arrays.
//   - allowRepeat = true enables OS-level key repeat (useful for menus).
//   - No reliance on SDL_GetKeyboardState; event-driven for reliability.
//
// Singleton access via Input::instance().
// =============================================================================

class Input
{
public:
    // ── Singleton ──────────────────────────────────────────────────────────
    static Input &instance();

    // Disallow copying and moving to strictly enforce the Singleton contract.
    Input(const Input &) = delete;
    Input &operator=(const Input &) = delete;
    Input(Input &&) = delete;
    Input &operator=(Input &&) = delete;

    // ── Frame update ────────────────────────────────────────────────────────
    // Poll all SDL events and update internal state.
    // Returns false if the user closed the window (SDL_QUIT).
    bool pollEvents();

    // ── Key state queries ──────────────────────────────────────────────────

    // True while the key is physically held down.
    bool isKeyDown(KeyCode key) const;

    // True only on the first frame the key is pressed (or repeated if allowRepeat).
    // - allowRepeat = false: only non-repeat presses (default, for actions).
    // - allowRepeat = true:  includes OS key repeat events (for menu navigation).
    bool isKeyPressed(KeyCode key, bool allowRepeat = false) const;

    // Mark a key as consumed so that isKeyPressed() returns false
    // for the remainder of this frame. Use after a handler has fully
    // processed a press and no later handler in the same frame
    // should see it.
    void consumeKey(KeyCode key);

    // True only on the frame the key is released.
    bool isKeyReleased(KeyCode key) const;

    // ── Mouse queries ──────────────────────────────────────────────────────

    // Current mouse position in window coordinates.
    Vec2f getMousePosition() const;

    // True while a mouse button is held (button: 1=left, 2=middle, 3=right).
    bool isMouseButtonDown(int button) const;

private:
    // ── Constructor (private for singleton) ──────────────────────────────
    // Note: Memory is zeroed out automatically via in-class initializers.
    Input();

    // ── Constants ──────────────────────────────────────────────────────────
    static constexpr int MAX_KEYS = 512; // covers all SDL scancodes.

    // ── Key state arrays ──────────────────────────────────────────────────

    // Current and previous frame key states (for hold/release detection).
    bool m_currentKeys[MAX_KEYS] = {};
    bool m_previousKeys[MAX_KEYS] = {};

    // Event accumulation arrays (reset each frame in pollEvents()).
    bool m_pressedThisFrame[MAX_KEYS] = {};  // non-repeat presses
    bool m_releasedThisFrame[MAX_KEYS] = {}; // releases
    bool m_repeatedThisFrame[MAX_KEYS] = {}; // OS key repeats
    // If true, isKeyPressed() returns false for the rest of this frame.
    // Cleared each frame in pollEvents().
    bool m_consumedThisFrame[MAX_KEYS] = {};

    // ── Mouse state ───────────────────────────────────────────────────────
    Vec2f m_mousePosition = {0.0f, 0.0f};
    uint32_t m_mouseButtons = 0; // bitmask of currently held buttons

    // ── Internal helpers ──────────────────────────────────────────────────
    static int keyCodeToScancode(KeyCode key);
};