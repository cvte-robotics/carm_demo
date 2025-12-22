
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was arm_control_sdkConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
set (arm_control_sdk_FOUND 1)
set (arm_control_sdk_VERSION 1.0.1)
set (arm_control_sdk_DEFINITIONS  "")
set (arm_control_sdk_INCLUDE_DIR  "${PACKAGE_PREFIX_DIR}/include")
set (arm_control_sdk_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include")
set (arm_control_sdk_ROOT_DIR     "${PACKAGE_PREFIX_DIR}")

set (arm_control_sdk_LIBRARIES
    "${PACKAGE_PREFIX_DIR}/lib/libarm_control_sdk.so"
    "${PACKAGE_PREFIX_DIR}/lib/libcarm_poco_net.so"
    "${PACKAGE_PREFIX_DIR}/lib/libtcp_com.so"
    "${PACKAGE_PREFIX_DIR}/lib/libmlog.so"
)
