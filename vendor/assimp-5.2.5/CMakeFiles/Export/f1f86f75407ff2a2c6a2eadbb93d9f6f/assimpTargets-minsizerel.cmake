#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "assimp::assimp" for configuration "MinSizeRel"
set_property(TARGET assimp::assimp APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(assimp::assimp PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/assimp-vc143-mt.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/assimp-vc143-mt.dll"
  )

list(APPEND _cmake_import_check_targets assimp::assimp )
list(APPEND _cmake_import_check_files_for_assimp::assimp "${_IMPORT_PREFIX}/lib/assimp-vc143-mt.lib" "${_IMPORT_PREFIX}/bin/assimp-vc143-mt.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
