/**
 * @file multiply.c
 * @brief Implementation of array multiplication operation
 * 
 * Multiplies all values in the input array.
 * Example: [2.0, 3.0, 4.0] -> 24.0
 * 
 * This operation demonstrates extensibility:
 * - New file created
 * - Implements Operation interface
 * - Automatically available in system
 */

#include "operations/multiply.h"
#include <stdio.h>

/**
 * @brief Execute multiplication operation
 */
static int multiply_execute(const double* values, int count, double* result) {
    // Validate input
    if (values == NULL || result == NULL) {
        fprintf(stderr, "ERROR: NULL pointer in multiplication\n");
        return -1;
    }
    
    if (count <= 0) {
        fprintf(stderr, "ERROR: Invalid count for multiplication\n");
        return -1;
    }
    
    // Multiply all values
    *result = 1.0;
    for (int i = 0; i < count; i++) {
        *result *= values[i];
    }
    
    return 0;  // Success
}

/**
 * @brief Multiplication operation definition
 */
Operation operation_multiply = {
    .name = "multiply",
    .description = "Multiply all values in array",
    .supports_array = 1,  // Accepts arrays
    .execute = multiply_execute
};