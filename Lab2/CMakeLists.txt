cmake_minimum_required(VERSION 3.14) 
project(Lab2 CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Main98 Lab2/Lab2.cpp)
target_include_directories(Main98 
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Lab2 
)

enable_testing()
add_executable(LabTests98 Tests/tests.cpp)
target_include_directories(LabTests98
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Tests
		${CMAKE_CURRENT_SOURCE_DIR}/Lab2
)
target_link_libraries(LabTests98 gtest_main)


add_test(NAME LabTests98 COMMAND LabTests98)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(Main14 Lab2/Lab2.cpp)
target_include_directories(Main14 
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Lab2 
)

enable_testing()
add_executable(LabTests14 Tests/tests.cpp)
target_include_directories(LabTests14 
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Tests
		${CMAKE_CURRENT_SOURCE_DIR}/Lab2
)
target_link_libraries(LabTests14 gtest_main)


add_test(NAME LabTests14 COMMAND LabTests14)