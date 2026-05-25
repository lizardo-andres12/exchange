# test/containers/CMakeLists.txt
# Tests for the 'containers' library.

# add_gtest() is defined in the parent (test/CMakeLists.txt) and is visible here.
add_gtest(
    TARGET  order_book_test
    SOURCES order_book_test.cc
    DEPS    containers
)