# AGENTS.md — `engine` (TRPG Reusable Engine Library)

Instructions for AI coding agents (opencode / Copilot / similar) working on
the `engine` project. This file is the **single source of truth** for agent
behavior on this repo. Update it when conventions change.

`engine` is a reusable static library: runtime loop, rendering, input, scene
flow, UI primitives, and data tooling. It must stay **gameplay-agnostic** —
game rules, scenes, and content belong to `game_1`.

---

## 1. Project Layout

```
engine/
├── ARCHITECTURE.md          # authoritative module/ownership map
├── PLAN.md                  # mission, status, done-vs-pending
├── TODO.md                  # development checklist (E1..E9)
├── AGENTS.md                # this file
├── CMakeLists.txt           # SINGLE source of truth for engine sources
├── tools/
│   ├── build_debug.bat      # vcpkg + cmake --preset engine + build Debug
│   ├── build_release.bat
│   └── check_public_headers.py  # self-containment check for include/engine/*
├── include/engine/          # PUBLIC API (the supported integration surface)
├── src/engine/              # private implementation (may change without notice)
└── install/include/engine/  # generated consumer copy (DEV flow uses build tree;
                             # sync manually when public headers change)
```

Public API lives under `include/engine/` only. `game_1` consumes it directly
via the `../engine/include` include path and links the engine build tree.

---

## 2. Hard Architectural Rules

These are non-negotiable. Violating them breaks the engine/game boundary.

1. **Gameplay-agnostic boundary**
   - No game headers, scenes, or gameplay concepts may appear in engine
     `include/` or `src/`.
   - No dependencies point from `engine/` into `game_1/`.
   - SDL types are never exposed to consumers in public headers.

2. **Public API surface**
   - Only headers under `include/engine/` are supported integration points.
   - Implementation details in `src/engine/` are internal and may change.
   - Public header changes require migration notes in TODO/PLAN/ARCH docs.

3. **Explicit source registration**
   - `engine/CMakeLists.txt` is the **only** place that lists engine sources.
     Adding a `.cpp` requires editing it explicitly — no globbing.

4. **Self-contained headers**
   - Every public header must compile alone (verified by
     `tools/check_public_headers.py`). Include what you use; `#pragma once`.

5. **Compose, don't duplicate**
   - New UI/layout helpers build on existing engine primitives
     (`HorizontalLayout`, `VerticalLayout`, `TextWrap`, `Insets`,
     `FocusGroup`) instead of re-implementing them.

---

## 3. Module Ownership (quick reference)

For detailed responsibilities see `ARCHITECTURE.md`. Quick roles:

- `core/` — `App` (fixed-step loop + scene dispatch), `Window`, `Timer`, `Log`.
- `input/` — `Input` (singleton key polling), `KeyCode`/`GamepadCode` vocabulary.
- `math/` — `Vec2`, `Rect`, `MathUtils` (lerp/clamp/distance/iso helpers).
- `renderer/` — `Renderer` (draw/text/passes), `Camera` (tile transforms,
  follow/zoom/clamp), `Texture`/`Font`/`FontManager`, `SpriteBatch`,
  `TileLayer`, `DebugRenderer`.
- `scene/` + `statemachine/` — `Scene` lifecycle; `StateMachine<T>`
  push/pop/replace with deferred mutation.
- `ui/` — widgets (`Button`, `Slider`, `TextLabel`, `MenuPanel`), controls
  (`ButtonControl`, `SliderControl`, `ValueControl`), layout (`FocusGroup`,
  `HorizontalLayout`, `VerticalLayout`, `Insets`), text layout (`TextWrap`
  word-wrap; `TextBlock` measure/anchor/render of stacked wrapped rows).
- `data/` — `TileMapData`, `TiledJsonLoader`, `PropertyId`/`PropertyRegistry`/
  `TileClass` map metadata conventions.
- `assets/` — `FileWatcher`, `HotReloadBus` (polled change detection + bus).
- `effects/` — `ScreenTransition` (fade/wipe scene transitions).
- `animation/` — `Tween<T>` (eased interpolation), `TimedState` (one-shot
  leaf), `Animator` (named-state FSM).

---

## 4. Conventions

- C++20; compile with `/W4 /WX` (zero warnings), Debug + Release.
- `PascalCase` types/functions, `camelCase` params, `m_` members; no comments
  unless asked.
- Headers self-contained, grouped includes (engine, then STL), no `using`.
- `Rect`, `Vec2`, `Color` are the engine geometry/color types; consumers must
  not see SDL types in signatures.
- Extend the module cheat-sheet in the game side only through the engine's
  public API — `game_1/AGENTS.md` §14 mirrors this table and stays in sync.

---

## 5. Build / Verify

```
engine> tools\build_debug.bat     # vcpkg install + CMake configure + Debug build
engine> tools\build_release.bat   # Release build
engine> python tools\check_public_headers.py   # self-containment scan
```

Rules:

- Rebuild `engine` after any header/source change, then rebuild the game.
- `install/include/` is a consumer copy — sync changed public headers there
  when the release/install flow is used (DEV flow reads `../engine/include`).
- Keep both Debug and Release building clean before declaring a feature done.

---

## 6. Documentation

- `ARCHITECTURE.md` — authoritative module/ownership map. Update when a
  component's responsibility changes.
- `PLAN.md` — mission, scope, architecture ownership, milestone status.
- `TODO.md` — numbered checklists (E1..E9). Mark `[x]` only when actually
  done and builds are verified.

When adding a feature: update the relevant checklist, the ownership notes, and
migration notes, in the same change as the code.