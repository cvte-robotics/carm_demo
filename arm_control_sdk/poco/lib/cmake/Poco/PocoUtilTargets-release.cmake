#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Poco::Util" for configuration "Release"
set_property(TARGET Poco::Util APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Poco::Util PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libPocoUtil.so.71"
  IMPORTED_SONAME_RELEASE "libPocoUtil.so.71"
  )

list(APPEND _IMPORT_CHECK_TARGETS Poco::Util )
list(APPEND _IMPORT_CHECK_FILES_FOR_Poco::Util "${_IMPORT_PREFIX}/lib/libPocoUtil.so.71" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
