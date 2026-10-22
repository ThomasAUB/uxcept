cmake_minimum_required(VERSION 3.13)            # CMake version check
project(uXcept VERSION 0.0.1 LANGUAGES CXX)     # Create project
set(CMAKE_CXX_STANDARD 17)                      # Enable c++17 standard

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/uatom/CMakeLists.txt")
    message(FATAL_ERROR "The uatom submodule was not downloaded, please run git submodule update --init --recursive.")
endif()

# Header-only library target
add_library(uxcept INTERFACE)
target_include_directories(uxcept INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(uxcept INTERFACE cxx_std_17)

# a parent project may already provide the uatom target
if(NOT TARGET uatom)
    add_subdirectory(uatom)
endif()

target_link_libraries(uxcept INTERFACE uatom)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# only build the tests when uxcept is the top level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    add_subdirectory(tests)
endif()
