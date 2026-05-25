cmake_minimum_required(VERSION 4.3.0)
project(exchange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(lib)
add_subdirectory(src)

option(BUILD_TESTS "Build test tarets" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
option(BUILD_TESTS "Build test targets" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()