# test/CMakeLists.txt
# Registers all test suites and wires up GoogleTest/CTest integration.

include(GoogleTest)   # provides gtest_discover_tests()

# Usage:
#   add_gtest(TARGET <name> SOURCES <file...> DEPS <lib...>)
function(add_gtest)
    cmake_parse_arguments(ARG "" "TARGET" "SOURCES;DEPS" ${ARGN})

    add_executable(${ARG_TARGET} ${ARG_SOURCES})

    target_include_directories(${ARG_TARGET}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(${ARG_TARGET}
        PRIVATE
            GTest::gtest_main
            GTest::gmock
            ${ARG_DEPS}
    )

    gtest_discover_tests(${ARG_TARGET})
endfunction()

add_subdirectory(containers)

add_gtest(
    TARGET  status_parser_test
    SOURCES status_parser_test.cc
    # DEPS  status_parser   ← uncomment once that library target exists
)