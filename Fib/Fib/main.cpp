#include <iostream>
#include "Fib.h"

int main() {
    Fibonacci fib;
    try {
        int n;
        std::cout << "Enter n of fibonacci numbers: ";
        std::cin >> n;
        std::vector<int> sequence = fib.getFirstNFibonacci(n);
        std::cout << "First 10 fibonacci numbers: ";
        for (int num : sequence) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    system("pause");
    return 0;
}