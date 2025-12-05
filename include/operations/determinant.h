/**
 * @file determinant.h
 * @brief Matrix determinant operation
 * 
 * * Supports matrices from 2x2 to 6x6:
 * - 2x2, 3x3: Direct formulas (optimized)
 * - 4x4, 5x5, 6x6: LU decomposition with partial pivoting
 */

#ifndef DETERMINANT_H
#define DETERMINANT_H

#include "operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Determinant operation instance
 * Calculates determinant of square matrices (2x2 to 6x6).
 * Input: Flat array representing matrix in row-major order.
 * Example 2x2: [a b c d] represents [a b]
 *                                   [c d]
 */
extern Operation operation_determinant;

#ifdef __cplusplus
}
#endif

#endif // DETERMINANT_H