#include "operations.hpp"
#include <numeric>
#include <functional>

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
}
