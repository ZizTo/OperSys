#include "Fib.h"

std::vector<int> Fibonacci::getFirstNFibonacci(int n) const {
    if (n < 0) {
        throw std::invalid_argument("n must be non-negative");
    }
    std::vector<int> result;
    if (n > 0) { result.push_back(0); }
    if (n > 1) { result.push_back(1); }
    for (int i = 2; i < n; i++) {
        result.push_back(result[i - 1] + result[i - 2]);
    }
    return result;
}
