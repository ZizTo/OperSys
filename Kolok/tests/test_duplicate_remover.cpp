/**
 * @file test_duplicate_remover.cpp
 * @brief Тесты для класса DuplicateRemover
 */

#include <gtest/gtest.h>
#include "unique/duplicate_remover.hpp"
#include <vector>
#include <list>
#include <deque>
#include <string>

using namespace containers;

// Тест на удаление дубликатов из вектора целых чисел
TEST(DuplicateRemoverTest, RemoveDuplicatesFromVectorInt) {
    // Проверка пустого вектора
    std::vector<int> emptyVec;
    EXPECT_TRUE(DuplicateRemover::removeDuplicates(emptyVec).empty());
    
    // Проверка вектора без дубликатов
    std::vector<int> noDuplicatesVec = {1, 2, 3, 4, 5};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(noDuplicatesVec), noDuplicatesVec);
    
    // Проверка вектора с дубликатами
    std::vector<int> withDuplicatesVec = {1, 2, 3, 2, 1, 4, 5, 4};
    std::vector<int> expectedVec = {1, 2, 3, 4, 5};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(withDuplicatesVec), expectedVec);
    
    // Проверка вектора с одинаковыми элементами
    std::vector<int> sameElementsVec = {1, 1, 1, 1};
    std::vector<int> expectedSameVec = {1};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(sameElementsVec), expectedSameVec);
}

// Тест на удаление дубликатов из вектора строк
TEST(DuplicateRemoverTest, RemoveDuplicatesFromVectorString) {
    std::vector<std::string> withDuplicatesVec = {"apple", "banana", "apple", "orange", "banana", "grape"};
    std::vector<std::string> expectedVec = {"apple", "banana", "orange", "grape"};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(withDuplicatesVec), expectedVec);
}

// Тест на удаление дубликатов из списка
TEST(DuplicateRemoverTest, RemoveDuplicatesFromList) {
    std::list<int> withDuplicatesList = {1, 2, 3, 2, 1, 4, 5, 4};
    std::list<int> expectedList = {1, 2, 3, 4, 5};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(withDuplicatesList), expectedList);
}

// Тест на удаление дубликатов из двусторонней очереди
TEST(DuplicateRemoverTest, RemoveDuplicatesFromDeque) {
    std::deque<int> withDuplicatesDeque = {1, 2, 3, 2, 1, 4, 5, 4};
    std::deque<int> expectedDeque = {1, 2, 3, 4, 5};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(withDuplicatesDeque), expectedDeque);
}

// Тест на использование функции с итераторами
TEST(DuplicateRemoverTest, RemoveDuplicatesWithIterators) {
    std::vector<int> input = {1, 2, 3, 2, 1, 4, 5, 4};
    std::vector<int> output;
    
    DuplicateRemover::removeDuplicates(input.begin(), input.end(), std::back_inserter(output));
    
    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(output, expected);
}

// Тест на правильный порядок элементов после удаления дубликатов
TEST(DuplicateRemoverTest, PreserveOrderAfterRemovingDuplicates) {
    std::vector<int> input = {5, 2, 9, 2, 7, 5, 3, 1, 9};
    std::vector<int> expected = {5, 2, 9, 7, 3, 1};
    EXPECT_EQ(DuplicateRemover::removeDuplicates(input), expected);
}

// Тест производительности (бенчмарк)
TEST(DuplicateRemoverTest, PerformanceBenchmark) {
    // В реальном бенчмарке мы бы использовали Google Benchmark или аналог
    // но для простоты просто проверим, что функция работает с большим объемом данных
    
    std::vector<int> largeVector;
    const int size = 10000;
    largeVector.reserve(size);
    
    // Заполняем вектор, чтобы в нем было много дубликатов
    for (int i = 0; i < size; ++i) {
        largeVector.push_back(i % 100);
    }
    
    // Проверяем, что функция работает без ошибок на большом наборе данных
    EXPECT_NO_THROW(DuplicateRemover::removeDuplicates(largeVector));
    
    // Проверяем размер результата (должно быть 100 уникальных элементов)
    auto result = DuplicateRemover::removeDuplicates(largeVector);
    EXPECT_EQ(result.size(), 100);
}
