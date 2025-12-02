/**
 * @file addition.c
 * @brief Implementation of array addition operation
 * 
 * Sums all values in the input array.
 * Example: [1.0, 2.0, 3.0] -> 6.0
 */

#include "operations/addition.h"
#include <stdio.h>

/**
 * @brief Execute addition operation
 * @param values Input array
 * @param count Number of values
 * @param result Pointer to store sum
 * @return 0 on success, -1 on error
 */
static int add_execute(const double* values, int count, double* result) {
    // Validate input
    if (values == NULL || result == NULL) {
        fprintf(stderr, "ERROR: NULL pointer in addition\n");
        return -1;
    }
    
    if (count <= 0) {
        fprintf(stderr, "ERROR: Invalid count for addition\n");
        return -1;
    }
    
    // Sum all values
    *result = 0.0;
    for (int i = 0; i < count; i++) {
        *result += values[i];
    }
    
    return 0;  // Success
}

/**
 * @brief Addition operation definition
 */
Operation operation_add = {
    .name = "add",
    .description = "Sum all values in array",
    .supports_array = 1,  // Accepts arrays
    .execute = add_execute
};