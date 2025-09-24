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

        HMI::Operation op = HMI::getOperationChoice();

        switch (op) {
            case HMI::Operation::SumSingle: {
                double values[2];
                values[0] = HMI::getSingleInput();
                values[1] = HMI::getSingleInput();
                double result = Operations::add(values[0], values[1]);
                HMI::showResult(result);
                Log::write("Operation add: " + std::to_string(values[0]) + " and " + std::to_string(values[1]) + ", resulting: " + std::to_string(result));
                break;
            }
            case HMI::Operation::SumArray: {
                auto values = HMI::getArrayInput();
                double result = Operations::add(values);
                HMI::showResult(result);
                Log::write("Operation add: array of size " + std::to_string(values.size()) + ", resulting: " + std::to_string(result));
                break;
            }
            case HMI::Operation::Divide: {
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
            case HMI::Operation::Determinant: {
                std::cout << "Not implemented yet!\n\n";
                Log::write("User selected Determinant operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::Transpose: {
                std::cout << "Not implemented yet!\n\n";
                Log::write("User selected Transpose operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::CreateNew: {
                std::cout << "Not implemented yet!\n\n";
                Log::write("User selected Create New operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::Exit: {
                std::cout << "Quiting application!\n";
                Log::write("User selected Exit operation.");
                return 0;
                break;
            }
            case HMI::Operation::LAST_OPERATION:
            default:
                std::cout << "Invalid option!\n";
                break;
        }
    }

    Log::write("Quiting Dynamox Challenge!");
    return 0;
}
