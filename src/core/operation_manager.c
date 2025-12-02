/**
 * @file operation_manager.c
 * @brief Implementation of operation manager
 */

#include "operation_manager.h"
#include <stdio.h>
#include <stdlib.h>

// Forward declarations of available operations
// These will be implemented in src/operations/
extern Operation operation_add;
extern Operation operation_mean;
extern Operation operation_determinant;
extern Operation operation_multiply;

/**
 * @brief Static array of available operations
 * 
 * TO ADD NEW OPERATION: Just add the extern declaration above
 * and add pointer here. No other changes needed!
 */
static Operation* available_operations[] = {
    &operation_add,
    &operation_mean,
    //&operation_determinant,
    &operation_multiply
};

static const int num_operations = sizeof(available_operations) / sizeof(Operation*);

/**
 * @brief Initialize operation manager
 */
int operation_manager_init(void) {
    // Validate that all operations are properly defined
    for (int i = 0; i < num_operations; i++) {
        if (available_operations[i] == NULL) {
            fprintf(stderr, "ERROR: Operation at index %d is NULL\n", i);
            return -1;
        }
        
        if (available_operations[i]->name == NULL) {
            fprintf(stderr, "ERROR: Operation at index %d has NULL name\n", i);
            return -1;
        }
        
        if (available_operations[i]->execute == NULL) {
            fprintf(stderr, "ERROR: Operation '%s' has NULL execute function\n",
                    available_operations[i]->name);
            return -1;
        }
    }
    
    printf("Operation Manager: %d operations loaded\n", num_operations);
    return 0;
}

/**
 * @brief Get total number of operations
 */
int operation_manager_get_count(void) {
    return num_operations;
}

/**
 * @brief Get operation by index
 */
const Operation* operation_manager_get(int index) {
    if (index < 0 || index >= num_operations) {
        return NULL;
    }
    return available_operations[index];
}

/**
 * @brief List all available operations
 */
void operation_manager_list(void) {
    printf("\n=== Available Operations ===\n");
    for (int i = 0; i < num_operations; i++) {
        Operation* op = available_operations[i];
        printf("%d. %s - %s\n", 
               i + 1,
               op->name,
               op->description);
        printf("   Supports arrays: %s\n",
               op->supports_array ? "Yes" : "No");
    }
    printf("\n");
}

/**
 * @brief Execute operation with validation
 */
int operation_manager_execute(int index, const double* values, int count, double* result) {
    // Validate index
    if (index < 0 || index >= num_operations) {
        fprintf(stderr, "ERROR: Invalid operation index %d\n", index);
        return -1;
    }
    
    // Validate inputs
    if (values == NULL || result == NULL) {
        fprintf(stderr, "ERROR: NULL pointer in execute\n");
        return -1;
    }
    
    if (count <= 0) {
        fprintf(stderr, "ERROR: Invalid count %d\n", count);
        return -1;
    }
    
    Operation* op = available_operations[index];
    
    // Check if operation supports the input type
    if (count > 1 && !op->supports_array) {
        fprintf(stderr, "ERROR: Operation '%s' does not support array input\n", op->name);
        fprintf(stderr, "       Please provide a single value\n");
        return -1;
    }
    
    printf("Executing: %s\n", op->name);
    
    // Execute the operation
    int status = op->execute(values, count, result);
    
    if (status != 0) {
        fprintf(stderr, "ERROR: Operation '%s' failed\n", op->name);
        return -1;
    }
    
    return 0;
}