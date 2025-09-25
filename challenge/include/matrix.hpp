#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>

namespace Matrix {
    bool determinant(const std::vector<std::vector<double>>& matrix, double& result);
    bool transpost(const std::vector<std::vector<double>>& matrix, std::vector<std::vector<double>>& mtx_result);
}

#endif
