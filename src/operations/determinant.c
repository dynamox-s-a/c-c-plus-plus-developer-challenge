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