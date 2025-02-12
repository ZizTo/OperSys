#include <gtest/gtest.h>
#include "Fib.h"

TEST(FibonacciTest, FirstNFibonacci) {
    Fibonacci fib;
    std::vector<int> expected = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
    EXPECT_EQ(fib.getFirstNFibonacci(10), expected);
}

// “ест дл€ проверки обработки ошибок
TEST(FibonacciTest, NegativeInput) {
    Fibonacci fib;
    EXPECT_THROW(fib.getFirstNFibonacci(-1), std::invalid_argument);
}

// “ест дл€ проверки пустой последовательности
TEST(FibonacciTest, ZeroInput) {
    Fibonacci fib;
    std::vector<int> expected = {};
    EXPECT_EQ(fib.getFirstNFibonacci(0), expected);
}