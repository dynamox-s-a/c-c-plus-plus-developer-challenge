#include "operations.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static void swap_rows(double *matrix, size_t row_a, size_t row_b, size_t dimension)
{
    if (row_a == row_b)
    {
        return;
    }

    for (size_t column = 0; column < dimension; ++column)
    {
        double temp = matrix[row_a * dimension + column];
        matrix[row_a * dimension + column] = matrix[row_b * dimension + column];
        matrix[row_b * dimension + column] = temp;
    }
}

esp_err_t operations_add(double lhs, double rhs, double *result)
{
    if (result == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    *result = lhs + rhs;
    return ESP_OK;
}

esp_err_t operations_matrix_determinant(const double *matrix, size_t dimension, double *result)
{
    if (matrix == NULL || result == NULL || dimension == 0U)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (dimension == 1U)
    {
        *result = matrix[0];
        return ESP_OK;
    }

    const size_t elements = dimension * dimension;
    double *working_matrix = malloc(elements * sizeof(double));
    if (working_matrix == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    memcpy(working_matrix, matrix, elements * sizeof(double));

    double determinant = 1.0;
    int sign = 1;

    for (size_t pivot_index = 0; pivot_index < dimension; ++pivot_index)
    {
        size_t pivot_row = pivot_index;
        double pivot_value = fabs(working_matrix[pivot_index * dimension + pivot_index]);

        for (size_t candidate_row = pivot_index + 1; candidate_row < dimension; ++candidate_row)
        {
            double candidate_value = fabs(working_matrix[candidate_row * dimension + pivot_index]);
            if (candidate_value > pivot_value)
            {
                pivot_value = candidate_value;
                pivot_row = candidate_row;
            }
        }

        if (pivot_value <= 1e-12)
        {
            determinant = 0.0;
            break;
        }

        if (pivot_row != pivot_index)
        {
            swap_rows(working_matrix, pivot_index, pivot_row, dimension);
            sign = -sign;
        }

        const double pivot = working_matrix[pivot_index * dimension + pivot_index];
        determinant *= pivot;

        for (size_t row = pivot_index + 1; row < dimension; ++row)
        {
            const double factor = working_matrix[row * dimension + pivot_index] / pivot;
            if (factor == 0.0)
            {
                continue;
            }

            working_matrix[row * dimension + pivot_index] = 0.0;
            for (size_t column = pivot_index + 1; column < dimension; ++column)
            {
                working_matrix[row * dimension + column] -= factor * working_matrix[pivot_index * dimension + column];
            }
        }
    }

    free(working_matrix);
    *result = determinant * (double)sign;
    return ESP_OK;
}