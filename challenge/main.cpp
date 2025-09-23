#include "hmi.hpp"
#include "operations.hpp"
#include "log.hpp"

#include <iostream>
#include <sstream>

int main() {
    Log::write("Welcome to Dynamox Challenge!");

    while (true) {
        HMI::showMenu();

        /* Depending the hardware, here we could go into low energy waiting for an HW interruption. */
        u_int8_t choice = HMI::getOperationChoice();

        if (choice == 0) {
            std::cout << "Exiting the program.\n";
            break;
        }

        switch (choice) {
            case 1: {
                double values[2];
                values[0] = HMI::getSingleInput();
                values[1] = HMI::getSingleInput();
                double result = Operations::add(values[0], values[1]);
                HMI::showResult(result);
                Log::write("Operation add: " + std::to_string(values[0]) + " and " + std::to_string(values[1]) + ", resulting: " + std::to_string(result));
                break;
            }
            case 2: {
                auto values = HMI::getArrayInput();
                double result = Operations::add(values);
                HMI::showResult(result);
                Log::write("Operation add: array of size " + std::to_string(values.size()) + ", resulting: " + std::to_string(result));
                break;
            }
            case 3: {
                double values[2];
                double result = 0;
                values[0] = HMI::getSingleInput("Dividend: ");
                values[1] = HMI::getSingleInput("Divisor: ");
                try {
                    result = Operations::divide(values[0], values[1]);
                    HMI::showResult(result);
                    Log::write("Operation divide: " + std::to_string(values[0]) + " and " + std::to_string(values[1]) + ", resulting: " + std::to_string(result));
                } catch (const std::invalid_argument& e) {
                    std::cout << "Error: " << e.what() << "\n\n";
                    Log::write(std::string("Error during division: ") + e.what());
                }
                break;
            }
            default:
                std::cout << "Invalid option!\n";
        }
    }

    Log::write("Quiting Dynamox Challenge!");
    return 0;
}
