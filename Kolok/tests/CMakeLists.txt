# Находим Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

# Определяем исходные файлы тестов
set(TEST_SOURCES
    test_factorial.cpp
    test_duplicate_remover.cpp
    test_linked_list.cpp
)

# Создаем исполняемый файл для тестов
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests
    PRIVATE
    gtest_main
    factorial
    unique
    linked_list
)

# Добавляем тесты в CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)
