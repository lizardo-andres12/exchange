# src/containers/CMakeLists.txt
# Builds the 'containers' static library (order_book, etc.).

add_library(containers STATIC
    order_book.cc
)

target_include_directories(containers
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include          # exposes include/containers/*.h to consumers
)

# Link any containers-specific dependencies here, e.g.:
# target_link_libraries(containers PUBLIC some_dep)