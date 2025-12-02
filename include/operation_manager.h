/**
 * @file operation_manager.h
 * @brief Manages available operations and their execution
 * 
 * This is the central component that:
 * - Maintains list of available operations
 * - Provides operation discovery
 * - Executes selected operations with validation
 */

#ifndef OPERATION_MANAGER_H
#define OPERATION_MANAGER_H

#include "operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the operation manager
 * @return 0 on success, -1 on failure
 */
int operation_manager_init(void);

/**
 * @brief Get total number of available operations
 * @return Number of operations
 */
int operation_manager_get_count(void);

/**
 * @brief Get operation by index
 * @param index Operation index (0 to count-1)
 * @return Pointer to Operation or NULL if invalid index
 */
const Operation* operation_manager_get(int index);

/**
 * @brief List all available operations to stdout
 */
void operation_manager_list(void);

/**
 * @brief Execute an operation by index
 * @param index Operation index
 * @param values Input values array
 * @param count Number of input values
 * @param result Pointer to store result
 * @return 0 on success, -1 on error
 */
int operation_manager_execute(int index, const double* values, int count, double* result);

#ifdef __cplusplus
}
#endif

#endif // OPERATION_MANAGER_H