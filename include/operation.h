/**
 * @file operation.h
 * @brief Base interface for all mathematical operations
 * 
 * This defines the contract that all operations must follow.
 * Uses pure C with function pointers for simplicity and zero overhead.
 * 
 * Design principles:
 * - Simple struct with function pointer
 * - Static allocation friendly (embedded systems)
 * - Easy to understand and extend
 */

#ifndef OPERATION_H
#define OPERATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Operation execution function signature
 * 
 * @param values Array of input values
 * @param count Number of values in the array
 * @param result Pointer to store the result
 * @return 0 on success, -1 on error
 * 
 * Example:
 *   double values[] = {1.0, 2.0, 3.0};
 *   double result;
 *   int status = execute(values, 3, &result);
 */
typedef int (*operation_execute_fn)(const double* values, int count, double* result);

/**
 * @brief Operation structure
 * 
 * This struct defines everything needed for an operation:
 * - Identification (name, description)
 * - Capabilities (supports arrays?)
 * - Execution (function pointer)
 */
typedef struct {
    const char* name;              /**< Operation name (e.g., "add", "mean") */
    const char* description;       /**< Human-readable description */
    int supports_array;            /**< 1 if accepts arrays, 0 if single value only */
    operation_execute_fn execute;  /**< Function that performs the operation */
} Operation;

#ifdef __cplusplus
}
#endif

#endif // OPERATION_H