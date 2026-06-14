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
  - [x] worldToScreen(Rectf) -> SDL_Rect
  - [x] follow(Vec2f target)

- [x] Extend Camera (later) — [Camera.h](include/engine/renderer/Camera.h) / [Camera.cpp](src/engine/renderer/Camera.cpp)
  - [x] zoom
  - [x] clampToBounds(Rectf mapBounds)
  - [x] smoothing (lerp follow)

---

### E4.3 — Sprite Rendering

- [x] Implement SpriteBatch — [SpriteBatch.h](include/engine/renderer/SpriteBatch.h) / [SpriteBatch.cpp](src/engine/renderer/SpriteBatch.cpp)
  - [x] queue draw commands (DrawCommand with SDL_Rect, flushed as SDL_FRect)
  - [x] flush via SDL_RenderTextureRotated
  - [x] clear per frame

- [x] Enforce screen-space input contract — [SpriteBatch.h](include/engine/renderer/SpriteBatch.h) / [SpriteBatch.cpp](src/engine/renderer/SpriteBatch.cpp)
  - [x] SpriteBatch only accepts SDL_Rect (no world-space types)
  - [x] caller must apply camera.worldToScreen before draw()

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
- [x] Keep IState compatibility wrapper for existing game code  
       Edit/create: [IState.h](include/engine/statemachine/IState.h)
- [x] Implement StateMachine<T> / SceneStack<T> (push / pop / replace / update / render / isEmpty)  
       Edit/create: [StateMachine.h](include/engine/statemachine/StateMachine.h), [SceneStack.h](include/engine/scene/SceneStack.h)
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

## Ongoing Rules

- Engine does not own concrete game scenes, dialog presentation, or gameplay concepts
- Public headers remain stable and consumer-oriented
- Zero warnings target in Debug (/W4 /WX)
- Validate changes with a fresh configure + build periodically
