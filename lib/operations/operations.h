/**
 * @file operations.h
 * @brief Arithmetic operations library for single values and arrays.
 *
 * Provides common arithmetic operations that work on both scalar
 * and array inputs.
 *
 * Future improvements:
 *   - Standard deviation and variance
 *   - Median calculation
 *   - Dot product / cross product for vectors
 *   - Statistical operations (mode, percentile)
 */

#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stddef.h>

/** Return codes for arithmetic operations. */
typedef enum {
    OP_OK = 0,
    OP_ERR_NULL_PTR,      /**< NULL pointer argument */
    OP_ERR_INVALID_SIZE,  /**< Invalid array size */
    OP_ERR_DIV_ZERO,      /**< Division by zero */
    OP_ERR_INVALID_INPUT  /**< Invalid user input */
} op_status_t;

/**
 * @brief Compute the sum of an array of doubles.
 *
 * @param[in]  values  Array of values.
 * @param[in]  count   Number of elements.
 * @param[out] result  Sum of all elements.
 * @return op_status_t
 */
op_status_t op_sum(const double *values, size_t count, double *result);

/**
 * @brief Compute the arithmetic mean of an array of doubles.
 *
 * @param[in]  values  Array of values.
 * @param[in]  count   Number of elements.
 * @param[out] result  Mean value.
 * @return op_status_t
 */
op_status_t op_mean(const double *values, size_t count, double *result);

/**
 * @brief Find the minimum value in an array.
 *
 * @param[in]  values  Array of values.
 * @param[in]  count   Number of elements.
 * @param[out] result  Minimum value.
 * @return op_status_t
 */
op_status_t op_min(const double *values, size_t count, double *result);

/**
 * @brief Find the maximum value in an array.
 *
 * @param[in]  values  Array of values.
 * @param[in]  count   Number of elements.
 * @param[out] result  Maximum value.
 * @return op_status_t
 */
op_status_t op_max(const double *values, size_t count, double *result);

/**
 * @brief Compute the factorial of a non-negative integer.
 *
 * Works on a single value (the first element of the array).
 *
 * @param[in]  values  Array containing a single non-negative integer value.
 * @param[in]  count   Must be 1.
 * @param[out] result  Factorial result.
 * @return op_status_t
 */
op_status_t op_factorial(const double *values, size_t count, double *result);

#endif /* OPERATIONS_H */
