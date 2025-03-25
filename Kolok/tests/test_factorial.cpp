/**
 * @file test_factorial.cpp
 * @brief Тесты для класса FactorialCalculator
 */

#include <gtest/gtest.h>
#include "factorial/factorial_calculator.hpp"

using namespace math;

// Тест на вычисление факториала
TEST(FactorialCalculatorTest, CalculateFactorial) {
    // Базовые случаи
    EXPECT_EQ(FactorialCalculator<>::calculateFactorial(0), 1);
    EXPECT_EQ(FactorialCalculator<>::calculateFactorial(1), 1);
    EXPECT_EQ(FactorialCalculator<>::calculateFactorial(5), 120);
    EXPECT_EQ(FactorialCalculator<>::calculateFactorial(10), 3628800);
    
    // Проверка на отрицательное число
    EXPECT_THROW(FactorialCalculator<>::calculateFactorial(-1), std::invalid_argument);
    
    // Проверка на переполнение с использованием меньшего типа
    EXPECT_THROW(FactorialCalculator<unsigned int>::calculateFactorial(13), std::overflow_error);
}

// Тест на вычисление первых n факториалов
TEST(FactorialCalculatorTest, CalculateFirstNFactorials) {
    // Проверка на отрицательное число
    EXPECT_THROW(FactorialCalculator<>::calculateFirstNFactorials(-1), std::invalid_argument);
    
    // Проверка пустого результата
    EXPECT_TRUE(FactorialCalculator<>::calculateFirstNFactorials(0).empty());
    
    // Проверка результата с одним элементом
    std::vector<unsigned long long> expected1 = {1};
    EXPECT_EQ(FactorialCalculator<>::calculateFirstNFactorials(1), expected1);
    
    // Проверка корректности вычисления для n = 6
    std::vector<unsigned long long> expected6 = {1, 1, 2, 6, 24, 120};
    EXPECT_EQ(FactorialCalculator<>::calculateFirstNFactorials(6), expected6);
    
    // Проверка на переполнение с использованием меньшего типа
    EXPECT_THROW(FactorialCalculator<unsigned int>::calculateFirstNFactorials(13), std::overflow_error);
}

// Тест с использованием пользовательского типа
TEST(FactorialCalculatorTest, CustomType) {
    // В этом тесте мы могли бы использовать пользовательский тип для больших чисел
    // например, boost::multiprecision::cpp_int
    // но для простоты ограничимся стандартными типами
    
    // Проверка совместимости с типом long double
    EXPECT_EQ(FactorialCalculator<long double>::calculateFactorial(5), 120.0L);
    
    std::vector<long double> expected = {1.0L, 1.0L, 2.0L, 6.0L, 24.0L};
    EXPECT_EQ(FactorialCalculator<long double>::calculateFirstNFactorials(5), expected);
}

// Тест производительности (бенчмарк)
TEST(FactorialCalculatorTest, PerformanceBenchmark) {
    // Этот тест проверяет, что вычисление большого количества факториалов
    // выполняется за разумное время
    
    // В реальном бенчмарке мы бы использовали Google Benchmark или аналог
    // но для простоты просто проверим, что функция выполняется без ошибок
    
    // Вычисление первых 20 факториалов типа unsigned long long
    EXPECT_NO_THROW(FactorialCalculator<>::calculateFirstNFactorials(20));
}
