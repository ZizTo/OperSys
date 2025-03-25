/**
 * @file factorial_calculator.hpp
 * @brief Класс для вычисления факториалов
 */

#ifndef FACTORIAL_CALCULATOR_HPP
#define FACTORIAL_CALCULATOR_HPP

#include <vector>
#include <stdexcept>
#include <string>

namespace math {

/**
 * @class FactorialCalculator
 * @brief Класс для вычисления факториалов и связанных операций
 * 
 * @tparam T Тип данных для хранения результата (должен поддерживать умножение)
 */
template <typename T = unsigned long long>
class FactorialCalculator {
public:
    /**
     * @brief Вычисляет первые n факториалов
     * 
     * @param n Количество факториалов
     * @return std::vector<T> Вектор, содержащий первые n факториалов
     * @throw std::invalid_argument Если n отрицательное
     * @throw std::overflow_error Если результат вычисления не помещается в тип T
     */
    static std::vector<T> calculateFirstNFactorials(int n);

    /**
     * @brief Вычисляет факториал числа n
     * 
     * @param n Число, для которого вычисляется факториал
     * @return T Факториал числа n
     * @throw std::invalid_argument Если n отрицательное
     * @throw std::overflow_error Если результат вычисления не помещается в тип T
     */
    static T calculateFactorial(int n);

private:
    /**
     * @brief Проверяет, произойдет ли переполнение при умножении a на b
     * 
     * @param a Первый множитель
     * @param b Второй множитель
     * @return bool true, если произойдет переполнение, иначе false
     */
    static bool willOverflow(T a, T b);
};

// Включаем реализацию шаблонных методов
#include "../../src/factorial/factorial_calculator.cpp"

} // namespace math

#endif // FACTORIAL_CALCULATOR_HPP
