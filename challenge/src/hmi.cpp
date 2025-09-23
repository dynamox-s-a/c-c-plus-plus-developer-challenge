#include "hmi.hpp"
#include <iostream>

namespace HMI {
    void showMenu() {
        std::cout << "=== Dynamox Challenge ===\n";
        std::cout << "1. Summ (Single value)\n";
        std::cout << "2. Summ (Array)\n";
        std::cout << "3. Divide\n";
        std::cout << "4. Determinant (2x2)\n";
        std::cout << "5. Transpost (2x2)\n";
        std::cout << "0. Exit\n";
    }

    int getOperationChoice() {
        int choice;
        std::cout << "Choose an operation: ";
        std::cin >> choice;
        return choice;
    }

    double getSingleInput(const std::string& prompt) {
        double value;
        std::cout << prompt;
        std::cin >> value;
        return value;
    }

    std::vector<double> getArrayInput() {
        int n;
        std::cout << "How many values? ";
        std::cin >> n;
        std::vector<double> values(n);
        for (int i = 0; i < n; i++) {
            std::cout << "Value " << (i+1) << ": ";
            std::cin >> values[i];
        }
        return values;
    }

    void showResult(double result) {
        std::cout << "Result: " << result << "\n\n";
    }
}
