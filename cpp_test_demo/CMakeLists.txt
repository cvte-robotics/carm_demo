cmake_minimum_required(VERSION 3.14)
project(test_find_pkg LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(arm_control_sdk REQUIRED)

add_executable(carm_demo test_carm_api.cpp)
include_directories(${arm_control_sdk_INCLUDE_DIRS})
target_link_libraries(carm_demo PRIVATE ${arm_control_sdk_LIBRARIES})

add_executable(dual_demo test_carm_dual.cpp)
include_directories(${arm_control_sdk_INCLUDE_DIRS})
target_link_libraries(dual_demo PRIVATE ${arm_control_sdk_LIBRARIES})