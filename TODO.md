# TRPG Engine — Development Checklist

Mark tasks with `[x]` as you complete them.
This file tracks only engine work in `engine/`.

---

## E1 — Build & Packaging Layer

- [x] Install vcpkg and integrate with Visual Studio 2022
- [x] Install engine dependencies (SDL3, SDL3_image, SDL3_mixer, SDL3_ttf, nlohmann_json)
- [x] Define engine static library target in [CMakeLists.txt](CMakeLists.txt)
- [x] Ensure public API is isolated to [engine/](include/engine/)
- [x] Build engine in Debug and Release configurations
- [x] Confirm `engine.lib` links into consumer project successfully
- [x] Freeze public header signatures for one milestone (no churn unless required)
- [x] Run a fresh configure + Debug + Release build after major feature batches

Checkpoint: Engine builds as a reusable static library with no consumer-side hacks.

---

## E2 — Core Math System

- [x] Implement `Vec2<T>` (basic arithmetic operators) — [Vec2.h](include/engine/math/Vec2.h)
- [x] Implement `Rect<T>` (contains, intersects) — [Rect.h](include/engine/math/Rect.h)
- [x] Implement math utilities — [MathUtils.h](include/engine/math/MathUtils.h)
  - [x] lerp
  - [x] clamp
  - [x] manhattanDistance
- [x] Implement isometric helpers — [MathUtils.h](include/engine/math/MathUtils.h)
  - [x] tileToIso
  - [x] isoToTile

Checkpoint: Math layer is fully independent of SDL and usable in any subsystem.

---

## E3 — Core Runtime System

- [x] Implement `Timer` (QueryPerformanceCounter-based delta time) — [Timer.h](include/engine/core/Timer.h) / [Timer.cpp](src/engine/core/Timer.cpp)
- [x] Implement logging system (info / warn / error, strip in Release) — [Log.h](include/engine/core/Log.h) / [Log.cpp](src/engine/core/Log.cpp)
- [x] Implement SDL Window + Renderer lifecycle — [Window.h](include/engine/core/Window.h) / [Window.cpp](src/engine/core/Window.cpp)
- [x] Implement App::run() game loop (fixed timestep) — [App.h](include/engine/core/App.h) / [App.cpp](src/engine/core/App.cpp)
- [x] Implement input system — [Input.h](include/engine/input/Input.h) / [Input.cpp](src/engine/input/Input.cpp)
  - [x] key states
  - [x] key transitions (down / held / up)

Checkpoint: Engine can run a stable empty loop with input + logging.

---

## E4 — Rendering System

### E4.1 — Resources

- [x] Implement Texture (SDL3_image loading + cleanup) — [Texture.h](include/engine/renderer/Texture.h) / [Texture.cpp](src/engine/renderer/Texture.cpp)

---

### E4.2 — Camera System

- [x] Implement Camera — [Camera.h](include/engine/renderer/Camera.h) / [Camera.cpp](src/engine/renderer/Camera.cpp)
  - [x] position (x, y)
    - [x] tile/iso -> screen transforms (`tileToScreen`, `isoSpaceRectToScreen`)
    - [x] follow/track target (`follow`, `trackTarget`)

- [x] Extend Camera (later) — [Camera.h](include/engine/renderer/Camera.h) / [Camera.cpp](src/engine/renderer/Camera.cpp)
  - [x] zoom
    - [x] clampToBounds() with map-size bounds configured via `setMapSize(...)`
  - [x] smoothing (lerp follow)

---

### E4.3 — Sprite Rendering

- [x] Implement SpriteBatch — [SpriteBatch.h](include/engine/renderer/SpriteBatch.h) / [SpriteBatch.cpp](src/engine/renderer/SpriteBatch.cpp) - [x] queue draw commands (DrawCommand with `Recti` source + `Rectf` destination) - [x] flush through engine `Renderer::drawTexture(...)`
  - [x] clear per frame

- [x] Enforce screen-space input contract — [SpriteBatch.h](include/engine/renderer/SpriteBatch.h) / [SpriteBatch.cpp](src/engine/renderer/SpriteBatch.cpp)
  - [x] SpriteBatch only accepts SDL_Rect (no world-space types)
  - [x] caller must apply camera.worldToScreen before draw()

