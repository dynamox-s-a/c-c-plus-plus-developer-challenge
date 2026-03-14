#include <math.h>
#include "determinant.h"

/* Por que eliminacao de Gauss e nao expansao por cofatores (Laplace)?
 *
 * Expansao por cofatores tem complexidade O(n!):
 *   n=5  ->   120 operacoes
 *   n=10 -> 3.628.800 operacoes
 *
 * Eliminacao de Gauss tem complexidade O(n^3):
 *   n=5  ->   125 operacoes
 *   n=10 ->  1000 operacoes
 *
 * Para qualquer uso pratico com matrizes maiores que 3x3,
 * Gauss e a escolha correta em termos de desempenho. */

static void swap_rows(double matrix[MAX_MATRIX_DIMENSION][MAX_MATRIX_DIMENSION],
                      int row_a, int row_b, int n) {
    int col;
    for (col = 0; col < n; col++) {
        double tmp = matrix[row_a][col];
        matrix[row_a][col] = matrix[row_b][col];
        matrix[row_b][col] = tmp;
    }
}

int calculate_determinant_gauss(int n,
                                double matrix[MAX_MATRIX_DIMENSION][MAX_MATRIX_DIMENSION],
                                double *determinant) {
    int pivot, row, col;
    int sign = 1;
    double det = 1.0;

    if (determinant == 0) return DET_NULL_OUTPUT;
    if (n <= 0 || n > MAX_MATRIX_DIMENSION) return DET_INVALID_DIMENSION;

    for (pivot = 0; pivot < n; pivot++) {
        int best_row = pivot;

        for (row = pivot + 1; row < n; row++) {
            if (fabs(matrix[row][pivot]) > fabs(matrix[best_row][pivot]))
                best_row = row;
        }

        if (fabs(matrix[best_row][pivot]) < 1e-12) {
            *determinant = 0.0;
            return DET_SUCCESS;
        }

        if (best_row != pivot) {
            swap_rows(matrix, pivot, best_row, n);
            sign *= -1;
        }

        for (row = pivot + 1; row < n; row++) {
            double factor = matrix[row][pivot] / matrix[pivot][pivot];
            for (col = pivot; col < n; col++)
                matrix[row][col] -= factor * matrix[pivot][col];
        }
    }

    for (pivot = 0; pivot < n; pivot++)
        det *= matrix[pivot][pivot];

    *determinant = det * sign;
    return DET_SUCCESS;
}