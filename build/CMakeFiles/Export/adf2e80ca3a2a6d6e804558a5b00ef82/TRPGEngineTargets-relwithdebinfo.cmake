#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TRPG::engine" for configuration "RelWithDebInfo"
set_property(TARGET TRPG::engine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(TRPG::engine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/engine.lib"
  )

list(APPEND _cmake_import_check_targets TRPG::engine )
list(APPEND _cmake_import_check_files_for_TRPG::engine "${_IMPORT_PREFIX}/lib/engine.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
