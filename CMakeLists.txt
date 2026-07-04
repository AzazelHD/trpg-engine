cmake_minimum_required(VERSION 3.20)

project(TRPGEngine VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# =========================
# Dependencies (vcpkg)
# =========================
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# =========================
# Engine library
# =========================
add_library(engine STATIC
    src/engine/assets/FileWatcher.cpp
    src/engine/assets/HotReloadBus.cpp
    src/engine/core/App.cpp
    src/engine/core/Log.cpp
    src/engine/core/Timer.cpp
    src/engine/core/Window.cpp
    src/engine/data/TileMapData.cpp
    src/engine/data/TiledJsonLoader.cpp
    src/engine/input/Input.cpp
    src/engine/renderer/Font.cpp
    src/engine/renderer/Camera.cpp
    src/engine/renderer/DebugRenderer.cpp
    src/engine/renderer/SpriteBatch.cpp
    src/engine/renderer/Texture.cpp
    src/engine/renderer/TileLayer.cpp
    src/engine/renderer/Renderer.cpp
    src/engine/effects/ScreenTransition.cpp
    src/engine/ui/Button.cpp
    src/engine/ui/MenuPanel.cpp
    src/engine/ui/Slider.cpp
    src/engine/ui/TextLabel.cpp
    include/engine/world/WorldGraph.h
)

add_library(TRPG::engine ALIAS engine)

# =========================
# Include dirs
# =========================

set_target_properties(engine PROPERTIES
    DEBUG_POSTFIX "d"
)

target_include_directories(engine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# =========================
# Link libraries
# =========================
target_link_libraries(engine
    PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
    PRIVATE
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf
        nlohmann_json::nlohmann_json
)

target_compile_features(engine PUBLIC cxx_std_20)

target_compile_options(engine PRIVATE /W4 /WX)

set_target_properties(engine PROPERTIES
    EXPORT_NAME engine
)

# =========================
# Install target
# =========================
install(TARGETS engine
    EXPORT TRPGEngineTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# =========================
# Export targets
# =========================
install(EXPORT TRPGEngineTargets
    NAMESPACE TRPG::
    FILE TRPGEngineTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TRPGEngine
)

# Also export targets to the build tree so game DEV builds can consume
# TRPGEngine via find_package(TRPGEngine) without requiring install().
export(EXPORT TRPGEngineTargets
    NAMESPACE TRPG::
    FILE ${CMAKE_CURRENT_BINARY_DIR}/TRPGEngineTargets.cmake
)

# =========================
# Package config generation
# =========================
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TRPGEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/TRPGEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TRPGEngine
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/TRPGEngineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# =========================
# Install package files
# =========================
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/TRPGEngineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/TRPGEngineConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TRPGEngine
)