# src/CMakeLists.txt
# Builds all production library targets and the main executable.

add_subdirectory(containers)

add_executable(exchange
    main.cc
)

target_include_directories(exchange
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(exchange
    PRIVATE
        containers
)