- [x] Per-draw texture tint + blend modes — [Renderer.h](include/engine/renderer/Renderer.h) / [Renderer.cpp](src/engine/renderer/Renderer.cpp) / [SpriteBatch.h](include/engine/renderer/SpriteBatch.h) / [SpriteBatch.cpp](src/engine/renderer/SpriteBatch.cpp)
  - [x] `BlendMode{None, Blend, Add, Mod}` and matching `setBlendMode`/`getBlendMode` mapping
  - [x] `Renderer::drawTexture(..., Color tint, BlendMode blend)` applies a per-channel RGB multiply (texture color mod) and a texture blend-mode override for this draw only, restoring both on exit
  - [x] `SpriteBatch::draw(..., tint, blend)` stores and forwards them on flush
  - Migration: additive change (new defaulted parameters). Existing `drawTexture`/`SpriteBatch::draw` call sites compile unchanged. Textures are unaffected unless a non-white tint or non-`Blend` blend is passed. Note the engine's `Renderer` ignores texture `.a`; alpha stays with `setTextureAlphaMod()`.

---

### E4.4 — World Rendering

- [x] Implement TileLayer — [TileLayer.h](include/engine/renderer/TileLayer.h) / [TileLayer.cpp](src/engine/renderer/TileLayer.cpp)
  - [x] rendering order
  - [x] camera integration
  - [x] batching via SpriteBatch

---

### E4.5 — Debug Rendering (optional)

- [x] Debug draw overlay (lines / boxes) — [DebugRenderer.h](include/engine/renderer/DebugRenderer.h) / [DebugRenderer.cpp](src/engine/renderer/DebugRenderer.cpp)
- [x] Strip debug rendering in Release builds

Checkpoint: Engine can render textures on screen using a camera-driven pipeline.

---

## E5 — Scene Flow & UI Primitives

- [x] Introduce generic Scene lifecycle base  
       Edit/create: [Scene.h](include/engine/scene/Scene.h)
- [x] Implement StateMachine<T> / SceneStack<T> (push / pop / replace / update / render / isEmpty)  
       Edit/create: [StateMachine.h](include/engine/statemachine/StateMachine.h)
- [x] Implement Button primitive  
       Edit/create: [Button.h](include/engine/ui/Button.h), [Button.cpp](src/engine/ui/Button.cpp)
- [x] Implement MenuPanel input/navigation  
       Edit/create: [MenuPanel.h](include/engine/ui/MenuPanel.h), [MenuPanel.cpp](src/engine/ui/MenuPanel.cpp)
- [x] Implement TextLabel primitive  
       Edit/create: [TextLabel.h](include/engine/ui/TextLabel.h), [TextLabel.cpp](src/engine/ui/TextLabel.cpp)
- [x] Implement Slider primitive  
       Edit/create: [Slider.h](include/engine/ui/Slider.h), [Slider.cpp](src/engine/ui/Slider.cpp)
- [x] Add a game bootstrap hook so consumers can provide the first Scene without modifying engine internals  
       Edit: [App.h](include/engine/core/App.h), [App.cpp](src/engine/core/App.cpp)
- [x] Add generic focus/layout helpers for keyboard-first UI composition  
        Edit/create: [FocusGroup.h](include/engine/ui/FocusGroup.h), [IFocusable.h](include/engine/ui/IFocusable.h), [VerticalLayout.h](include/engine/ui/VerticalLayout.h)
- [x] Add a wrap toggle to `FocusGroup` (`setWrap(false)` stops at the ends instead of cycling; default keeps wrap) for fixed action lists  
        Edit: [FocusGroup.h](include/engine/ui/FocusGroup.h)
- [x] Move or rename game-flavored concepts from engine-facing APIs so this layer stays domain-neutral  
       Edit: [KeyCode.h](include/engine/input/KeyCode.h), [Input.h](include/engine/input/Input.h) (and corresponding usage sites)

Checkpoint: Generic scene transitions and UI primitives run without crashes.

---

## E6 — Content System (Hot Reload + Data Pipeline)

- [x] Add file watching service  
       Edit/create: [FileWatcher.h](include/engine/assets/FileWatcher.h), [FileWatcher.cpp](src/engine/assets/FileWatcher.cpp)
- [x] Add hot-reload dispatcher / event bus  
       Edit/create: [HotReloadBus.h](include/engine/assets/HotReloadBus.h), [HotReloadBus.cpp](src/engine/assets/HotReloadBus.cpp)
- [x] Add property id + property registry types for Tiled-backed runtime data  
       Edit/create: [PropertyId.h](include/engine/data/PropertyId.h), [PropertyRegistry.h](include/engine/data/PropertyRegistry.h)
- [x] Add engine tile map runtime data model  
       Edit/create: [TileMapData.h](include/engine/data/TileMapData.h), [TileMapData.cpp](src/engine/data/TileMapData.cpp)
