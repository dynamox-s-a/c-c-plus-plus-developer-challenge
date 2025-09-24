#include "hmi.hpp"
#include <iostream>

namespace HMI {

    void showMenu() {
        std::cout << "=== Dynamox Challenge ===\n";

        for (int i = 1; i < static_cast<int>(Operation::LAST_OPERATION); i++) {
            auto op = static_cast<Operation>(i);
            std::cout << i << ". " << toString(op) << "\n";
        }
        std::cout << "0. " << toString(Operation::Exit) << "\n";
    }

    HMI::Operation getOperationChoice() {
        int choice;
        std::cout << "Choose an operation: ";
        std::cin >> choice;
        return static_cast<HMI::Operation>(choice);
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
