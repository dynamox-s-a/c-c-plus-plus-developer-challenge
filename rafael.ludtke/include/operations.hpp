#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include <vector>

namespace Operations {
    double add(const double& valueA, const double& valueB);
    double add(const std::vector<double>& values);
    double multiply(const std::vector<double>& values);
    bool divide(const double& valueA, const double& valueB, double& result);
}

#endif
