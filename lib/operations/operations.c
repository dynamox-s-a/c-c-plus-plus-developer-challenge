/**
 * @file operations.c
 * @brief Implementation of arithmetic operations library.
 */

#include "operations.h"

#include <math.h>
#include <stdio.h>

op_status_t op_sum(const double *values, size_t count, double *result)
{
    if (values == NULL || result == NULL) {
        return OP_ERR_NULL_PTR;
    }
    if (count == 0) {
        return OP_ERR_INVALID_SIZE;
    }

    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += values[i];
    }
    *result = sum;
    return OP_OK;
}

op_status_t op_mean(const double *values, size_t count, double *result)
{
    if (values == NULL || result == NULL) {
        return OP_ERR_NULL_PTR;
    }
    if (count == 0) {
        return OP_ERR_INVALID_SIZE;
    }

    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += values[i];
    }
    *result = sum / (double)count;
    return OP_OK;
}

op_status_t op_min(const double *values, size_t count, double *result)
{
    if (values == NULL || result == NULL) {
        return OP_ERR_NULL_PTR;
    }
    if (count == 0) {
        return OP_ERR_INVALID_SIZE;
    }

    double min_val = values[0];
    for (size_t i = 1; i < count; i++) {
        if (values[i] < min_val) {
            min_val = values[i];
        }
    }
    *result = min_val;
    return OP_OK;
}

op_status_t op_max(const double *values, size_t count, double *result)
{
    if (values == NULL || result == NULL) {
        return OP_ERR_NULL_PTR;
    }
    if (count == 0) {
        return OP_ERR_INVALID_SIZE;
    }

    double max_val = values[0];
    for (size_t i = 1; i < count; i++) {
        if (values[i] > max_val) {
            max_val = values[i];
        }
    }
    *result = max_val;
    return OP_OK;
}

op_status_t op_factorial(const double *values, size_t count, double *result)
{
    if (values == NULL || result == NULL) {
        return OP_ERR_NULL_PTR;
    }
    if (count != 1) {
        return OP_ERR_INVALID_SIZE;
    }

    double val = values[0];

    /* Validate: must be a non-negative integer */
    if (val < 0.0 || floor(val) != val) {
        return OP_ERR_INVALID_INPUT;
    }

    /* Guard against overflow (20! is the max that fits in a double without loss) */
    if (val > 170.0) {
        fprintf(stderr, "Warning: factorial overflow for values > 170.\n");
        return OP_ERR_INVALID_INPUT;
    }

    double fact = 1.0;
    for (int i = 2; i <= (int)val; i++) {
        fact *= i;
    }

    *result = fact;
    return OP_OK;
}
