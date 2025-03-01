cmake_minimum_required(VERSION 3.14) 
project(Lab2 CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Main Lab2/Lab2.cpp)
target_include_directories(Main 
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Lab2 
)

enable_testing()
add_executable(LabTests Tests/tests.cpp)
target_include_directories(LabTests 
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Tests
		${CMAKE_CURRENT_SOURCE_DIR}/Lab2
)
target_link_libraries(LabTests gtest_main)


add_test(NAME LabTests COMMAND LabTests)