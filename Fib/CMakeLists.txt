cmake_minimum_required(VERSION 3.14) 
project(FibonacciProject CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(FibonacciMain Fib/main.cpp Fib/Fib.cpp)
target_include_directories(FibonacciMain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Fib)

enable_testing()
add_executable(FibonacciTests tests/FibTest.cpp Fib/Fib.cpp)
target_include_directories(FibonacciTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Fib)
target_link_libraries(FibonacciTests gtest_main)


add_test(NAME FibonacciTests COMMAND FibonacciTests)