- [x] Add Tiled JSON loader API  
       Edit/create: [TiledJsonLoader.h](include/engine/data/TiledJsonLoader.h), [TiledJsonLoader.cpp](src/engine/data/TiledJsonLoader.cpp)
- [x] Add focused tests for `TiledJsonLoader` and property decoding edge cases  
       Edit/create: [TiledJsonLoaderTests.cpp](tests/TiledJsonLoaderTests.cpp), fixtures under [tests/data](tests/data), test target in [CMakeLists.txt](CMakeLists.txt)
- [x] Add dev-mode debounce and reload error logging  
       Edit: [FileWatcher.cpp](src/engine/assets/FileWatcher.cpp), [HotReloadBus.cpp](src/engine/assets/HotReloadBus.cpp)
- [x] Add hot-reload sources to engine target so they are compiled and warning-checked  
       Edit: [CMakeLists.txt](CMakeLists.txt)
- [x] Implement hot-reload bus methods fully (currently stubbed)  
       Edit: [HotReloadBus.cpp](src/engine/assets/HotReloadBus.cpp)
- [ ] Validate map save → reload loop with a sample Tiled map  
       Edit: test project or game_1 integration code

Checkpoint: Dev hot reload updates tile map visuals reliably and logs reload failures without crashing.

---

## E7 — Consumer Bootstrap Contract

- [x] Expose App startup hook for a consumer-provided first Scene
- [x] Remove the need for engine-side example bootstrap edits in [App.cpp](src/engine/core/App.cpp)
- [x] Document the minimal consumer startup path from [main.cpp](../game_1/game/src/main.cpp) to first Scene

Checkpoint: A game boots into its first Scene using only public engine APIs.

---

## E8 — Animation Primitives

- [x] Add `Tween<T>` fixed-duration eased value interpolation — [Tween.h](include/engine/animation/Tween.h)
- [x] Add `AnimationState` playback interface — [AnimationState.h](include/engine/animation/AnimationState.h)
- [x] Add `Animator` named-state machine (trigger + exit-time transitions) — [Animator.h](include/engine/animation/Animator.h) / [Animator.cpp](src/engine/animation/Animator.cpp)
- [x] Add iso facing helper (`IsoDirection`, FFTA-style SW/SE/NW/NE) — [MathUtils.h](include/engine/math/MathUtils.h)
- [x] Verify all new headers are self-contained (compile alone with the real C++20 + vcpkg toolchain)
- [x] Fix pre-existing Release-only `/WX` breaks (`StateMachine.h`, `DebugRenderer.cpp`) so both Debug and Release build clean
- [x] Add `TimedState` one-shot duration state (exit-time leaf, `progress()`) — [TimedState.h](include/engine/animation/TimedState.h)
- [x] Wire game-side consumers (combat effects, floating text) to `Animator` + `TimedState` + `Tween` — `CombatAnimationSystem` (per-effect one-shot animator), `FloatingTextSystem` (rise/fade `Tween`)

Checkpoint: Engine exposes reusable tweening and an animator FSM; game_1 consumes it.

---

## E9 — Text Layout Helpers

- [x] Add `TextBlock` measure / anchored-origin / render API  
       Edit/create: [TextBlock.h](include/engine/ui/TextBlock.h), [TextBlock.cpp](src/engine/ui/TextBlock.cpp)
  - [x] wraps rows via `TextWrap` and stacks them via `VerticalLayout`
  - [x] nine-way anchor (content grows away from the anchored edge; bottom-anchored blocks rise)
  - [x] per-row horizontal alignment inside a max width
  - [x] measured row widths/heights sourced from `Renderer::measureText`
- [x] Register `TextBlock.cpp` in the engine target so it is compiled and warning-checked  
       Edit: [CMakeLists.txt](CMakeLists.txt)
- [x] Verify the new header is self-contained (compiles alone with the real toolchain)
- [x] Replace a hand-rolled bottom-anchored text layout in game_1 (`IntroState`) with the engine `TextBlock`

Checkpoint: Engine exposes reusable anchored text-block layout; consumers no longer reimplement measure/stack/align by hand.

---

## Ongoing Rules

- Engine does not own concrete game scenes, dialog presentation, or gameplay concepts
- Public headers remain stable and consumer-oriented
- Zero warnings target in Debug (/W4 /WX)
- Validate changes with a fresh configure + build periodically

---

## Documentation

- [x] Add engine component responsibility documentation (`ARCHITECTURE.md`)
- [x] Refresh development plan to match current SDL3-based implementation
- [ ] Add automated docs check/update step to keep TODO/PLAN/docs aligned
