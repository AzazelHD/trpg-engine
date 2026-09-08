# TRPG Engine - Development Plan

## Mission

Maintain a reusable static library (`engine`) that provides runtime, rendering, input, UI primitives, and content-loading utilities with no game-specific logic.

## Scope

This plan covers only `engine/`:

- Public API: `include/engine/`
- Private implementation: `src/engine/`
- Build and package contracts: `CMakeLists.txt`, install/export targets

Gameplay and content behavior belongs to `game_1/` and is tracked in `game_1/PLAN.md`.

## Current Architecture Ownership

### Core runtime (`core/`)

- `App`: fixed-step loop ownership, scene stack entrypoint, and global accessors (`Renderer`, `Window`, scene stack)
- `Window`: SDL window + renderer lifecycle and presentation configuration
- `Timer`: frame delta/fixed-step time source
- `Log`: runtime diagnostics macros/helpers

### Input (`input/`)

- `Input`: singleton event polling and key transition queries
- `KeyCode` / `GamepadCode`: engine-level input vocabulary

### Math (`math/`)

- `Vec2`, `Rect`, `MathUtils`: geometry primitives and helper math used by all systems

### Rendering (`renderer/`)

- `Renderer`: SDL abstraction and pass model (`beginLogicalPass` / `endLogicalPass`)
- `Camera`: tile/screen transforms, tracking, clamping, zoom, rotation
- `Texture`, `Font`, `FontManager`: graphics and text assets
- `SpriteBatch`, `TileLayer`: batched sprite and tile layer rendering utilities
- `DebugRenderer`: optional debug overlays in world space

### Scene and flow (`scene/`, `statemachine/`)

- `Scene`: minimal lifecycle interface for game-owned scenes
- `StateMachine<T>`: deferred push/pop/replace stack orchestration

### UI primitives and layout (`ui/`)

- Focus and widget primitives: `Button`, `Slider`, `TextLabel`, `MenuPanel`
- Settings/menu controls: `ButtonControl`, `SliderControl`, `ValueControl`
- Layout helpers: `HorizontalLayout`, `VerticalLayout`, `Insets`, `FocusGroup`

### Data/content pipeline (`data/`)

- `TileMapData`: runtime map model (tilesets, layers, objects, properties)
- `TiledJsonLoader`: Tiled JSON to runtime data conversion
- `PropertyId` / `PropertyRegistry` / `TileClass`: map metadata conventions

### Hot reload utilities (`assets/`)

- `FileWatcher`: polling-based change detection with debounce
- `HotReloadBus`: thread-safe publish/subscribe change event dispatch

### Effects (`effects/`)

- `ScreenTransition`: reusable fade/wipe transitions for scene flow

### Animation (`animation/`)

- `Tween<T>`: fixed-duration single-value interpolation with easing
- `TimedState`: one-shot duration state (exit-time leaf, `progress()`)
- `Animator`: named animation states + trigger / exit-time transitions
  (`AnimationState` playback interface)

## Milestone Status

1. Build and package contract: done
2. Core runtime and input: done
3. Renderer/camera pipeline: done
4. Scene stack and UI primitives: done
5. Tiled data pipeline and hot-reload foundation: done
6. Animation primitives (`Tween`, `Animator` FSM): done
7. Remaining integration validation: pending

## Remaining Work

1. Validate full save-reload loop for map edits against a live consumer scene.
2. Add focused tests for `TiledJsonLoader` and property decoding edge cases.
3. Verify release-stripping expectations for debug and logging paths.
4. Keep public headers stable while game-side systems evolve.

## Verification Checklist

- Debug and Release both build as static library
- Install/export config is consumable through `find_package(TRPGEngine)`
- Logical/native render pass behavior remains deterministic
- Scene transitions and UI controls remain input-safe
- Content load failures degrade safely with clear logs

## Non-Negotiables

- No game headers or gameplay concepts in engine source
- Public API remains under `include/engine/` only
- Game code interacts through engine abstractions, not SDL types directly
- Changes to public contracts require explicit migration notes
