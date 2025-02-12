#include "Fib.h"

std::vector<int> Fibonacci::getFirstNFibonacci(int n) const {
    if (n < 0) {
        throw std::invalid_argument("n must be non-negative");
    }

    std::vector<int> result;
    for (int i = 0; i < n; ++i) {
        result.push_back(computeFibonacci(i));
    }
    return result;
}

int Fibonacci::computeFibonacci(int n) const {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return computeFibonacci(n - 1) + computeFibonacci(n - 2);
}
