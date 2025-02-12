// Fib.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once
#include <vector>
#include <stdexcept>

class Fibonacci {
public:
    // Возвращает первые n чисел Фибоначчи
    std::vector<int> getFirstNFibonacci(int n) const;

private:
    // Вспомогательная функция для вычисления чисел Фибоначчи
    int computeFibonacci(int n) const;
};

// TODO: установите здесь ссылки на дополнительные заголовки, требующиеся для программы.
