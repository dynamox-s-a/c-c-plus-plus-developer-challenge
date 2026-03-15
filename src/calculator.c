#include <stddef.h>
#include "calculator.h"

double sum_two_values(double a, double b) {
    return a + b;
}

double multiply_two_values(double a, double b) {
    return a * b;
}

double sum_array(const double *values, int size) {
    double total = 0.0;
    int i = 0;

    if (values == NULL || size <= 0) return 0.0;

    for (i = 0; i < size; i++)
        total += values[i];

    return total;
}

double average_array(const double *values, int size) {
    if (values == NULL || size <= 0) return 0.0;
    return sum_array(values, size) / (double)size;
}
