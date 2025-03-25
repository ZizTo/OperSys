/**
 * @file factorial_calculator.cpp
 * @brief Реализация класса FactorialCalculator
 */

#include <limits>

namespace math {

template <typename T>
std::vector<T> FactorialCalculator<T>::calculateFirstNFactorials(int n) {
    // Проверка на отрицательное значение
    if (n < 0) {
        throw std::invalid_argument("Number of factorials cannot be negative");
    }
    
    // Если n = 0, возвращаем пустой вектор
    if (n == 0) {
        return {};
    }
    
    std::vector<T> factorials;
    factorials.reserve(n); // Оптимизация: резервируем память
    
    T currentFactorial = 1;
    factorials.push_back(currentFactorial); // 0! = 1
    
    for (int i = 1; i < n; ++i) {
        // Проверка на переполнение
        if (willOverflow(currentFactorial, static_cast<T>(i))) {
            throw std::overflow_error("Factorial calculation caused overflow at " + std::to_string(i));
        }
        
        currentFactorial *= i;
        factorials.push_back(currentFactorial);
    }
    
    return factorials;
}

template <typename T>
T FactorialCalculator<T>::calculateFactorial(int n) {
    // Проверка на отрицательное значение
    if (n < 0) {
        throw std::invalid_argument("Factorial is not defined for negative numbers");
    }
    
    if (n == 0 || n == 1) {
        return 1;
    }
    
    T result = 1;
    for (int i = 2; i <= n; ++i) {
        // Проверка на переполнение
        if (willOverflow(result, static_cast<T>(i))) {
            throw std::overflow_error("Factorial calculation caused overflow at " + std::to_string(i));
        }
        
        result *= i;
    }
    
    return result;
}

template <typename T>
bool FactorialCalculator<T>::willOverflow(T a, T b) {
    // Проверка на переполнение при умножении
    if (a == 0 || b == 0) {
        return false;
    }
    
    T maxValue = std::numeric_limits<T>::max();
    return a > maxValue / b;
}

} // namespace math
