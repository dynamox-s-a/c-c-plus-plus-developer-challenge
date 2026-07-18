#include "operations.h"

double calculate_sum(double* values, int size) {
    double sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += values[i];
    }
    return sum;
}

double calculate_determinant(double* matrix, int size) {
    // Exemplo básico para matriz 2x2
    if (size == 2) {
        return matrix[0] * matrix[3] - matrix[1] * matrix[2];
    }
    // Implementar para matrizes maiores
    return 0;
}