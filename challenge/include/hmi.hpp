#ifndef HMI_HPP
#define HMI_HPP

#include <vector>
#include <string>

namespace HMI {
    void showMenu();
    int getOperationChoice();
    double getSingleInput();
    std::vector<double> getArrayInput();
    std::vector<std::vector<double>> getMatrixInput(int n);
    void showResult(double result);
}

#endif
