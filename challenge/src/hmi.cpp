#include "hmi.hpp"
#include <iostream>

namespace HMI {
    void showMenu() {
        std::cout << "=== Dynamox Challenge ===\n";
        std::cout << "1. Soma (Single value)\n";
        std::cout << "2. Soma (Array)\n";
        std::cout << "2. Subtração (array de valores)\n";
        std::cout << "3. Multiplicacao (array de valores)\n";
        std::cout << "4. Divisão\n";
        std::cout << "5. Determinante (2x2)\n";
        std::cout << "6. Transposta (2x2)\n";
        std::cout << "0. Sair\n";
    }

    int getOperationChoice() {
        int choice;
        std::cout << "Escolha uma operacao: ";
        std::cin >> choice;
        return choice;
    }

    double getSingleInput() {
        double value;
        std::cout << "Valor: ";
        std::cin >> value;
        return value;
    }

    std::vector<double> getArrayInput() {
        int n;
        std::cout << "Quantos valores? ";
        std::cin >> n;
        std::vector<double> values(n);
        for (int i = 0; i < n; i++) {
            std::cout << "Valor " << (i+1) << ": ";
            std::cin >> values[i];
        }
        return values;
    }

    void showResult(double result) {
    std::cout << "Resultado: " << result << "\n\n";
}
}
