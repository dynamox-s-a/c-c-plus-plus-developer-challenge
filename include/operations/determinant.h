/**
 * @file determinant.h
 * @brief Matrix determinant operation
 */

#ifndef DETERMINANT_H
#define DETERMINANT_H

#include "operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Determinant operation instance
 * Calculates determinant of 2x2 or 3x3 matrix
 */
extern Operation operation_determinant;

#ifdef __cplusplus
}
#endif

#endif // DETERMINANT_H