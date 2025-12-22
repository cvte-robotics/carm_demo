#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Poco::Redis" for configuration "Release"
set_property(TARGET Poco::Redis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Poco::Redis PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libPocoRedis.so.71"
  IMPORTED_SONAME_RELEASE "libPocoRedis.so.71"
  )

list(APPEND _IMPORT_CHECK_TARGETS Poco::Redis )
list(APPEND _IMPORT_CHECK_FILES_FOR_Poco::Redis "${_IMPORT_PREFIX}/lib/libPocoRedis.so.71" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
