#include <gtest/gtest.h>
#include "matrix.h"
#include "lib_matrix.h"


TEST(MatrixTest, Determinant2x2) {
    Matrix mat(2, 2);
    mat.at(0, 0) = 1; mat.at(0, 1) = 2;
    mat.at(1, 0) = 3; mat.at(1, 1) = 4;
    EXPECT_DOUBLE_EQ(Lib_matrix::determinant(mat), -2.0);
}

