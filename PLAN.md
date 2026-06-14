# TRPG Engine — Development Plan

## Mission

Build and maintain a reusable 2D engine as a static library (`engine.lib`) with zero gameplay knowledge.
The game layer must consume only public engine headers and never depend on SDL internals directly.

---

## Scope for This Repository

This document tracks only work inside `engine/`:

- Public API under `include/engine/`
- Private implementation under `src/engine/`
- CMake target and packaging concerns specific to the engine library

All gameplay systems (states, battle logic, AI, data, content) are tracked in `game_1/PLAN.md`.

---

## Project Structure (Engine)

```
engine/
├── CMakeLists.txt
├── include/engine/
│   ├── core/          (App, Window, Timer, Log)
│   ├── input/         (Input, KeyCode, GamepadCode)
│   ├── math/          (Vec2, Rect, MathUtils)
│   ├── renderer/      (Camera, Texture, SpriteBatch, TileLayer)
│   ├── scene/         (Scene, SceneStack)
│   ├── statemachine/  (IState, StateMachine compatibility)
│   └── ui/            (Button, MenuPanel, Slider, TextLabel)
└── src/engine/
    ├── core/
    ├── input/
    ├── renderer/
    └── ui/
```

---

## Phases

### E1 — Build & Packaging Contract

1. Keep `engine/CMakeLists.txt` producing a `STATIC` target (`engine`)
2. Export public include directory cleanly (`include/`)
3. Keep dependency surface minimal and explicit (SDL2, SDL2_image, SDL2_mixer, SDL2_ttf only where needed)
4. Verify both Debug and Release compile as library consumers expect

### E2 — Core Runtime

5. `App`: deterministic main loop wrapper with `update(dt)` + `render()`
6. `Timer`: fixed timestep based on `QueryPerformanceCounter`
7. `Window`: SDL window/renderer lifecycle and resize/fullscreen handling
8. `Log`: tagged logging macros, stripped in Release
9. `Input`: event polling and key/button state queries
10. `math`: `Vec2`, `Rect`, interpolation and utility helpers

### E3 — Rendering Layer

11. `Texture`: image load/unload and metadata
12. `SpriteBatch`: frame-level draw queue + flush
13. `Camera`: world/screen transforms, follow, clamping
14. `TileLayer`: tilemap rendering using camera transform
15. Debug rendering hooks guarded out of Release

### E4 — Scene Flow & UI Primitives

16. `Scene` lifecycle contract for game-owned scenes
17. `SceneStack<T>` / `StateMachine<T>` stack semantics
18. Keyboard-first UI primitives (`Button`, `MenuPanel`, `Slider`, `TextLabel`)

### E5 — Content Pipeline and Hot Reload (Dev)

19. Add a generic file watcher service for dev builds (polling fallback where native watchers are unavailable)
20. Add a hot-reload dispatcher that broadcasts changed asset paths to interested systems
21. Define engine-owned tile map runtime data structures (layers, tilesets, properties) with no gameplay concepts
22. Implement a Tiled JSON loader that converts Tiled data into engine tile map runtime structures
23. Integrate tile map reload into rendering path so changed maps and tilesets refresh without restarting
24. Add reload safety rules (debounce writes, partial failure rollback, clear logging)

---

## Verification Checkpoints

| After phase | What should be true                                            |
| ----------- | -------------------------------------------------------------- |
| E1          | `engine` builds as static lib in Debug + Release               |
| E2          | App loop, timing, input, and logging work reliably             |
| E3          | Texture/tile rendering and camera transforms are stable        |
| E4          | Scene transitions and UI primitives work without leaks/crashes |
| E5          | Saving a Tiled map in dev updates rendering without restart    |

---

## Non-Negotiable Rules

- Engine code must not include game headers or gameplay concepts.
- Public headers in `include/engine/` are the only supported consumer interface.
- Debug helpers must compile out in Release where planned.
- Keep APIs general enough for reuse in future projects.
- Concrete dialog widgets and game flow scenes belong in the game layer, not in `engine/`.

---

## Toolchain

- IDE: Visual Studio 2022 Community
- Compiler: MSVC x64
- Build system: CMake
- Package manager: vcpkg
- Libraries: SDL2 family + nlohmann_json (game-facing data stays outside engine APIs)
