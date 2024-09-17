#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdio.h>

typedef double (*operation_fn)(double*, int);

double calculate_sum(double* values, int size);
double calculate_determinant(double* matrix, int size);

#endif // OPERATIONS_H