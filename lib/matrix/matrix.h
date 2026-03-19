/**
 * @file matrix.h
 * @brief Matrix operations library.
 *
 * Provides matrix allocation, deallocation, input, display,
 * and determinant calculation (LU-decomposition based, O(n^3)).
 *
 * Future improvements:
 *   - Eigenvalue / eigenvector computation
 *   - Matrix inversion
 *   - Sparse matrix support for memory-constrained environments
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

/** Return codes for matrix operations. */
typedef enum {
    MATRIX_OK = 0,
    MATRIX_ERR_ALLOC,       /**< Memory allocation failure */
    MATRIX_ERR_INVALID_DIM, /**< Invalid dimensions (zero or negative) */
    MATRIX_ERR_NULL_PTR,    /**< NULL pointer argument */
    MATRIX_ERR_SINGULAR     /**< Matrix is singular (det == 0) */
} matrix_status_t;

/**
 * @brief Dynamically allocate a rows x cols matrix (contiguous block).
 *
 * @param[out] out   Pointer to receive the allocated 2D array.
 * @param[in]  rows  Number of rows.
 * @param[in]  cols  Number of columns.
 * @return matrix_status_t
 */
matrix_status_t matrix_alloc(double ***out, size_t rows, size_t cols);

/**
 * @brief Free a matrix previously allocated with matrix_alloc.
 *
 * @param[in] mat   Pointer to the 2D array.
 * @param[in] rows  Number of rows (used to free row pointers).
 */
void matrix_free(double **mat, size_t rows);

/**
 * @brief Read matrix elements from stdin with input validation.
 *
 * @param[in,out] mat   Allocated matrix.
 * @param[in]     rows  Number of rows.
 * @param[in]     cols  Number of columns.
 * @return matrix_status_t
 */
matrix_status_t matrix_read(double **mat, size_t rows, size_t cols);

/**
 * @brief Print matrix to stdout.
 *
 * @param[in] mat   Matrix to display.
 * @param[in] rows  Number of rows.
 * @param[in] cols  Number of columns.
 */
void matrix_print(double **mat, size_t rows, size_t cols);

/**
 * @brief Compute the determinant of a square matrix using LU decomposition.
 *
 * Uses partial pivoting for numerical stability.
 *
 * @param[in]  mat   Square matrix (n x n).
 * @param[in]  n     Dimension.
 * @param[out] det   Computed determinant value.
 * @return matrix_status_t
 */
matrix_status_t matrix_determinant(double **mat, size_t n, double *det);

#endif /* MATRIX_H */
