# lib/CMakeLists.txt
# Manages all third-party dependencies.
#
# GoogleTest is expected to live in lib/googletest/.
# If the directory is absent the build will attempt to fetch it via FetchContent
# so developers don't have to clone the submodule manually.

include(FetchContent)

set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest)

if(EXISTS ${GTEST_DIR}/CMakeLists.txt)
    message(STATUS "GoogleTest: using local copy at ${GTEST_DIR}")
    # Prevent GoogleTest from overriding our compiler/linker options on Windows.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/lib/googletest)
else()
    message(STATUS "GoogleTest: local copy not found — fetching from GitHub")
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        SOURCE_DIR     ${GTEST_DIR}   # cache into lib/googletest for subsequent builds
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Suppress GoogleTest's own install rules so they don't pollute `make install`.
set_target_properties(gtest gtest_main gmock gmock_main
    PROPERTIES EXCLUDE_FROM_ALL TRUE
)