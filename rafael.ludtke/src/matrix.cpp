#include "matrix.hpp"

namespace Matrix {
    bool determinant(const std::vector<std::vector<double>>& matrix,
        double& result) {
        // Invalid or not square
        if (matrix.empty() || matrix.size() != matrix[0].size()) {
            return false;
        }
        if (matrix.size() == 1) {
            result = matrix[0][0];
            return true;
        }
        else if (matrix.size() == 2) {
            result = (matrix[0][0] * matrix[1][1]) - (matrix[0][1] * matrix[1][0]);
            return true;
        }
        else {
            // TODO: implement Laplace (or other)
            return false;
        }
    }

    bool transpose(const std::vector<std::vector<double>>& matrix,
                std::vector<std::vector<double>>& result) {

        if (matrix.empty() || matrix[0].size() == 0)
            return false;

        size_t rows = matrix.size();
        size_t cols = matrix[0].size();

        // transpose switches rows and cols
        result.assign(cols, std::vector<double>(rows));

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result[j][i] = matrix[i][j];
            }
        }
        return true;
    }
}
