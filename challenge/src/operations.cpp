#include "operations.hpp"
#include <numeric>
#include <functional>
#include <stdexcept>

namespace Operations {
    double add(const double& valueA, const double& valueB) {
        return valueA + valueB;
    }

    double add(const std::vector<double>& values) {
        return std::accumulate(values.begin(), values.end(), 0.0);
    }

    double multiply(const std::vector<double>& values) {
        return std::accumulate(values.begin(), values.end(), 1.0, std::multiplies<double>());
    }

    double divide(const double& valueA, const double& valueB) {
        if (valueB == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
        return valueA/valueB;
    }
}
