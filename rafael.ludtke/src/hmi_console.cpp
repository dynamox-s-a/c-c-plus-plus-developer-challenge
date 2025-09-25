#include "hmi_console.hpp"
#include <iostream>
#include <limits>
#include <iomanip>

namespace HMI {

    hmi_console::hmi_console() {}
    hmi_console::~hmi_console() {}  
    
    // Generate menu dynamically from HMI::Operation enum   
    // This avoids duplicating code and makes it easy to extend.
    // Keep 0 has Exit.
    void hmi_console::showMenu() {
        std::cout << "=== Dynamox Challenge ===\n";

        for (int i = 1; i < static_cast<int>(Operation::LAST_OPERATION); i++) {
            auto op = static_cast<Operation>(i);
            std::cout << i << ". " << toString(op) << "\n";
        }
        std::cout << "0. " << toString(Operation::Exit) << "\n";
    }

    void hmi_console::showResult(double result) {
        std::cout << "Result: " << result << "\n\n";
    }

    HMI::Operation hmi_console::getOperationChoice() {
        int choice = getInt("Choose an operation: ");
        return static_cast<HMI::Operation>(choice);
    }

    double hmi_console::getSingleInput(const std::string& prompt) {
        return getDouble(prompt);
    }

    std::vector<double> hmi_console::getArrayInput() {
        int n = getInt("How many values? ");
        std::vector<double> values(n);
        for (int i = 0; i < n; i++) {
            values[i] = getDouble("Value " + std::to_string(i+1) + ": ");
        }
        return values;
    }

    std::vector<std::vector<double>> hmi_console::getMatrixInput(int rows, int cols) {
        std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
        std::cout << "Enter matrix values " << rows << "x" << cols << ":\n";
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i][j] = getDouble("Element [" + std::to_string(i) + "][" + std::to_string(j) + "]: ");
            }
        }
        return matrix;
    }

    std::vector<std::vector<double>> hmi_console::getMatrixInput(int n) {
        return getMatrixInput(n, n);
    }

    std::vector<std::vector<double>> hmi_console::getMatrixInput() {
        int rows = getInt("Number of rows: ");
        int cols = getInt("Number of columns: ");
        return getMatrixInput(rows, cols);
    }

    int hmi_console::getInt(const std::string& prompt) {
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

    double hmi_console::getDouble(const std::string& prompt) {
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

    void hmi_console::showMessage(const std::string& msg) {
        std::cout << msg << "\n\n";
    }

    void hmi_console::showError(const std::string& msg) {
        std::cout << "Error: " << msg << "\n\n";
    }

    void hmi_console::showResult(const std::vector<std::vector<double>>& matrix) {
        if (matrix.empty()) {
            std::cout << "[Empty matrix]\n\n";
            return;
        }

        int rows = matrix.size();
        int cols = matrix[0].size();

        std::cout << "Matrix (" << rows << "x" << cols << "):\n";

        for (const auto& row : matrix) {
            for (double val : row) {
                // largura fixa e 3 casas decimais
                std::cout << std::setw(10) << std::setprecision(3) << std::fixed << val << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}