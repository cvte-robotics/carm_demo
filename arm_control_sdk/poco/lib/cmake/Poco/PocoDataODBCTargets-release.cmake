#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Poco::DataODBC" for configuration "Release"
set_property(TARGET Poco::DataODBC APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Poco::DataODBC PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libPocoDataODBC.so.71"
  IMPORTED_SONAME_RELEASE "libPocoDataODBC.so.71"
  )

list(APPEND _IMPORT_CHECK_TARGETS Poco::DataODBC )
list(APPEND _IMPORT_CHECK_FILES_FOR_Poco::DataODBC "${_IMPORT_PREFIX}/lib/libPocoDataODBC.so.71" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
