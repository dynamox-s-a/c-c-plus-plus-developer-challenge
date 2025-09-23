#include "hmi.hpp"
#include "operations.hpp"

#include <iostream>
#include <sstream>

int main() {
    while (true) {
        HMI::showMenu();

        /* Depending the hardware, here we could sleep waiting for an HW interruption. */
        u_int8_t choice = HMI::getOperationChoice();

        if (choice == 0) {
            std::cout << "Exiting the program.\n";
            break;
        }

        switch (choice) {
            case 1: {
                auto values = HMI::getArrayInput();
                double result = Operations::add(values);
                HMI::showResult(result);

                break;
            }
            default:
                std::cout << "Opcao invalida!\n";
        }
    }

    return 0;
}
