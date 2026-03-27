cmake_minimum_required(VERSION 3.14)
project(carm_py LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########### 查找依赖 ###########
set(PYTHON_VERSION "" CACHE STRING "Python version (e.g., 3.12). Empty for auto-detect")
if(PYTHON_VERSION)
    # 尝试查找指定版本的 Python
    find_package(Python ${PYTHON_VERSION} EXACT COMPONENTS Interpreter Development)
    if(NOT Python_FOUND)
        message(FATAL_ERROR "Could not find Python ${PYTHON_VERSION}")
    endif()
else()
    # 未指定版本，自动查找
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    set(PYTHON_VERSION ${Python_VERSION} CACHE STRING "Python version" FORCE)
endif()

message(STATUS "Using Python version: ${PYTHON_VERSION}")
set(CMAKE_INSTALL_PREFIX 
    "${CMAKE_CURRENT_SOURCE_DIR}/../carm_py"
)
# 先查找旧版 Python 组件（会被 pybind11 使用）
find_package(PythonInterp ${PYTHON_VERSION} EXACT REQUIRED)
find_package(PythonLibs ${PYTHON_VERSION} EXACT REQUIRED)
# 再查找新版 Python 组件（供您的代码使用）
find_package(Python ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

########### 设置依赖路径 ###########
if (WIN32)
    # if CMAKE_GENERATOR_PLATFORM is not set, set it to x64
    if(NOT CMAKE_GENERATOR_PLATFORM)
        set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "Build type" FORCE)
    endif()
    # if CMAKE_GENERATOR_PLATFORM is Win32, set it to x86
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(CMAKE_GENERATOR_PLATFORM x86)
    endif()
    set(arm_control_sdk_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../include/")
    set(arm_control_sdk_LIBRARIES
        "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_GENERATOR_PLATFORM}/Debug/arm_control_sdkd.lib"
    )
else()
    set(arm_control_sdk_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../include/")
    set(arm_control_sdk_LIBRARIES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libarm_control_sdk.so"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libcarm_poco_net.so"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libtcp_com.so"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libmlog.so"
    )

endif()

########### 打印 CMAKE 变量 ###########
message("======================================================")
message("PYTHON_VERSION:                ${PYTHON_VERSION}")
message("pybind11_VERSION:              ${pybind11_VERSION}")
message("CMAKE_GENERATOR_PLATFORM:      ${CMAKE_GENERATOR_PLATFORM}")
message("CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message("CMAKE_INSTALL_PREFIX:          ${CMAKE_INSTALL_PREFIX}")
message("CMAKE_PREFIX_PATH:             ${CMAKE_PREFIX_PATH}")
message("CMAKE_CURRENT_SOURCE_DIR:      ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("arm_control_sdk_INCLUDE_DIRS:  ${arm_control_sdk_INCLUDE_DIRS}")
message("arm_control_sdk_LIBRARIES:     ${arm_control_sdk_LIBRARIES}")
message("======================================================")

########### 构建 pybind11 模块 ###########
pybind11_add_module(${PROJECT_NAME} ./carm_py.cpp)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${arm_control_sdk_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${arm_control_sdk_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ""
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

########### Python 节点文件安装 ###########
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../carm_py
)