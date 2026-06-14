#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TRPG::engine" for configuration "MinSizeRel"
set_property(TARGET TRPG::engine APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(TRPG::engine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/engine.lib"
  )

list(APPEND _cmake_import_check_targets TRPG::engine )
list(APPEND _cmake_import_check_files_for_TRPG::engine "${_IMPORT_PREFIX}/lib/engine.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
