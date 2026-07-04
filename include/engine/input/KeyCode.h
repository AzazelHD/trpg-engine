#pragma once

// KeyCode maps our engine's key identifiers to SDL scancodes.
// By going through this enum the game never depends on SDL headers.
//
// [x]: Define an enum class KeyCode with the keys your game will use.
// Map each value to the corresponding SDL_SCANCODE_* in Input.cpp.
//
// Suggested layout:
//
//   Cursor / menu navigation (arrow keys or WASD):
//     Up, Down, Left, Right
//
//   Actions:
//     Confirm   (Enter or Z)
//     Cancel    (Escape or X)
//     Pause     (P or Start)
//
//   Camera pan (separate from cursor so you can move the view without moving the cursor):
//     CameraPanUp    (I or numpad 8)
//     CameraPanDown  (K or numpad 2)
//     CameraPanLeft  (J or numpad 4)
//     CameraPanRight (L or numpad 6)
//     CameraZoomIn   (+ or numpad +)
//     CameraZoomOut  (- or numpad -)
//     CameraReset    (Home — snaps camera back to the active unit)
//
//   Dialog advance:
//     DialogAdvance  (Enter or Space — advances to the next line in a dialog box)
//
//   Debug (only active in Debug builds):
//     DebugToggle    (F1 — toggles grid overlay and coordinate display)
//
// Tip: binding two physical keys to the same KeyCode (e.g. Enter AND Z → Confirm)
// requires a small lookup table in Input.cpp that maps multiple scancodes to one KeyCode.
//
// KeyCode maps neutral engine key identifiers to SDL scancodes.
// By using this enum, game code never depends on SDL headers directly.
//
// Rename note (v0.1.0):
//   Confirm       -> Accept
//   Cancel        -> Back
//   DialogAdvance -> Advance
//
// These are the canonical names. The old names have been removed entirely.

enum class KeyCode
{
    // Navigation
    Up,
    Down,
    Left,
    Right,
    W,
    A,
    S,
    D,
    Q,
    E,

    // Actions
    Accept,  // was Confirm (Enter or Z)
    Back,    // was Cancel (Escape or X)
    Pause,   // P
    Advance, // was DialogAdvance (Space or Enter)

    // Camera controls
    CameraPanUp,
    CameraPanDown,
    CameraPanLeft,
    CameraPanRight,
    CameraZoomIn,
    CameraZoomOut,
    CameraReset,

    // Debug
    DebugToggle,

    // Count (for array sizing)
    Count
};