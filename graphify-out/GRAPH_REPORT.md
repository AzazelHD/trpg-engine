# Graph Report - engine  (2026-10-05)

## Corpus Check
- Corpus is ~27,522 words - fits in a single context window. You may not need a graph.

## Summary
- 1503 nodes · 2225 edges · 110 communities (91 shown, 19 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 157 edges (avg confidence: 0.84)
- Token cost: 21,000 input · 4,300 output

## Community Hubs (Navigation)
- Button Widget API
- Camera Public Interface
- Text Animation Timing
- FontManager Resource Cache
- Renderer Draw Passes
- FocusGroup Keyboard Navigation
- StateMachine Core Types
- AnimationState Lifecycle
- Camera Tile Transform
- TiledJsonLoader Parsing
- KeyCode Input Vocabulary
- MenuPanel Composition
- Logging Facility
- Presentation Mode Enums
- TileMapData Fields
- ValueControl Abstraction
- App Singleton Bootstrap
- TileMapData Lookups
- Window Creation Sizing
- Input Polling API
- HorizontalLayout Containers
- ButtonControl Adapter
- Container Layout Result
- Tween Easing Core
- FileWatcher Debounce Polling
- PropertyId Map Conventions
- Tiled Layer JSON Schema
- Text Alignment Enums
- BlendMode SDL Conversion
- SpriteBatch DrawCommand
- TextBlock Layout
- Easing Function Math
- HotReload Dispatch Result
- HotReloadBus Handlers
- TileLayer Rendering
- DebugRenderer Line Drawing
- ScreenTransition State Machine
- Text Width Measurement
- Animator Named State Machine
- FrameRatePreset Startup Config
- KeyCode To Scancode Mapping
- Color Type
- DebugRenderer LineCommand
- Letterbox Transform Math
- Slider Widget
- MenuPanel Navigation Impl
- Scene Lifecycle Dispatch
- Architecture Doc Concepts
- UI Primitive Aggregation
- FileWatcher Debounce Impl
- HotReloadBus Subscription Ids
- SliderControl Adapter
- UIAnimation Sequence
- VerticalLayout Config
- TileMapData Property Lookup
- Runtime Flow Doc Concepts
- Core Data Headers
- WindowStartupConfig Header
- Isometric Tile Math
- Rect Geometry
- Slider Range Clamping
- FileWatcher Timestamp State
- FileWatcher Pending Events
- Vec2 Headers
- FollowEasing Curve Kinds
- Anchor Enums
- Animation Concept Aggregation
- Renderer Subsystem Facade
- ScreenTransition Config
- FColor Type
- DebugRenderer Shape API
- TextBlock Line Layout
- Scoped Font State
- Widget Header Includes
- Tiled Object Schema
- Texture Resource
- Scene Interface
- IRowControl Interface
- Insets Padding
- ScreenTransition Impl
- SpriteBatch Impl
- TextWrap Impl
- Data Module Aggregation
- FileWatcher Registration
- Timer Frame Pacing
- Window Handle Impl
- VSyncMode Config
- Font Rendering
- Tile Rotation Enums
- LetterboxTransform Struct
- Insets Static Helpers
- Slider RenderStyle
- vcpkg Manifest
- IsoDirection Enum
- Animator Transition Struct
- FileWatcher ChangeType
- Vertex Struct
- MenuPanel Cancel Navigation
- TextWrap Header
- CMake Package Config
- App Event Loop
- MenuPanel Button Rebuild
- Slider Rendering Impl
- LoggedKey Struct
- GamepadCode Header
- Log Singleton Type

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
- `Input::keyCodeToScancode()` --references--> `KeyCode`  [EXTRACTED]
  src/engine/input/Input.cpp → include/engine/input/KeyCode.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **TextBlock Composition On TextWrap And VerticalLayout** — architecture_textblock, architecture_textwrap, architecture_verticallayout [EXTRACTED 1.00]
- **UI Primitives And Layout Layer** — architecture_button, architecture_slider, architecture_textlabel, architecture_menupanel, architecture_buttoncontrol, architecture_slidercontrol, architecture_valuecontrol, architecture_focusgroup, architecture_ifocusable, architecture_horizontallayout, architecture_verticallayout, architecture_insets [EXTRACTED 1.00]
- **Animation Playback Stack** — architecture_tween, architecture_animationstate, architecture_timedstate, architecture_animator, architecture_mathutils [EXTRACTED 1.00]

## Communities (110 total, 19 thin omitted)

### Community 0 - "Button Widget API"
Cohesion: 0.05
Nodes (18): Button, m_enabled, m_normalColor, m_onClick, m_padding, m_rect, m_selected, m_selectedColor (+10 more)

### Community 1 - "Camera Public Interface"
Cohesion: 0.05
Nodes (15): Camera, m_boundsMarginPixels, m_boundsMax, m_boundsMin, m_edgePanSpeed, m_followDuration, m_followEasing, m_mapBounds (+7 more)

### Community 2 - "Text Animation Timing"
Cohesion: 0.05
Nodes (22): TextAnimation, shakeStrength, time, wiggleStrength, TextLabel, m_animation, m_color, m_font (+14 more)

### Community 3 - "FontManager Resource Cache"
Cohesion: 0.05
Nodes (20): Font, FontManager, m_fonts, m_loaded, FontRole, Body, Count, Heading (+12 more)

### Community 4 - "Renderer Draw Passes"
Cohesion: 0.06
Nodes (10): Renderer, m_inWorldPass, m_letterboxColor, m_logicalH, m_logicalScaleMode, m_logicalTarget, m_logicalW, m_presentationMode (+2 more)

### Community 5 - "FocusGroup Keyboard Navigation"
Cohesion: 0.13
Nodes (6): FocusGroup, m_items, m_selectedIndex, m_wrap, reset(), IFocusable

### Community 6 - "StateMachine Core Types"
Cohesion: 0.12
Nodes (11): OpType, Pop, Push, Replace, PendingOp, state, type, StateMachine (+3 more)

### Community 7 - "AnimationState Lifecycle"
Cohesion: 0.09
Nodes (11): AnimationState, TimedState, m_duration, m_elapsed, Animator::addState(), Animator::addTransition(), Animator::enterState(), Animator::play() (+3 more)

### Community 8 - "Camera Tile Transform"
Cohesion: 0.10
Nodes (17): Camera::follow(), Camera::getDrawOrder(), Camera::isoSpaceRectToScreen(), Camera::isTileInsideMap(), Camera::pan(), Camera::rotateCCW(), Camera::rotateCW(), Camera::rotateTile() (+9 more)

### Community 9 - "TiledJsonLoader Parsing"
Cohesion: 0.13
Nodes (9): TiledJsonLoader, extractAttr(), TiledJsonLoader::internTileType(), TiledJsonLoader::loadExternalTileset(), TiledJsonLoader::loadFromFile(), TiledJsonLoader::parseLayer(), TiledJsonLoader::parseObject(), TiledJsonLoader::parseTileset() (+1 more)

### Community 10 - "KeyCode Input Vocabulary"
Cohesion: 0.08
Nodes (26): KeyCode, Accept, Advance, Back, CameraPanDown, CameraPanLeft, CameraPanRight, CameraPanUp (+18 more)

### Community 11 - "MenuPanel Composition"
Cohesion: 0.08
Nodes (9): MenuPanel, m_bgColor, m_bgRect, m_buttons, m_focus, m_hasLayout, m_layout, m_padding (+1 more)

### Community 12 - "Logging Facility"
Cohesion: 0.09
Nodes (5): buildTimestamp(), latestRunFile(), write(), SliderControl::measureWidth(), SliderControl::render()

### Community 13 - "Presentation Mode Enums"
Cohesion: 0.10
Nodes (15): PresentationMode, Letterbox, ScaleMode, Linear, Nearest, Renderer::clear(), Renderer::getTextureSize(), Renderer::loadTexture() (+7 more)

### Community 14 - "TileMapData Fields"
Cohesion: 0.09
Nodes (20): TileMapData, height, layers, properties, tileHeight, tilesets, tileTypeNames, tileWidth (+12 more)

### Community 15 - "ValueControl Abstraction"
Cohesion: 0.11
Nodes (8): ValueControl, m_getValue, m_onAdjust, m_selected, ValueControl::handleLeft(), ValueControl::handleRight(), ValueControl::measureWidth(), ValueControl::render()

### Community 16 - "App Singleton Bootstrap"
Cohesion: 0.09
Nodes (10): App, kDefaultFixedStepSeconds, kDefaultFrameRatePreset, m_fixedStep, m_frameRatePreset, m_running, m_sceneStack, m_targetFrameSeconds (+2 more)

### Community 17 - "TileMapData Lookups"
Cohesion: 0.17
Nodes (11): A, checkEqImpl(), fixtureDir(), main(), reportFailure(), reportFailureImplChecked(), testLoaderErrors(), testLoaderExternalTileset() (+3 more)

### Community 18 - "Window Creation Sizing"
Cohesion: 0.10
Nodes (7): Window, m_height, m_renderer, m_sdlRenderer, m_vsync, m_width, m_window

### Community 19 - "Input Polling API"
Cohesion: 0.11
Nodes (11): Input, m_consumedThisFrame, m_currentKeys, m_mouseButtons, m_mousePosition, m_pressedThisFrame, m_previousKeys, m_releasedThisFrame (+3 more)

### Community 20 - "HorizontalLayout Containers"
Cohesion: 0.21
Nodes (11): Container, items, padding, ContainerResult, box, itemRects, HorizontalLayout, Item (+3 more)

### Community 21 - "ButtonControl Adapter"
Cohesion: 0.13
Nodes (6): ButtonControl, m_getLabel, m_labelFormatter, m_onClick, m_selected, ButtonControl::render()

### Community 22 - "Container Layout Result"
Cohesion: 0.22
Nodes (11): Container, items, padding, ContainerResult, box, itemRects, Item, height (+3 more)

### Community 23 - "Tween Easing Core"
Cohesion: 0.12
Nodes (10): Tween, m_active, m_duration, m_easing, m_elapsed, m_finished, m_from, m_progress (+2 more)

### Community 24 - "FileWatcher Debounce Polling"
Cohesion: 0.12
Nodes (5): FileWatcher, m_changeCallback, m_debounce, m_entries, FileWatcher::poll()

### Community 25 - "PropertyId Map Conventions"
Cohesion: 0.16
Nodes (11): PropertyId, Damage, Solid, SpawnPoint, Unknown, PropertyRegistry, m_map, TileProperty (+3 more)

### Community 26 - "Tiled Layer JSON Schema"
Cohesion: 0.11
Nodes (17): LayerType, Image, Object, Tile, TileLayerData, className, height, name (+9 more)

### Community 27 - "Text Alignment Enums"
Cohesion: 0.14
Nodes (13): HorizontalAlign, Center, Left, Right, VerticalAlign, Bottom, Middle, Top (+5 more)

### Community 28 - "BlendMode SDL Conversion"
Cohesion: 0.19
Nodes (12): BlendMode, Add, Blend, Mod, None, fromSDL(), Renderer::drawRect(), Renderer::drawTexture() (+4 more)

### Community 29 - "SpriteBatch DrawCommand"
Cohesion: 0.13
Nodes (10): DrawCommand, blend, dst, flipH, src, texture, tint, SpriteBatch (+2 more)

### Community 30 - "TextBlock Layout"
Cohesion: 0.18
Nodes (6): Layout, box, rowRects, TextBlock::anchoredOrigin(), TextBlock::measure(), TextBlock::render()

### Community 31 - "Easing Function Math"
Cohesion: 0.23
Nodes (12): bounce(), easeIn(), easeInCubic(), easeInOut(), easeInOutCubic(), easeOut(), easeOutCubic(), lerp() (+4 more)

### Community 32 - "HotReload Dispatch Result"
Cohesion: 0.13
Nodes (13): DispatchResult, Continue, Stop, HotReloadEvent, assetPath, oldPath, type, HotReloadEventType (+5 more)

### Community 33 - "HotReloadBus Handlers"
Cohesion: 0.13
Nodes (5): HotReloadBus, m_handlers, m_mutex, m_nextId, m_order

### Community 34 - "TileLayer Rendering"
Cohesion: 0.14
Nodes (10): Camera, SpriteBatch, TileLayer, m_mapHeight, m_mapWidth, m_tileH, m_tiles, m_tileset (+2 more)

### Community 35 - "DebugRenderer Line Drawing"
Cohesion: 0.18
Nodes (4): DebugRenderer::addIsoLine(), DebugRenderer::addScreenCircle(), DebugRenderer::addScreenLine(), DebugRenderer::flush()

### Community 36 - "ScreenTransition State Machine"
Cohesion: 0.14
Nodes (7): ScreenTransition, m_active, m_config, m_progress, m_state, State, alpha

### Community 37 - "Text Width Measurement"
Cohesion: 0.17
Nodes (3): Font, SDL_Renderer, Texture

### Community 38 - "Animator Named State Machine"
Cohesion: 0.15
Nodes (4): Animator, m_currentName, m_states, m_transitions

### Community 39 - "FrameRatePreset Startup Config"
Cohesion: 0.21
Nodes (10): FrameRatePreset, Fps120, Fps30, Fps60, VSync, App::App(), App::setFrameRatePreset(), frameRatePresetName() (+2 more)

### Community 40 - "KeyCode To Scancode Mapping"
Cohesion: 0.19
Nodes (6): Input::consumeKey(), Input::getMousePosition(), Input::isKeyDown(), Input::isKeyPressed(), Input::isKeyReleased(), Input::keyCodeToScancode()

### Community 41 - "Color Type"
Cohesion: 0.17
Nodes (9): Color, a, b, g, r, DebugRenderer::addIsoRect(), DebugRenderer::addScreenRect(), Renderer::getDrawColor() (+1 more)

### Community 42 - "DebugRenderer LineCommand"
Cohesion: 0.15
Nodes (10): Camera, LineCommand, a, b, color, RectCommand, color, filled (+2 more)

### Community 43 - "Letterbox Transform Math"
Cohesion: 0.21
Nodes (8): Renderer::drawDebugText(), Renderer::drawGeometry(), Renderer::drawLine(), Renderer::endLogicalPass(), Renderer::measureText(), Renderer::renderText(), Renderer::toNativePos(), Renderer::toNativeRect()

### Community 44 - "Slider Widget"
Cohesion: 0.15
Nodes (6): Slider, m_max, m_min, m_style, m_track, m_value

### Community 45 - "MenuPanel Navigation Impl"
Cohesion: 0.15
Nodes (3): MenuPanel::refreshLayout(), MenuPanel::render(), MenuPanel::size()

### Community 46 - "Scene Lifecycle Dispatch"
Cohesion: 0.19
Nodes (4): App::getInstance(), App::getRenderer(), App::getSceneStack(), App::getWindow()

### Community 47 - "Architecture Doc Concepts"
Cohesion: 0.23
Nodes (8): Build and Packaging Contract, Current Known Gaps, High-Level Layering, TiledJsonLoader, TRPG Engine Architecture, engine Static Library Target, engine_tests Target, nlohmann_json Dependency

### Community 48 - "UI Primitive Aggregation"
Cohesion: 0.17
Nodes (12): Button, ButtonControl, FocusGroup, HorizontalLayout, IFocusable, Insets, MenuPanel, Slider (+4 more)

### Community 49 - "FileWatcher Debounce Impl"
Cohesion: 0.23
Nodes (7): clampDebounce(), FileWatcher::FileWatcher(), FileWatcher::getDebounce(), FileWatcher::setChangeCallback(), FileWatcher::setDebounce(), FileWatcher::snapshotDirectory(), FileWatcher::snapshotFile()

### Community 50 - "HotReloadBus Subscription Ids"
Cohesion: 0.21
Nodes (4): HotReloadBus::findNextFreeId(), HotReloadBus::snapshotHandlers(), HotReloadBus::subscribe(), HotReloadBus::unsubscribe()

### Community 51 - "SliderControl Adapter"
Cohesion: 0.17
Nodes (4): SliderControl, m_selected, m_slider, m_step

### Community 52 - "UIAnimation Sequence"
Cohesion: 0.26
Nodes (3): UIAnimation, UIAnimationTrack, m_animations

### Community 53 - "VerticalLayout Config"
Cohesion: 0.18
Nodes (10): apply(), HorizontalAlignment, Center, Left, Right, VerticalLayoutConfig, alignment, padding (+2 more)

### Community 55 - "TileMapData Property Lookup"
Cohesion: 0.24
Nodes (5): findProperty(), TileMapData::findLayer(), TileMapData::tileTypeForGlobalTileId(), TileMapData::tileTypeId(), TileMapData::tileTypeName()

### Community 56 - "Runtime Flow Doc Concepts"
Cohesion: 0.29
Nodes (11): App, GamepadCode, Input, KeyCode, Runtime Flow, Scene, ScreenTransition, StateMachine<T> (+3 more)

### Community 58 - "WindowStartupConfig Header"
Cohesion: 0.18
Nodes (9): Font, Renderer, Scene, StateMachine, Window, WindowStartupConfig, borderless, height (+1 more)

### Community 59 - "Isometric Tile Math"
Cohesion: 0.22
Nodes (7): isoDirectionForTileDelta(), isoToTile(), manhattanDistance(), tileToIso(), Camera::clampToBounds(), Camera::rebuildBoundsCache(), Camera::tileToScreen()

### Community 60 - "Rect Geometry"
Cohesion: 0.27
Nodes (5): Rect, h, w, x, y

### Community 61 - "Slider Range Clamping"
Cohesion: 0.25
Nodes (6): clamp(), Slider::handleDrag(), Slider::setRange(), Slider::setTrackRect(), Slider::setValue(), Slider::step()

### Community 62 - "FileWatcher Timestamp State"
Cohesion: 0.20
Nodes (7): WatchEntry, fileLastNotifications, fileTimestamps, isDirectory, lastNotification, path, recursive

### Community 63 - "FileWatcher Pending Events"
Cohesion: 0.27
Nodes (6): PendingEvent, path, type, FileWatcher::isDebounceReady(), FileWatcher::pollDirectoryEntry(), FileWatcher::pollFileEntry()

### Community 64 - "Vec2 Headers"
Cohesion: 0.24
Nodes (3): Vec2, x, y

### Community 65 - "FollowEasing Curve Kinds"
Cohesion: 0.20
Nodes (10): FollowEasing, Bounce, EaseIn, EaseInCubic, EaseInOut, EaseInOutCubic, EaseOut, EaseOutCubic (+2 more)

### Community 66 - "Anchor Enums"
Cohesion: 0.20
Nodes (10): Anchor, Bottom, BottomLeft, BottomRight, Center, Left, Right, Top (+2 more)

### Community 67 - "Animation Concept Aggregation"
Cohesion: 0.33
Nodes (9): AnimationState, Animator, Camera, IsoDirection, MathUtils, Rect, TimedState, Tween<T> (+1 more)

### Community 68 - "Renderer Subsystem Facade"
Cohesion: 0.25
Nodes (9): BlendMode, DebugRenderer, Font, FontManager, Renderer, SpriteBatch, TextLabel, Texture (+1 more)

### Community 70 - "ScreenTransition Config"
Cohesion: 0.22
Nodes (6): Config, color, duration, easing, onComplete, transition

### Community 71 - "FColor Type"
Cohesion: 0.22
Nodes (5): FColor, a, b, g, r

### Community 73 - "TextBlock Line Layout"
Cohesion: 0.25
Nodes (7): Font, Line, bold, color, font, text, Renderer

### Community 74 - "Scoped Font State"
Cohesion: 0.28
Nodes (6): ScopedFontSize, m_font, m_previousSize, ScopedFontStyle, m_font, m_previousStyle

### Community 76 - "Tiled Object Schema"
Cohesion: 0.25
Nodes (7): MapObject, className, id, isPoint, name, x, y

### Community 77 - "Texture Resource"
Cohesion: 0.25
Nodes (4): Texture, m_height, m_texture, m_width

### Community 79 - "IRowControl Interface"
Cohesion: 0.25
Nodes (3): Font, IRowControl, Renderer

### Community 80 - "Insets Padding"
Cohesion: 0.25
Nodes (3): MenuPanel::clearLayout(), MenuPanel::setPosition(), MenuPanel::setVerticalLayout()

### Community 84 - "Data Module Aggregation"
Cohesion: 0.29
Nodes (7): FileWatcher, HotReloadBus, PropertyId, PropertyRegistry, TileClass, TileLayer, TileMapData

### Community 85 - "FileWatcher Registration"
Cohesion: 0.43
Nodes (4): FileWatcher::unwatch(), FileWatcher::watchDirectory(), FileWatcher::watchFile(), normalisePath()

### Community 86 - "Timer Frame Pacing"
Cohesion: 0.29
Nodes (4): Timer, m_frequency, m_lastCounter, m_startCounter

### Community 87 - "Window Handle Impl"
Cohesion: 0.29
Nodes (6): DisplayResolution, height, width, SDL_Renderer, SDL_Window, Window::GetPrimaryDesktopResolution()

### Community 89 - "VSyncMode Config"
Cohesion: 0.29
Nodes (6): VSyncMode, Adaptive, Disabled, Enabled, Window::setVSync(), Window::Window()

### Community 90 - "Font Rendering"
Cohesion: 0.29
Nodes (4): Font, m_font, m_size, Renderer

### Community 91 - "Tile Rotation Enums"
Cohesion: 0.33
Nodes (5): Rotation, R0, R180, R270, R90

### Community 92 - "LetterboxTransform Struct"
Cohesion: 0.33
Nodes (6): LetterboxTransform, offsetX, offsetY, scaleX, scaleY, Renderer::computeLetterboxTransform()

### Community 94 - "Slider RenderStyle"
Cohesion: 0.33
Nodes (5): RenderStyle, handleHeight, handleWidth, offsetY, trackHeight

### Community 95 - "vcpkg Manifest"
Cohesion: 0.33
Nodes (5): builtin-baseline, dependencies, name, $schema, version

### Community 96 - "IsoDirection Enum"
Cohesion: 0.40
Nodes (5): IsoDirection, NorthEast, NorthWest, SouthEast, SouthWest

### Community 97 - "Animator Transition Struct"
Cohesion: 0.50
Nodes (4): Transition, from, to, trigger

### Community 98 - "FileWatcher ChangeType"
Cohesion: 0.50
Nodes (4): ChangeType, Added, Modified, Removed

### Community 99 - "Vertex Struct"
Cohesion: 0.50
Nodes (3): Vertex, color, position

### Community 101 - "TextWrap Header"
Cohesion: 0.50
Nodes (3): Font, Renderer, TextWrap

### Community 107 - "LoggedKey Struct"
Cohesion: 0.67
Nodes (3): LoggedKey, code, name

## Ambiguous Edges - Review These
- `Font` → `engine Static Library Target`  [AMBIGUOUS]
  CMakeLists.txt · relation: references
- `Scene` → `engine Static Library Target`  [AMBIGUOUS]
  CMakeLists.txt · relation: references

## Knowledge Gaps
- **404 isolated node(s):** `from`, `to`, `trigger`, `m_states`, `m_transitions` (+399 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 821 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Font` and `engine Static Library Target`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **Why does `Color` connect `Color Type` to `Button Widget API`, `Text Animation Timing`, `Renderer Draw Passes`, `MenuPanel Composition`, `Logging Facility`, `Presentation Mode Enums`, `ValueControl Abstraction`, `ButtonControl Adapter`, `Text Alignment Enums`, `BlendMode SDL Conversion`, `SpriteBatch DrawCommand`, `DebugRenderer Line Drawing`, `ScreenTransition State Machine`, `Text Width Measurement`, `DebugRenderer LineCommand`, `Letterbox Transform Math`, `Scene Lifecycle Dispatch`, `Core Data Headers`, `FColor Type`, `TextBlock Line Layout`, `Widget Header Includes`, `IRowControl Interface`, `SpriteBatch Impl`, `App Event Loop`?**
  _High betweenness centrality (0.142) - this node is a cross-community bridge._
- **What connects `from`, `to`, `trigger` to the rest of the system?**
  _404 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Button Widget API` be split into smaller, more focused modules?**
  _Cohesion score 0.050505050505050504 - nodes in this community are weakly interconnected._
- **What is the exact relationship between `Scene` and `engine Static Library Target`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **Why does `Camera` connect `Camera Public Interface` to `Vec2 Headers`, `FollowEasing Curve Kinds`, `Camera Tile Transform`, `Isometric Tile Math`, `Tile Rotation Enums`?**
  _High betweenness centrality (0.091) - this node is a cross-community bridge._
- **Should `Camera Public Interface` be split into smaller, more focused modules?**
  _Cohesion score 0.049682875264270614 - nodes in this community are weakly interconnected._