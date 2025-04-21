cmake_minimum_required(VERSION 3.14) 
project(Lab4 CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Receiver98 Receiver/Receiver.cpp)
target_include_directories(Receiver98
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Receiver 
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

add_executable(Sender98 Sender/Sender.cpp)
target_include_directories(Sender98
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Sender 
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

enable_testing()
add_executable(LabTests98 Tests/tests.cpp)
target_include_directories(LabTests98
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Tests
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

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

add_executable(Receiver14 Receiver14/Receiver.cpp)
target_include_directories(Receiver14
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Receiver14
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

add_executable(Sender14 Sender14/Sender.cpp)
target_include_directories(Sender14
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Sender14
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

enable_testing()
add_executable(LabTests14 Tests/tests.cpp)
target_include_directories(LabTests14
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Tests
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

add_test(NAME LabTests14 COMMAND LabTests14)