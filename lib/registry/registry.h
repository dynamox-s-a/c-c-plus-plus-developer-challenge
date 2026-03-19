/**
 * @file registry.h
 * @brief Operation registry for dynamic operation registration.
 *
 * Allows registration of new operations at runtime, enabling
 * extensibility without modifying existing code (Open/Closed Principle).
 *
 * Each operation has:
 *   - A human-readable name
 *   - An input mode (single value, array, or matrix)
 *   - A function pointer implementing the operation
 *
 * Future improvements:
 *   - Plugin system loading operations from shared libraries (.so / .dll)
 *   - Operation categories and grouping
 *   - Operation chaining (pipe output of one as input to another)
 */

#ifndef REGISTRY_H
#define REGISTRY_H

#include <stddef.h>

/** Maximum number of operations that can be registered */
#define REGISTRY_MAX_OPERATIONS 32

/** Maximum length of an operation name */
#define REGISTRY_NAME_MAX_LEN 64

/** Input mode for an operation */
typedef enum {
    INPUT_SINGLE,  /**< Single scalar value */
    INPUT_ARRAY,   /**< Array of values */
    INPUT_MATRIX   /**< Matrix (2D) input */
} input_mode_t;

/**
 * @brief Function signature for array/scalar operations.
 *
 * @param[in]  values  Input array (single-element for scalar ops).
 * @param[in]  count   Number of elements.
 * @param[out] result  Pointer to store the result.
 * @return 0 on success, non-zero on error.
 */
typedef int (*op_array_fn)(const double *values, size_t count, double *result);

/**
 * @brief Function signature for matrix operations.
 *
 * @param[in]  mat   2D matrix.
 * @param[in]  rows  Number of rows.
 * @param[in]  cols  Number of columns.
 * @param[out] result  Pointer to store the result.
 * @return 0 on success, non-zero on error.
 */
typedef int (*op_matrix_fn)(double **mat, size_t rows, size_t cols, double *result);

/** Descriptor for a registered operation */
typedef struct {
    char name[REGISTRY_NAME_MAX_LEN];
    input_mode_t input_mode;
    union {
        op_array_fn  array_fn;
        op_matrix_fn matrix_fn;
    } fn;
} operation_t;

/**
 * @brief Initialize the operation registry. Must be called before use.
 */
void registry_init(void);

/**
 * @brief Register an array/scalar operation.
 *
 * @param[in] name       Display name for the operation.
 * @param[in] mode       INPUT_SINGLE or INPUT_ARRAY.
 * @param[in] func       Function implementing the operation.
 * @return 0 on success, -1 if registry is full.
 */
int registry_add_array_op(const char *name, input_mode_t mode, op_array_fn func);

/**
 * @brief Register a matrix operation.
 *
 * @param[in] name  Display name.
 * @param[in] func  Function implementing the operation.
 * @return 0 on success, -1 if registry is full.
 */
int registry_add_matrix_op(const char *name, op_matrix_fn func);

/**
 * @brief Get the number of registered operations.
 * @return Count of operations.
 */
size_t registry_count(void);

/**
 * @brief Get an operation by index.
 *
 * @param[in] index  Zero-based index.
 * @return Pointer to operation descriptor, or NULL if out of range.
 */
const operation_t *registry_get(size_t index);

#endif /* REGISTRY_H */
