cmake_minimum_required(VERSION 3.16)
project(variant_bst CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bst_lib src/bst.cpp)
target_include_directories(bst_lib PUBLIC include)

add_executable(bst_demo src/main.cpp)
target_link_libraries(bst_demo PRIVATE bst_lib)

enable_testing()
add_executable(bst_tests tests/bst_tests.cpp)
target_link_libraries(bst_tests PRIVATE bst_lib)
add_test(NAME bst_tests COMMAND bst_tests)
