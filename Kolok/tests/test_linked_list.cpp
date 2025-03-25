/**
 * @file test_linked_list.cpp
 * @brief Тесты для класса LinkedList
 */

#include <gtest/gtest.h>
#include "linked_list/linked_list.hpp"
#include <vector>
#include <string>

using namespace data_structures;

// Тест на создание списка и базовые операции
TEST(LinkedListTest, BasicOperations) {
    LinkedList<int> list;
    
    // Проверка пустого списка
    EXPECT_TRUE(list.isEmpty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.toVector().empty());
    
    // Добавление элементов в конец
    list.append(1);
    list.append(2);
    list.append(3);
    
    EXPECT_FALSE(list.isEmpty());
    EXPECT_EQ(list.size(), 3);
    
    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(list.toVector(), expected);
    
    // Добавление элементов в начало
    list.prepend(0);
    expected = {0, 1, 2, 3};
    EXPECT_EQ(list.toVector(), expected);
    EXPECT_EQ(list.size(), 4);
    
    // Очистка списка
    list.clear();
    EXPECT_TRUE(list.isEmpty());
    EXPECT_EQ(list.size(), 0);
}

// Тест на создание списка из вектора
TEST(LinkedListTest, CreateFromVector) {
    std::vector<int> values = {1, 2, 3, 4, 5};
    
    // Создание через конструктор
    LinkedList<int> list1(values);
    EXPECT_EQ(list1.toVector(), values);
    
    // Создание через статический метод
    LinkedList<int> list2 = LinkedList<int>::fromVector(values);
    EXPECT_EQ(list2.toVector(), values);
}

// Тест на разворот списка рекурсивно
TEST(LinkedListTest, ReverseRecursively) {
    // Проверка пустого списка
    LinkedList<int> emptyList;
    emptyList.reverseRecursively();
    EXPECT_TRUE(emptyList.isEmpty());
    
    // Проверка списка из одного элемента
    LinkedList<int> singleElementList;
    singleElementList.append(42);
    singleElementList.reverseRecursively();
    EXPECT_EQ(singleElementList.toVector(), std::vector<int>({42}));
    
    // Проверка списка из нескольких элементов
    LinkedList<int> multiElementList = LinkedList<int>::fromVector({1, 2, 3, 4, 5});
    multiElementList.reverseRecursively();
    EXPECT_EQ(multiElementList.toVector(), std::vector<int>({5, 4, 3, 2, 1}));
    
    // Проверка списка с повторяющимися элементами
    LinkedList<int> repeatingElementsList = LinkedList<int>::fromVector({1, 2, 2, 3, 1});
    repeatingElementsList.reverseRecursively();
    EXPECT_EQ(repeatingElementsList.toVector(), std::vector<int>({1, 3, 2, 2, 1}));
}

// Тест на работу с разными типами данных
TEST(LinkedListTest, DifferentDataTypes) {
    // Список строк
    LinkedList<std::string> stringList;
    stringList.append("Hello");
    stringList.append("World");
    stringList.append("C++");
    
    stringList.reverseRecursively();
    std::vector<std::string> expectedStrings = {"C++", "World", "Hello"};
    EXPECT_EQ(stringList.toVector(), expectedStrings);
    
    // Список с типом double
    LinkedList<double> doubleList = LinkedList<double>::fromVector({1.1, 2.2, 3.3, 4.4});
    doubleList.reverseRecursively();
    std::vector<double> expectedDoubles = {4.4, 3.3, 2.2, 1.1};
    EXPECT_EQ(doubleList.toVector(), expectedDoubles);
}

// Тест на многократный разворот списка
TEST(LinkedListTest, MultipleReversals) {
    LinkedList<int> list = LinkedList<int>::fromVector({1, 2, 3, 4, 5});
    std::vector<int> original = list.toVector();
    
    // Первый разворот
    list.reverseRecursively();
    std::vector<int> reversed = {5, 4, 3, 2, 1};
    EXPECT_EQ(list.toVector(), reversed);
    
    // Второй разворот (должен вернуть к исходному порядку)
    list.reverseRecursively();
    EXPECT_EQ(list.toVector(), original);
    
    // Третий разворот
    list.reverseRecursively();
    EXPECT_EQ(list.toVector(), reversed);
}

// Тест производительности (бенчмарк)
TEST(LinkedListTest, PerformanceBenchmark) {
    // В реальном бенчмарке мы бы использовали Google Benchmark или аналог
    // но для простоты просто проверим, что функция работает с большим списком
    
    const int size = 1000;
    std::vector<int> largeVector;
    largeVector.reserve(size);
    
    for (int i = 0; i < size; ++i) {
        largeVector.push_back(i);
    }
    
    LinkedList<int> largeList = LinkedList<int>::fromVector(largeVector);
    
    // Проверяем, что функция разворота работает без ошибок на большом списке
    EXPECT_NO_THROW(largeList.reverseRecursively());
    
    // Проверяем результат (должен быть обратный порядок)
    std::vector<int> reversed = largeList.toVector();
    EXPECT_EQ(reversed.size(), size);
    EXPECT_EQ(reversed.front(), size - 1);
    EXPECT_EQ(reversed.back(), 0);
}
