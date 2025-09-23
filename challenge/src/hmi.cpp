#include "hmi.hpp"
#include <iostream>

namespace HMI {
    void showMenu() {
        std::cout << "=== Dynamox Challenge ===\n";
        std::cout << "1. Soma (array de valores)\n";
        std::cout << "2. Multiplicacao (array de valores)\n";
        std::cout << "3. Determinante (2x2)\n";
        std::cout << "4. Transposta (2x2)\n";
        std::cout << "0. Sair\n";
    }

    int getOperationChoice() {
        int choice;
        std::cout << "Escolha uma operacao: ";
        std::cin >> choice;
        return choice;
    }
}
