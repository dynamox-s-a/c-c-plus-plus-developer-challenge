/**
 * @file mean.c
 * @brief Implementation of arithmetic mean operation
 * 
 * Calculates the average of all values.
 * Example: [10.0, 20.0, 30.0] -> 20.0
 */

#include "operations/mean.h"
#include <stdio.h>

/**
 * @brief Execute mean operation
 */
static int mean_execute(const double* values, int count, double* result) {
    // Validate input
    if (values == NULL || result == NULL) {
        fprintf(stderr, "ERROR: NULL pointer in mean\n");
        return -1;
    }
    
    if (count <= 0) {
        fprintf(stderr, "ERROR: Invalid count for mean\n");
        return -1;
    }
    
    // Calculate sum
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    
    // Calculate mean
    *result = sum / count;
    
    return 0;  // Success
}

/**
 * @brief Mean operation definition
 */
Operation operation_mean = {
    .name = "mean",
    .description = "Calculate arithmetic mean (average)",
    .supports_array = 1,  // Accepts arrays
    .execute = mean_execute
};