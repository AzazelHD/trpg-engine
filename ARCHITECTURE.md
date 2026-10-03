# TRPG Engine Architecture

## Purpose

`engine` is a reusable static library that provides runtime loop, rendering, input, UI primitives, and map/data support for multiple games.

The engine must stay gameplay-agnostic. Game-specific rules, scene behavior, AI, and content logic belong to `game_1`.

## High-Level Layering

1. Runtime and loop: `core/`
2. Input and math: `input/`, `math/`
3. Rendering and effects: `renderer/`, `effects/`
4. Animation: `animation/`
5. Scene stack and flow: `scene/`, `statemachine/`
6. UI primitives and layout: `ui/`
7. Data and hot reload: `data/`, `assets/`

## Component Responsibilities

### Core (`include/engine/core`)

- `App`: owns SDL lifetime, `Window`, main loop, and scene stack dispatch.
- `Window`: manages native window and renderer initialization/presentation.
- `Timer`: frame timing source.
- `Log`: runtime logging and diagnostics wrappers.

### Input (`include/engine/input`)

- `Input`: singleton polling SDL events and exposing down/pressed/released queries.
- `KeyCode`, `GamepadCode`: stable game-facing key vocabulary.

### Math (`include/engine/math`)

- `Vec2`, `Rect`: shared numeric geometry types.
- `MathUtils`: interpolation, clamps, distance helpers, and utility transforms.

### Rendering (`include/engine/renderer`)

- `Renderer`: SDL abstraction for drawing, text, textures, and logical/native pass boundaries. `drawTexture()` accepts a per-draw RGB tint and `BlendMode{None, Blend, Add, Mod}` override for recolours/effects.
- `Camera`: tile/screen transforms, follow/tracking, zoom, rotation, and clamping.
- `Texture`, `Font`, `FontManager`: texture/font asset interfaces.
- `SpriteBatch`: queued sprite draw submission utility; forwards a per-command tint/blend through to `Renderer::drawTexture()`.
- `TileLayer`: tile-layer rendering helper.
- `DebugRenderer`: optional debug draw support.

### Scene and state flow (`include/engine/scene`, `include/engine/statemachine`)

- `Scene`: base lifecycle contract (`onEnter`, `handleInput`, `update`, `render`, `onExit`).
- `StateMachine<T>`: deferred push/pop/replace stack operations to prevent re-entrant state mutations.

### UI primitives and layout (`include/engine/ui`)

- Widgets: `Button`, `Slider`, `TextLabel`, `MenuPanel`.
- Settings/menu controls: `ButtonControl`, `SliderControl`, `ValueControl`.
- Focus and layout: `FocusGroup` (wrap toggle via `setWrap`), `IFocusable`, `HorizontalLayout`, `VerticalLayout`, `Insets`.
- Text layout: `TextWrap` (word-wrap text to a max width); `TextBlock`
  (measure, anchor, and draw a stacked block of wrapped text rows — composed
  on `TextWrap` + `VerticalLayout`, with nine-way anchor semantics where
  content grows away from the anchored edge).

### Data and content (`include/engine/data`)

- `TileMapData`: runtime map schema for tilesets/layers/objects/properties.
- `TiledJsonLoader`: parser from Tiled JSON (`.tmj`) into runtime map data.
- `PropertyId`, `PropertyRegistry`, `TileClass`: map metadata and classification conventions.

### Hot reload (`include/engine/assets`)

- `FileWatcher`: polled filesystem watcher with debounce.
- `HotReloadBus`: thread-safe asset change event bus.

### Effects (`include/engine/effects`)

- `ScreenTransition`: shared scene transition animation helper.

### Animation (`include/engine/animation`)

- `Tween<T>`: single-value interpolation over a fixed duration with a
  MathUtils easing function. Works for scalars and `Vec2<T>`.
- `AnimationState`: playback-behavior interface (enter/update/exit/finished).
- `TimedState`: concrete one-shot state that plays one duration then reports
  finished — the exit-time leaf for timed effects; exposes eased `progress()`.
- `Animator`: Unity-style named-state machine — trigger and exit-time
  transitions between states. Gameplay-agnostic; the game decides when to
  `play()`/`trigger()`, how states sample sprites, and what facing to use
  (`IsoDirection` in `MathUtils`). Movable, so it can live in STL containers.

## Runtime Flow

1. Consumer constructs `App` with scene factory.
2. `App` creates `Window` and renderer wrapper.
3. Main loop:
   - process input/events
   - fixed-step `update(dt)`
   - interpolated `render(alpha)`
4. Scene stack dispatches to active scene.

## Public API Boundary

Only headers under `include/engine` are supported integration points.

Rules:

- Game code should not include SDL headers directly for normal runtime usage.
- Engine implementation details in `src/engine` are internal and may change.
- Any breaking change in `include/engine` should be accompanied by migration notes.

## Build and Packaging Contract

- Build target: `engine` (static library).
- Export alias: `TRPG::engine`.
- Install/export package config generated via CMake for consumer `find_package(TRPGEngine)` flows.
- Dependencies currently linked through vcpkg: SDL3 family and `nlohmann_json`.

## Current Known Gaps

1. End-to-end automated tests for map loader/property parsing are still limited.
2. Hot reload integration depends on consumer-side wiring patterns.
3. Some debug/release behavior expectations rely on convention and should be codified by tests.
