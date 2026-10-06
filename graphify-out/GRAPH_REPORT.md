# Graph Report - engine  (2026-10-06)

## Corpus Check
- 85 files · ~27,438 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 7 file(s) not represented in the graph (top: .bat 3, (none) 2, .in 1)

## Summary
- 1502 nodes · 2222 edges · 105 communities (87 shown, 18 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 157 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `0490822d`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- Button
- Camera
- TextLabel
- FontManager
- Renderer
- FocusGroup
- StateMachine
- Animator.cpp
- Camera.cpp
- TiledJsonLoader.cpp
- KeyCode
- MenuPanel
- Log.h
- Renderer.cpp
- TileMapData
- ValueControl
- App
- TiledJsonLoaderTests.cpp
- Window
- Input
- Item
- ButtonControl
- Item
- Tween
- FileWatcher
- PropertyId
- TileLayerData
- HorizontalAlign
- BlendMode
- DrawCommand
- Layout
- MathUtils.h
- HotReloadBus
- Button.cpp
- TileLayer
- DebugRenderer.cpp
- ScreenTransition
- Renderer.h
- Animator
- FrameRatePreset
- AnimationState
- Color
- DebugRenderer.h
- Vec2f
- Slider
- MenuPanel.cpp
- App.cpp
- engine Static Library Target
- IFocusable
- FileWatcher.cpp
- ValueControl.cpp
- TileLayer.cpp
- UIAnimation
- VerticalLayout.h
- TileMapData.cpp
- App
- cstdint
- App.h
- tileToIso
- Rect
- ButtonControl::render
- WatchEntry
- PendingEvent
- Vec2
- FollowEasing
- Anchor
- Animator
- Renderer
- RectCommand
- Config
- FColor
- DebugRenderer
- TextBlock.h
- ScopedFontSize
- Texture
- Scene
- IRowControl
- ScreenTransition.cpp
- SpriteBatch::draw
- Renderer
- TileMapData
- normalisePath
- Timer
- Window.h
- VSyncMode
- Font
- Rotation
- LetterboxTransform
- Insets.h
- vcpkg.json
- IsoDirection
- Transition
- ChangeType
- Vertex
- MenuPanel::update
- vector
- TRPG::engine Export Alias
- App::run
- GamepadCode.h
- Log

## God Nodes (most connected - your core abstractions)
1. `Camera` - 53 edges
2. `Renderer` - 50 edges
3. `Button` - 39 edges
4. `MenuPanel` - 39 edges
5. `KeyCode` - 34 edges
6. `App` - 31 edges
7. `TileMapData` - 28 edges
8. `Window` - 25 edges
9. `FocusGroup` - 24 edges
10. `FileWatcher` - 23 edges

## Surprising Connections (you probably didn't know these)
- `FileWatcher::snapshotDirectory()` --references--> `WatchEntry`  [EXTRACTED]
  src/engine/assets/FileWatcher.cpp → include/engine/assets/FileWatcher.h
- `FileWatcher::snapshotFile()` --references--> `WatchEntry`  [EXTRACTED]
  src/engine/assets/FileWatcher.cpp → include/engine/assets/FileWatcher.h
- `App::getInstance()` --references--> `App`  [EXTRACTED]
  src/engine/core/App.cpp → include/engine/core/App.h
- `ScreenTransition::start()` --references--> `Config`  [EXTRACTED]
  src/engine/effects/ScreenTransition.cpp → include/engine/effects/ScreenTransition.h
- `applyEasing()` --calls--> `linear()`  [INFERRED]
  src/engine/renderer/Camera.cpp → include/engine/math/MathUtils.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Animation Playback Stack** — architecture_tween, architecture_animationstate, architecture_timedstate, architecture_animator, architecture_mathutils [EXTRACTED 1.00]
- **TextBlock Composition On TextWrap And VerticalLayout** — architecture_textblock, architecture_textwrap, architecture_verticallayout [EXTRACTED 1.00]
- **UI Primitives And Layout Layer** — architecture_button, architecture_slider, architecture_textlabel, architecture_menupanel, architecture_buttoncontrol, architecture_slidercontrol, architecture_valuecontrol, architecture_focusgroup, architecture_ifocusable, architecture_horizontallayout, architecture_verticallayout, architecture_insets [EXTRACTED 1.00]

## Communities (105 total, 18 thin omitted)

### Community 0 - "Button"
Cohesion: 0.08
Nodes (13): Button, m_enabled, m_normalColor, m_onClick, m_padding, m_rect, m_selected, m_selectedColor (+5 more)

### Community 1 - "Camera"
Cohesion: 0.05
Nodes (15): Camera, m_boundsMarginPixels, m_boundsMax, m_boundsMin, m_edgePanSpeed, m_followDuration, m_followEasing, m_mapBounds (+7 more)

### Community 2 - "TextLabel"
Cohesion: 0.05
Nodes (22): TextAnimation, shakeStrength, time, wiggleStrength, TextLabel, m_animation, m_color, m_font (+14 more)

### Community 3 - "FontManager"
Cohesion: 0.18
Nodes (5): Font, FontManager, m_fonts, m_loaded, Renderer

### Community 4 - "Renderer"
Cohesion: 0.06
Nodes (10): Renderer, m_inWorldPass, m_letterboxColor, m_logicalH, m_logicalScaleMode, m_logicalTarget, m_logicalW, m_presentationMode (+2 more)

### Community 5 - "FocusGroup"
Cohesion: 0.12
Nodes (6): FocusGroup, m_items, m_selectedIndex, m_wrap, reset(), IFocusable

### Community 6 - "StateMachine"
Cohesion: 0.06
Nodes (23): FontRole, Body, Count, Heading, Placeholder, Title, OpType, Pop (+15 more)

### Community 7 - "Animator.cpp"
Cohesion: 0.19
Nodes (7): Animator::addState(), Animator::addTransition(), Animator::enterState(), Animator::play(), Animator::stop(), Animator::trigger(), Animator::update()

### Community 8 - "Camera.cpp"
Cohesion: 0.10
Nodes (17): Camera::follow(), Camera::getDrawOrder(), Camera::isoSpaceRectToScreen(), Camera::isTileInsideMap(), Camera::pan(), Camera::rotateCCW(), Camera::rotateCW(), Camera::rotateTile() (+9 more)

### Community 9 - "TiledJsonLoader.cpp"
Cohesion: 0.13
Nodes (9): TiledJsonLoader, extractAttr(), TiledJsonLoader::internTileType(), TiledJsonLoader::loadExternalTileset(), TiledJsonLoader::loadFromFile(), TiledJsonLoader::parseLayer(), TiledJsonLoader::parseObject(), TiledJsonLoader::parseTileset() (+1 more)

### Community 10 - "KeyCode"
Cohesion: 0.06
Nodes (32): KeyCode, Accept, Advance, Back, CameraPanDown, CameraPanLeft, CameraPanRight, CameraPanUp (+24 more)

### Community 11 - "MenuPanel"
Cohesion: 0.08
Nodes (9): MenuPanel, m_bgColor, m_bgRect, m_buttons, m_focus, m_hasLayout, m_layout, m_padding (+1 more)

### Community 12 - "Log.h"
Cohesion: 0.08
Nodes (5): buildTimestamp(), latestRunFile(), write(), SliderControl::measureWidth(), SliderControl::render()

### Community 13 - "Renderer.cpp"
Cohesion: 0.11
Nodes (16): PresentationMode, Letterbox, ScaleMode, Linear, Nearest, fromSDL(), Renderer::getBlendMode(), Renderer::getDrawColor() (+8 more)

### Community 14 - "TileMapData"
Cohesion: 0.08
Nodes (20): TileMapData, height, layers, properties, tileHeight, tilesets, tileTypeNames, tileWidth (+12 more)

### Community 16 - "App"
Cohesion: 0.09
Nodes (10): App, kDefaultFixedStepSeconds, kDefaultFrameRatePreset, m_fixedStep, m_frameRatePreset, m_running, m_sceneStack, m_targetFrameSeconds (+2 more)

### Community 17 - "TiledJsonLoaderTests.cpp"
Cohesion: 0.21
Nodes (11): A, checkEqImpl(), fixtureDir(), main(), reportFailure(), reportFailureImplChecked(), testLoaderErrors(), testLoaderExternalTileset() (+3 more)

### Community 18 - "Window"
Cohesion: 0.10
Nodes (7): Window, m_height, m_renderer, m_sdlRenderer, m_vsync, m_width, m_window

### Community 19 - "Input"
Cohesion: 0.11
Nodes (11): Input, m_consumedThisFrame, m_currentKeys, m_mouseButtons, m_mousePosition, m_pressedThisFrame, m_previousKeys, m_releasedThisFrame (+3 more)

### Community 20 - "Item"
Cohesion: 0.23
Nodes (11): Container, items, padding, ContainerResult, box, itemRects, HorizontalLayout, Item (+3 more)

### Community 21 - "ButtonControl"
Cohesion: 0.21
Nodes (3): ButtonControl, m_onClick, m_selected

### Community 22 - "Item"
Cohesion: 0.22
Nodes (11): Container, items, padding, ContainerResult, box, itemRects, Item, height (+3 more)

### Community 23 - "Tween"
Cohesion: 0.12
Nodes (10): Tween, m_active, m_duration, m_easing, m_elapsed, m_finished, m_from, m_progress (+2 more)

### Community 24 - "FileWatcher"
Cohesion: 0.12
Nodes (5): FileWatcher, m_changeCallback, m_debounce, m_entries, FileWatcher::poll()

### Community 25 - "PropertyId"
Cohesion: 0.19
Nodes (8): PropertyId, Damage, Solid, SpawnPoint, Unknown, PropertyRegistry, m_map, testPropertyRegistry()

### Community 26 - "TileLayerData"
Cohesion: 0.08
Nodes (27): LayerType, Image, Object, Tile, MapObject, className, id, isPoint (+19 more)

### Community 27 - "HorizontalAlign"
Cohesion: 0.14
Nodes (13): HorizontalAlign, Center, Left, Right, VerticalAlign, Bottom, Middle, Top (+5 more)

### Community 28 - "BlendMode"
Cohesion: 0.21
Nodes (10): BlendMode, Add, Blend, Mod, None, Renderer::drawRect(), Renderer::drawTexture(), Renderer::fillRect() (+2 more)

### Community 29 - "DrawCommand"
Cohesion: 0.13
Nodes (10): DrawCommand, blend, dst, flipH, src, texture, tint, SpriteBatch (+2 more)

### Community 30 - "Layout"
Cohesion: 0.18
Nodes (6): Layout, box, rowRects, TextBlock::anchoredOrigin(), TextBlock::measure(), TextBlock::render()

### Community 31 - "MathUtils.h"
Cohesion: 0.23
Nodes (12): bounce(), easeIn(), easeInCubic(), easeInOut(), easeInOutCubic(), easeOut(), easeOutCubic(), lerp() (+4 more)

### Community 32 - "HotReloadBus"
Cohesion: 0.06
Nodes (22): DispatchResult, Continue, Stop, HotReloadBus, m_handlers, m_mutex, m_nextId, m_order (+14 more)

### Community 33 - "Button.cpp"
Cohesion: 0.15
Nodes (5): Button::Button(), Button::getRect(), Button::render(), Button::setPosition(), Button::translate()

### Community 34 - "TileLayer"
Cohesion: 0.14
Nodes (10): Camera, SpriteBatch, TileLayer, m_mapHeight, m_mapWidth, m_tileH, m_tiles, m_tileset (+2 more)

### Community 35 - "DebugRenderer.cpp"
Cohesion: 0.15
Nodes (6): DebugRenderer::addIsoLine(), DebugRenderer::addIsoRect(), DebugRenderer::addScreenCircle(), DebugRenderer::addScreenLine(), DebugRenderer::addScreenRect(), DebugRenderer::flush()

### Community 36 - "ScreenTransition"
Cohesion: 0.15
Nodes (7): ScreenTransition, m_active, m_config, m_progress, m_state, State, alpha

### Community 37 - "Renderer.h"
Cohesion: 0.17
Nodes (3): Font, SDL_Renderer, Texture

### Community 38 - "Animator"
Cohesion: 0.15
Nodes (4): Animator, m_currentName, m_states, m_transitions

### Community 39 - "FrameRatePreset"
Cohesion: 0.21
Nodes (10): FrameRatePreset, Fps120, Fps30, Fps60, VSync, App::App(), App::setFrameRatePreset(), frameRatePresetName() (+2 more)

### Community 40 - "AnimationState"
Cohesion: 0.16
Nodes (4): AnimationState, TimedState, m_duration, m_elapsed

### Community 41 - "Color"
Cohesion: 0.17
Nodes (8): Color, a, b, g, r, Renderer::clear(), Renderer::setDrawColor(), Renderer::setLetterboxColor()

### Community 42 - "DebugRenderer.h"
Cohesion: 0.25
Nodes (6): Camera, LineCommand, a, b, color, Renderer

### Community 43 - "Vec2f"
Cohesion: 0.21
Nodes (8): Renderer::drawDebugText(), Renderer::drawGeometry(), Renderer::drawLine(), Renderer::endLogicalPass(), Renderer::measureText(), Renderer::renderText(), Renderer::toNativePos(), Renderer::toNativeRect()

### Community 44 - "Slider"
Cohesion: 0.05
Nodes (22): RenderStyle, handleHeight, handleWidth, offsetY, trackHeight, Slider, m_max, m_min (+14 more)

### Community 45 - "MenuPanel.cpp"
Cohesion: 0.10
Nodes (7): MenuPanel::addButton(), MenuPanel::clearLayout(), MenuPanel::refreshLayout(), MenuPanel::render(), MenuPanel::setPosition(), MenuPanel::setVerticalLayout(), MenuPanel::size()

### Community 46 - "App.cpp"
Cohesion: 0.15
Nodes (7): App::getInstance(), App::getRenderer(), App::getSceneStack(), App::getWindow(), LoggedKey, code, name

### Community 47 - "engine Static Library Target"
Cohesion: 0.23
Nodes (8): Build and Packaging Contract, Current Known Gaps, High-Level Layering, TiledJsonLoader, TRPG Engine Architecture, engine Static Library Target, engine_tests Target, nlohmann_json Dependency

### Community 48 - "IFocusable"
Cohesion: 0.17
Nodes (12): Button, ButtonControl, FocusGroup, HorizontalLayout, IFocusable, Insets, MenuPanel, Slider (+4 more)

### Community 49 - "FileWatcher.cpp"
Cohesion: 0.23
Nodes (7): clampDebounce(), FileWatcher::FileWatcher(), FileWatcher::getDebounce(), FileWatcher::setChangeCallback(), FileWatcher::setDebounce(), FileWatcher::snapshotDirectory(), FileWatcher::snapshotFile()

### Community 50 - "ValueControl.cpp"
Cohesion: 0.25
Nodes (6): m_getValue, m_onAdjust, ValueControl::handleLeft(), ValueControl::handleRight(), ValueControl::measureWidth(), ValueControl::render()

### Community 51 - "TileLayer.cpp"
Cohesion: 0.24
Nodes (3): TileLayer::render(), TileLayer::setTiles(), TileLayer::TileLayer()

### Community 52 - "UIAnimation"
Cohesion: 0.27
Nodes (3): UIAnimation, UIAnimationTrack, m_animations

### Community 53 - "VerticalLayout.h"
Cohesion: 0.18
Nodes (10): apply(), HorizontalAlignment, Center, Left, Right, VerticalLayoutConfig, alignment, padding (+2 more)

### Community 55 - "TileMapData.cpp"
Cohesion: 0.24
Nodes (5): findProperty(), TileMapData::findLayer(), TileMapData::tileTypeForGlobalTileId(), TileMapData::tileTypeId(), TileMapData::tileTypeName()

### Community 56 - "App"
Cohesion: 0.29
Nodes (11): App, GamepadCode, Input, KeyCode, Runtime Flow, Scene, ScreenTransition, StateMachine<T> (+3 more)

### Community 58 - "App.h"
Cohesion: 0.20
Nodes (9): Font, Renderer, Scene, StateMachine, Window, WindowStartupConfig, borderless, height (+1 more)

### Community 59 - "tileToIso"
Cohesion: 0.22
Nodes (7): isoDirectionForTileDelta(), isoToTile(), manhattanDistance(), tileToIso(), Camera::clampToBounds(), Camera::rebuildBoundsCache(), Camera::tileToScreen()

### Community 60 - "Rect"
Cohesion: 0.27
Nodes (5): Rect, h, w, x, y

### Community 61 - "ButtonControl::render"
Cohesion: 0.33
Nodes (3): m_getLabel, m_labelFormatter, ButtonControl::render()

### Community 62 - "WatchEntry"
Cohesion: 0.20
Nodes (7): WatchEntry, fileLastNotifications, fileTimestamps, isDirectory, lastNotification, path, recursive

### Community 63 - "PendingEvent"
Cohesion: 0.27
Nodes (6): PendingEvent, path, type, FileWatcher::isDebounceReady(), FileWatcher::pollDirectoryEntry(), FileWatcher::pollFileEntry()

### Community 64 - "Vec2"
Cohesion: 0.17
Nodes (4): Vec2, x, y, Renderer

### Community 65 - "FollowEasing"
Cohesion: 0.20
Nodes (10): FollowEasing, Bounce, EaseIn, EaseInCubic, EaseInOut, EaseInOutCubic, EaseOut, EaseOutCubic (+2 more)

### Community 66 - "Anchor"
Cohesion: 0.20
Nodes (10): Anchor, Bottom, BottomLeft, BottomRight, Center, Left, Right, Top (+2 more)

### Community 67 - "Animator"
Cohesion: 0.33
Nodes (9): AnimationState, Animator, Camera, IsoDirection, MathUtils, Rect, TimedState, Tween<T> (+1 more)

### Community 68 - "Renderer"
Cohesion: 0.25
Nodes (9): BlendMode, DebugRenderer, Font, FontManager, Renderer, SpriteBatch, TextLabel, Texture (+1 more)

### Community 69 - "RectCommand"
Cohesion: 0.40
Nodes (4): RectCommand, color, filled, rect

### Community 70 - "Config"
Cohesion: 0.22
Nodes (6): Config, color, duration, easing, onComplete, transition

### Community 71 - "FColor"
Cohesion: 0.22
Nodes (5): FColor, a, b, g, r

### Community 73 - "TextBlock.h"
Cohesion: 0.25
Nodes (7): Font, Line, bold, color, font, text, Renderer

### Community 74 - "ScopedFontSize"
Cohesion: 0.28
Nodes (6): ScopedFontSize, m_font, m_previousSize, ScopedFontStyle, m_font, m_previousStyle

### Community 77 - "Texture"
Cohesion: 0.25
Nodes (4): Texture, m_height, m_texture, m_width

### Community 79 - "IRowControl"
Cohesion: 0.20
Nodes (3): Font, IRowControl, Renderer

### Community 84 - "TileMapData"
Cohesion: 0.29
Nodes (7): FileWatcher, HotReloadBus, PropertyId, PropertyRegistry, TileClass, TileLayer, TileMapData

### Community 85 - "normalisePath"
Cohesion: 0.43
Nodes (4): FileWatcher::unwatch(), FileWatcher::watchDirectory(), FileWatcher::watchFile(), normalisePath()

### Community 86 - "Timer"
Cohesion: 0.29
Nodes (4): Timer, m_frequency, m_lastCounter, m_startCounter

### Community 87 - "Window.h"
Cohesion: 0.29
Nodes (6): DisplayResolution, height, width, SDL_Renderer, SDL_Window, Window::GetPrimaryDesktopResolution()

### Community 89 - "VSyncMode"
Cohesion: 0.29
Nodes (6): VSyncMode, Adaptive, Disabled, Enabled, Window::setVSync(), Window::Window()

### Community 90 - "Font"
Cohesion: 0.40
Nodes (3): Font, m_font, m_size

### Community 91 - "Rotation"
Cohesion: 0.33
Nodes (5): Rotation, R0, R180, R270, R90

### Community 92 - "LetterboxTransform"
Cohesion: 0.33
Nodes (6): LetterboxTransform, offsetX, offsetY, scaleX, scaleY, Renderer::computeLetterboxTransform()

### Community 95 - "vcpkg.json"
Cohesion: 0.33
Nodes (5): builtin-baseline, dependencies, name, $schema, version

### Community 96 - "IsoDirection"
Cohesion: 0.40
Nodes (5): IsoDirection, NorthEast, NorthWest, SouthEast, SouthWest

### Community 97 - "Transition"
Cohesion: 0.50
Nodes (4): Transition, from, to, trigger

### Community 98 - "ChangeType"
Cohesion: 0.50
Nodes (4): ChangeType, Added, Modified, Removed

### Community 99 - "Vertex"
Cohesion: 0.50
Nodes (3): Vertex, color, position

### Community 101 - "vector"
Cohesion: 0.22
Nodes (4): Renderer, Font, Renderer, TextWrap

## Ambiguous Edges - Review These
- `engine Static Library Target` → `Font`  [AMBIGUOUS]
  CMakeLists.txt · relation: references
- `engine Static Library Target` → `Scene`  [AMBIGUOUS]
  CMakeLists.txt · relation: references

## Knowledge Gaps
- **404 isolated node(s):** `from`, `to`, `trigger`, `m_states`, `m_transitions` (+399 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 821 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `engine Static Library Target` and `Font`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **Why does `Color` connect `Color` to `Button`, `TextLabel`, `Renderer`, `MenuPanel`, `Log.h`, `Renderer.cpp`, `HorizontalAlign`, `BlendMode`, `DrawCommand`, `DebugRenderer.cpp`, `Renderer.h`, `DebugRenderer.h`, `Vec2f`, `App.cpp`, `ValueControl.cpp`, `cstdint`, `ButtonControl::render`, `Vec2`, `RectCommand`, `FColor`, `TextBlock.h`, `IRowControl`, `SpriteBatch::draw`, `App::run`?**
  _High betweenness centrality (0.144) - this node is a cross-community bridge._
- **What connects `from`, `to`, `trigger` to the rest of the system?**
  _404 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Button` be split into smaller, more focused modules?**
  _Cohesion score 0.07586206896551724 - nodes in this community are weakly interconnected._
- **What is the exact relationship between `engine Static Library Target` and `Scene`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **Why does `Camera` connect `Camera` to `Vec2`, `FollowEasing`, `Camera.cpp`, `tileToIso`, `Rotation`?**
  _High betweenness centrality (0.091) - this node is a cross-community bridge._
- **Should `Camera` be split into smaller, more focused modules?**
  _Cohesion score 0.049682875264270614 - nodes in this community are weakly interconnected._