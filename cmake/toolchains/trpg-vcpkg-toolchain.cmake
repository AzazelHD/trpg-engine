# Resolve vcpkg toolchain in a portable way for TRPG workspaces.
# Priority:
# 1) TRPG_VCPKG_ROOT environment variable
# 2) Workspace-local .vcpkg next to engine/game_1
# 3) Drive-root .vcpkg on the workspace drive (e.g. H:/.vcpkg)
# 4) VCPKG_ROOT environment variable

set(_trpg_vcpkg_root "")

if(DEFINED ENV{TRPG_VCPKG_ROOT} AND EXISTS "$ENV{TRPG_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(_trpg_vcpkg_root "$ENV{TRPG_VCPKG_ROOT}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../.vcpkg/scripts/buildsystems/vcpkg.cmake")
    # engine/cmake/toolchains -> workspace root sibling that contains .vcpkg
    set(_trpg_vcpkg_root "${CMAKE_CURRENT_LIST_DIR}/../../../.vcpkg")
else()
    # Fallback for setups using a drive-root vcpkg, e.g. H:/.vcpkg.
    string(SUBSTRING "${CMAKE_CURRENT_LIST_DIR}" 0 2 _trpg_source_drive)
    if(EXISTS "${_trpg_source_drive}/.vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(_trpg_vcpkg_root "${_trpg_source_drive}/.vcpkg")
    endif()
endif()

if(NOT _trpg_vcpkg_root AND DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(_trpg_vcpkg_root "$ENV{VCPKG_ROOT}")
endif()

if(NOT _trpg_vcpkg_root)
    message(FATAL_ERROR
        "Unable to locate vcpkg toolchain. Set TRPG_VCPKG_ROOT or VCPKG_ROOT, or place .vcpkg at the workspace root."
    )
endif()

include("${_trpg_vcpkg_root}/scripts/buildsystems/vcpkg.cmake")
