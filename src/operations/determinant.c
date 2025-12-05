/**
 * @file determinant.c
 * @brief Implementation of matrix determinant calculation
 * 
 * Supports 2x2 and 3x3 matrices using direct formulas.
 * Supports 4x4, 5x5, and 6x6 matrices using LU decomposition.
 */

#include "operations/determinant.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_MATRIX_SIZE 6  // Changed from 10 to 6 (input buffer constraint)

// For debug
static void print_matrix(double A[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], int n) {
    printf("Matrix for debug:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%8.3f ", A[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief LU Decomposition with partial pivoting
 * 
 * Decomposes matrix A into L (lower) and U (upper) triangular matrices.
 * Uses partial pivoting for numerical stability.
 * 
 * @param A Matrix to decompose (will be modified in-place)
 * @param n Size of matrix
 * @param perm Permutation array (output)
 * @return 0 on success, -1 if matrix is singular
 */
static int lu_decompose(double A[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], 
                       int n, 
                       int* num_swaps) {
    *num_swaps = 0;
    
    // Calculate matrix norm for adaptive threshold
    double matrix_norm = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double abs_val = fabs(A[i][j]);
            if (abs_val > matrix_norm) {
                matrix_norm = abs_val;
            }
        }
    }
    
    // Adaptive tolerance (scaled with matrix size and values)
    double tolerance = matrix_norm * 1e-14 * n;
    if (tolerance < 1e-12) {
        tolerance = 1e-12;  // Minimum threshold
    }
    
    // Gaussian elimination with partial pivoting
    for (int k = 0; k < n; k++) {
        // Find pivot (maximum element in column k below row k)
        double max_val = fabs(A[k][k]);
        int max_row = k;
        
        for (int i = k + 1; i < n; i++) {
            if (fabs(A[i][k]) > max_val) {
                max_val = fabs(A[i][k]);
                max_row = i;
            }
        }
        
        // Check for singular matrix
        if (max_val < tolerance) {
            fprintf(stderr, "ERROR: Matrix is singular or nearly singular\n");
            fprintf(stderr, "       Pivot value: %.2e (threshold: %.2e)\n", 
                    max_val, tolerance);
            return -1;
        }
        
        // Swap rows if needed
        if (max_row != k) {
            (*num_swaps)++;  // Count actual row swap
            
            // Swap entire rows
            for (int j = 0; j < n; j++) {
                double temp = A[k][j];
                A[k][j] = A[max_row][j];
                A[max_row][j] = temp;
            }
        }
        
        // Elimination step
        for (int i = k + 1; i < n; i++) {
            // Calculate multiplier
            A[i][k] = A[i][k] / A[k][k];
            
            // Update row i
            for (int j = k + 1; j < n; j++) {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
    }

    //debug
    //print_matrix(A, n);
    
    return 0;
}

/**
 * @brief Calculate determinant from LU decomposition
 */
static double lu_determinant(double A[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], 
                             int n, 
                             int num_swaps) {
    // Determinant = product of diagonal elements of U
    double det = 1.0;
    for (int i = 0; i < n; i++) {
        det *= A[i][i];
    }
    
    // Apply sign from permutations (odd swaps = negative)
    if (num_swaps % 2 == 1) {
        det = -det;
    }
    
    return det;
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

    if (n < 2) {
        fprintf(stderr, "ERROR: Matrix too small (min 2x2)\n");
        return -1;
    }
    
    // Calculate determinant based on size
    if (n == 2) {
        // 2x2 determinant: ad - bc
        // Matrix layout: [a b]
        //                [c d]
        double a = values[0];
        double b = values[1];
        double c = values[2];
        double d = values[3];
        
        *result = a * d - b * c;
        return 0;
    }
    else if (n == 3) {
        // 3x3 determinant using Sarrus rule
        // Matrix layout: [a b c]
        //                [d e f]
        //                [g h i]
        double a = values[0], b = values[1], c = values[2];
        double d = values[3], e = values[4], f = values[5];
        double g = values[6], h = values[7], i = values[8];
        
        *result = a * (e * i - f * h)
                - b * (d * i - f * g)
                + c * (d * h - e * g);
        return 0;
    }
    
    // For 4x4, 5x5, 6x6: use LU decomposition
    // Copy values to 2D matrix
    double A[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = values[i * n + j];
        }
    }

    // Perform LU decomposition
    int num_swaps;
    if (lu_decompose(A, n, &num_swaps) != 0) {
        return -1;  // Singular matrix
    }
    
    // Calculate determinant
    *result = lu_determinant(A, n, num_swaps);
    
    return 0;
}

/**
 * @brief Determinant operation definition
 */
Operation operation_determinant = {
    .name = "determinant",
    .description = "Calculate matrix determinant (2x2 or 6x6)",
    .supports_array = 1,  // Accepts arrays (matrix as flat array)
    .execute = determinant_execute
};