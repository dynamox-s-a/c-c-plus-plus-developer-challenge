#ifndef DETERMINANT_H
#define DETERMINANT_H

#define MAX_MATRIX_DIMENSION 10

enum DeterminantStatus {
    DET_SUCCESS = 0,
    DET_INVALID_DIMENSION = 1,
    DET_NULL_OUTPUT = 2
};

int calculate_determinant_gauss(int n,
                                double matrix[MAX_MATRIX_DIMENSION][MAX_MATRIX_DIMENSION],
                                double *determinant);

#endif