#include "hmi.hpp"
#include "operations.hpp"
#include "log.hpp"

#include <iostream>
#include <sstream>

int main() {
    Log::write("Welcome to Dynamox Challenge!");

    while (true) {
        HMI::showMenu();

        /* Depending on the hardware, here we could go into low energy waiting for an HW interruption. */

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

                if ( Operations::divide(values[0], values[1], result) )
                {
                    HMI::showResult(result);
                    Log::write("Operation divide: " + std::to_string(values[0]) + " and " + std::to_string(values[1]) + ", resulting: " + std::to_string(result));
                }
                else {
                    HMI::showError("Division by zero");
                    Log::write(std::string("Error during division: Division by zero"));
                }
                break;
            }
            case HMI::Operation::Determinant: {
                HMI::showMessage( "Not implemented yet!" );
                Log::write("User selected Determinant operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::Transpose: {
                HMI::showMessage( "Not implemented yet!" );
                Log::write("User selected Transpose operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::CreateNew: {
                HMI::showMessage( "Not implemented yet!" );
                Log::write("User selected Create New operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::Exit: {
                HMI::showMessage( "Quiting application!" );
                Log::write("User selected Exit operation.");
                return 0;
                break;
            }
            case HMI::Operation::LAST_OPERATION:
            default:
                HMI::showMessage( "Invalid option!" );
                break;
        }
    }

    Log::write("Quiting Dynamox Challenge!");
    return 0;
}
