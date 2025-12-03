/**
 * @file determinant.c
 * @brief Implementation of matrix determinant calculation
 * 
 * Supports 2x2 and 3x3 matrices using direct formulas.
 * Future: Can be extended to NxN using LU decomposition.
 */

#include "operations/determinant.h"
#include <stdio.h>
#include <math.h>

#define MAX_MATRIX_SIZE 10

/**
 * @brief Calculate determinant of 2x2 matrix
 */
static double det_2x2(const double matrix[2][2]) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

/**
 * @brief Calculate determinant of 3x3 matrix using Sarrus rule
 */
static double det_3x3(const double matrix[3][3]) {
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
         - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
         + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

/**
 * @brief Execute determinant calculation
 */
static int determinant_execute(const double* values, int count, double* result) {
    // Validate input
    if (values == NULL || result == NULL) {
        fprintf(stderr, "ERROR: NULL pointer in determinant\n");
        return -1;
    }
    
    // Check if values form a square matrix
    int n = (int)sqrt((double)count);
    if (n * n != count) {
        fprintf(stderr, "ERROR: Values do not form a square matrix\n");
        fprintf(stderr, "       Received %d values (need n²)\n", count);
        return -1;
    }
    
    // Check matrix size limit
    if (n > MAX_MATRIX_SIZE) {
        fprintf(stderr, "ERROR: Matrix too large (max %dx%d)\n", 
                MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
        return -1;
    }
    
    // Copy values to 2D matrix for easier manipulation
    double matrix[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = values[i * n + j];
        }
    }
    
    // Calculate based on size
    if (n == 2) {
        *result = det_2x2((const double(*)[2])matrix);
        return 0;
    }
    else if (n == 3) {
        *result = det_3x3((const double(*)[3])matrix);
        return 0;
    }
    else {
        fprintf(stderr, "ERROR: Determinant only supported for 2x2 and 3x3 matrices\n");
        fprintf(stderr, "       Future: LU decomposition for NxN\n");
        return -1;
    }
}

/**
 * @brief Determinant operation definition
 */
Operation operation_determinant = {
    .name = "determinant",
    .description = "Calculate matrix determinant (2x2 or 3x3)",
    .supports_array = 1,  // Accepts arrays (matrix as flat array)
    .execute = determinant_execute
};