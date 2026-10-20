cmake_minimum_required(VERSION 3.16)
project(kvstore_cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kvstore_lib
    src/store.cpp
    src/transaction.cpp
)
target_include_directories(kvstore_lib PUBLIC include)

add_executable(kvstore src/main.cpp)
target_link_libraries(kvstore PRIVATE kvstore_lib)

enable_testing()
add_executable(store_tests tests/store_tests.cpp)
target_link_libraries(store_tests PRIVATE kvstore_lib)
add_test(NAME store_tests COMMAND store_tests)
