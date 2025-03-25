cmake_minimum_required(VERSION 3.14)
project(CppTasks VERSION 1.0.0 LANGUAGES CXX)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем предупреждения компилятора
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Определяем пути к исходникам и заголовкам
set(HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Определяем исходные файлы для каждого задания
set(FACTORIAL_SOURCES ${SOURCES_DIR}/factorial/factorial_calculator.cpp)
set(UNIQUE_SOURCES ${SOURCES_DIR}/unique/duplicate_remover.cpp)
set(LINKED_LIST_SOURCES ${SOURCES_DIR}/linked_list/linked_list.cpp)

# Создаем библиотеки для каждого задания
add_library(factorial STATIC ${FACTORIAL_SOURCES})
target_include_directories(factorial PUBLIC ${HEADERS_DIR})

add_library(unique STATIC ${UNIQUE_SOURCES})
target_include_directories(unique PUBLIC ${HEADERS_DIR})

add_library(linked_list STATIC ${LINKED_LIST_SOURCES})
target_include_directories(linked_list PUBLIC ${HEADERS_DIR})

# Включаем тесты
enable_testing()
add_subdirectory(tests)
