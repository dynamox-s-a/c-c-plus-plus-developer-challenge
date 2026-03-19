/**
 * @file matrix.c
 * @brief Implementation of matrix operations library.
 */

#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tolerance for floating-point zero comparison */
#define EPSILON 1e-12

matrix_status_t matrix_alloc(double ***out, size_t rows, size_t cols)
{
    if (out == NULL) {
        return MATRIX_ERR_NULL_PTR;
    }
    if (rows == 0 || cols == 0) {
        return MATRIX_ERR_INVALID_DIM;
    }

    double **mat = (double **)malloc(rows * sizeof(double *));
    if (mat == NULL) {
        return MATRIX_ERR_ALLOC;
    }

    /*
     * Allocate a single contiguous block for all elements.
     * This improves cache locality and simplifies deallocation.
     */
    double *block = (double *)calloc(rows * cols, sizeof(double));
    if (block == NULL) {
        free(mat);
        return MATRIX_ERR_ALLOC;
    }

    for (size_t i = 0; i < rows; i++) {
        mat[i] = block + i * cols;
    }

    *out = mat;
    return MATRIX_OK;
}

void matrix_free(double **mat, size_t rows)
{
    if (mat == NULL) {
        return;
    }
    /* Free the contiguous data block (pointed to by row 0) */
    free(mat[0]);
    free(mat);
    (void)rows; /* rows not needed thanks to contiguous allocation */
}

matrix_status_t matrix_read(double **mat, size_t rows, size_t cols)
{
    if (mat == NULL) {
        return MATRIX_ERR_NULL_PTR;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            printf("  [%zu][%zu]: ", i + 1, j + 1);
            if (scanf("%lf", &mat[i][j]) != 1) {
                fprintf(stderr, "Error: invalid numeric input.\n");
                /* Clear the invalid input from stdin */
                int c;
                while ((c = getchar()) != '\n' && c != EOF) { }
                return MATRIX_ERR_INVALID_DIM;
            }
        }
    }
    return MATRIX_OK;
}

void matrix_print(double **mat, size_t rows, size_t cols)
{
    if (mat == NULL) {
        return;
    }

    printf("\n");
    for (size_t i = 0; i < rows; i++) {
        printf("  |");
        for (size_t j = 0; j < cols; j++) {
            printf(" %10.4f", mat[i][j]);
        }
        printf(" |\n");
    }
    printf("\n");
}

matrix_status_t matrix_determinant(double **mat, size_t n, double *det)
{
    if (mat == NULL || det == NULL) {
        return MATRIX_ERR_NULL_PTR;
    }
    if (n == 0) {
        return MATRIX_ERR_INVALID_DIM;
    }

    /* Special case: 1x1 */
    if (n == 1) {
        *det = mat[0][0];
        return MATRIX_OK;
    }

    /* Special case: 2x2 */
    if (n == 2) {
        *det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
        return MATRIX_OK;
    }

    /*
     * General case: LU decomposition with partial pivoting.
     * We work on a copy to preserve the original matrix.
     */
    double **tmp = NULL;
    matrix_status_t status = matrix_alloc(&tmp, n, n);
    if (status != MATRIX_OK) {
        return status;
    }

    /* Copy original matrix into tmp */
    for (size_t i = 0; i < n; i++) {
        memcpy(tmp[i], mat[i], n * sizeof(double));
    }

    double determinant = 1.0;
    int sign = 1;

    for (size_t col = 0; col < n; col++) {
        /* Partial pivoting: find row with largest absolute value in column */
        size_t max_row = col;
        double max_val = fabs(tmp[col][col]);

        for (size_t row = col + 1; row < n; row++) {
            double val = fabs(tmp[row][col]);
            if (val > max_val) {
                max_val = val;
                max_row = row;
            }
        }

        /* Check for singular matrix */
        if (max_val < EPSILON) {
            *det = 0.0;
            matrix_free(tmp, n);
            return MATRIX_OK;
        }

        /* Swap rows if necessary */
        if (max_row != col) {
            double *swap = tmp[col];
            tmp[col] = tmp[max_row];
            tmp[max_row] = swap;
            sign = -sign;
        }

        determinant *= tmp[col][col];

        /* Eliminate below */
        for (size_t row = col + 1; row < n; row++) {
            double factor = tmp[row][col] / tmp[col][col];
            for (size_t k = col + 1; k < n; k++) {
                tmp[row][k] -= factor * tmp[col][k];
            }
            tmp[row][col] = 0.0;
        }
    }

    *det = determinant * sign;
    matrix_free(tmp, n);
    return MATRIX_OK;
}
