#include "hmi.hpp"
#include <iostream>
#include <limits>

namespace HMI {

    void showMenu() {
        std::cout << "=== Dynamox Challenge ===\n";

        for (int i = 1; i < static_cast<int>(Operation::LAST_OPERATION); i++) {
            auto op = static_cast<Operation>(i);
            std::cout << i << ". " << toString(op) << "\n";
        }
        std::cout << "0. " << toString(Operation::Exit) << "\n";
    }

    void showResult(double result) {
        std::cout << "Result: " << result << "\n\n";
    }

    HMI::Operation getOperationChoice() {
        int choice = getInt("Choose an operation: ");
        return static_cast<HMI::Operation>(choice);
    }

    double getSingleInput(const std::string& prompt) {
        return getDouble(prompt);
    }

    std::vector<double> getArrayInput() {
        int n = getInt("How many values? ");
        std::vector<double> values(n);
        for (int i = 0; i < n; i++) {
            values[i] = getDouble("Value " + std::to_string(i+1) + ": ");
        }
        return values;
    }

    int getInt(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                return value; // valid input
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter an integer.\n";
        }
    }

    double getDouble(const std::string& prompt) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                return value; // valid input
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter a number.\n";
        }
    }

    void showMessage(const std::string& msg) {
        std::cout << msg << "\n\n";
    }

    void showError(const std::string& msg) {
        std::cout << "Error: " << msg << "\n\n";
    }
}
