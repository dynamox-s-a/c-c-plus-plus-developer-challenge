#include "hmi.hpp"
#include "hmi_console.hpp"
#include "operations.hpp"
#include "matrix.hpp"
#include "log.hpp"

#include <iostream>
#include <sstream>

int main() {
    Log::write("Welcome to Dynamox Challenge!");

    while (true) {
        HMI::hmi_console hmi;

        hmi.showMenu();

        /* For the challenge: Depending on the hardware, here we could go into low energy waiting for an HW interruption. */

        HMI::Operation op = hmi.getOperationChoice();

        switch (op) {
            case HMI::Operation::SumSingle: {
                double values[2];
                values[0] = hmi.getSingleInput();
                values[1] = hmi.getSingleInput();
                double result = Operations::add(values[0], values[1]);
                hmi.showResult(result);
                Log::write("Operation add: " + std::to_string(values[0]) + " and " + std::to_string(values[1]) + ", resulting: " + std::to_string(result));
                break;
            }
            case HMI::Operation::SumArray: {
                auto values = hmi.getArrayInput();
                double result = Operations::add(values);
                hmi.showResult(result);
                Log::write("Operation add: array of size " + std::to_string(values.size()) + ", resulting: " + std::to_string(result));
                break;
            }
            case HMI::Operation::Divide: {
                double values[2];
                double result = 0;
                values[0] = hmi.getSingleInput("Dividend: ");
                values[1] = hmi.getSingleInput("Divisor: ");

                if ( Operations::divide(values[0], values[1], result) )
                {
                    hmi.showResult(result);
                    Log::write("Operation divide: " + std::to_string(values[0]) + " and " + std::to_string(values[1]) + ", resulting: " + std::to_string(result));
                }
                else {
                    hmi.showError("Division by zero");
                    Log::write(std::string("Error during division: Division by zero"));
                }
                break;
            }
            case HMI::Operation::Determinant: {
                int mtx_size = hmi.getInt("Matrix size: ");
                if ( mtx_size <= 0 || mtx_size > 2 ) {
                    hmi.showError("Matrix size for determinant must be positive and not higher than 2(to be implemented).");
                    break;
                }

                auto matrix = hmi.getMatrixInput(mtx_size);
                double result;

                if ( Matrix::determinant(matrix, result) ) {
                    hmi.showResult(result);
                    Log::write("Determinant for " + std::to_string(mtx_size) + "x" + std::to_string(mtx_size) +
                            ", result: " + std::to_string(result));
                } else {
                    hmi.showError("Invalid input matrix.");
                    Log::write("User attempted determinant of unsupported size: " + std::to_string(mtx_size));
                }
                break;
            }
            case HMI::Operation::Transpose: {
                auto matrix = hmi.getMatrixInput();
                std::vector<std::vector<double>> result;

                if ( Matrix::transpose(matrix, result) ) {
                    hmi.showResult(matrix);
                    hmi.showResult(result);
                    Log::write("Transpose operation successfully.");
                } else {
                    hmi.showError("Invalid input matrix.");
                    Log::write("Transpose failed: invalid input matrix.");
                }
                break;
            }
            case HMI::Operation::CreateNew: {
                hmi.showMessage( "Not implemented yet!" );
                Log::write("User selected Create New operation, but it's not implemented yet.");
                break;
            }
            case HMI::Operation::Exit: {
                hmi.showMessage( "Quitting application!" );
                Log::write("User selected Exit operation.");
                return 0;
                break;
            }
            case HMI::Operation::LAST_OPERATION:
            default:
                hmi.showMessage( "Invalid option!" );
                break;
        }
    }

    Log::write("Quitting Dynamox Challenge!");
    return 0;
}
