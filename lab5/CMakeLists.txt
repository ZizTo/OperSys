cmake_minimum_required(VERSION 3.14) 
project(Lab5 CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Client98 Client/clinet.cpp)
target_include_directories(Client98
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Client 
		${CMAKE_CURRENT_SOURCE_DIR}/data
)

add_executable(Server98 Server/server.cpp)
target_include_directories(Server98
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/Server 
		${CMAKE_CURRENT_SOURCE_DIR}/data
)