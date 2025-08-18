/**
 * @file operations.c
 * @brief Implements mathematical operations (addition, average, mode, determinant) 
 *        and provides a plug-in style system to register and execute them dynamically.
 *
 * Determinant calculation method:
 *   -  Gaussian elimination with row swaps (O(n³)), optimized for performance
 *     and memory usage in embedded environments.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#include "operations.h"
#include <string.h>

static Operation operations[MAX_OPERATIONS]; /**< Registry of available operations */
static int operation_count = 0;              /**< Current number of registered operations */

/**
 * @brief Adds two integers.
 * @param a First integer.
 * @param b Second integer.
 * @return Sum of a and b.
 */
int add_two_numbers(int a, int b) 
{ 
    return a + b; 
}

/**
 * @brief Calculates the average (integer division) of two integers.
 * @param a First integer.
 * @param b Second integer.
 * @return Integer average of a and b.
 */
int calculate_average(int a, int b) 
{ 
    return (a + b) / 2; 
}

/**
 * @brief Calculates the mode (most frequent element) of an integer array.
 * @param arr Array of integers.
 * @param size Number of elements in the array.
 * @return Mode value of the array. If multiple, returns the first found.
 */
int calculate_mode(int arr[], int size) 
{
    int max_count = 0, mode = arr[0];
    int i, j, count;
    for (i = 0; i < size; i++) 
    {
        count = 0;
        for (j = 0; j < size; j++) 
            if (arr[j] == arr[i]) 
                count++;
        if (count > max_count) 
        { 
            max_count = count; 
            mode = arr[i]; 
        }
    }
    return mode;
}

/**
 * @brief Calculates the determinant of a square matrix using Gaussian elimination.
 *
 * This method is significantly faster (O(n³)) than Laplace expansion (O(n!)),
 * making it more suitable for embedded systems with limited resources.
 *
 * @param m Input matrix (up to MAX_MATRIX_SIZE).
 * @param n Dimension of the matrix (n x n).
 * @return Determinant value of the matrix.
 */
int calculate_determinant(int m[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], int n) 
{
    int i, j, k;
    double ratio;
    double temp[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    double det = 1.0;

    /* Copy matrix to temporary (to avoid modifying original) */
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            temp[i][j] = m[i][j];

    /* Perform Gaussian elimination */
    for (i = 0; i < n; i++) 
    {
        if (temp[i][i] == 0.0) 
        {
            /* Swap with a non-zero row below */
            for (j = i + 1; j < n; j++) 
            {
                if (temp[j][i] != 0.0) 
                {
                    for (k = 0; k < n; k++) 
                    {
                        double swap = temp[i][k];
                        temp[i][k] = temp[j][k];
                        temp[j][k] = swap;
                    }
                    det *= -1; /* Row swap changes determinant sign */
                    break;
                }
            }
            if (j == n) 
                return 0; /* Singular matrix */
        }

        /* Eliminate below diagonal */
        for (j = i + 1; j < n; j++) 
        {
            ratio = temp[j][i] / temp[i][i];
            for (k = 0; k < n; k++) 
                temp[j][k] -= ratio * temp[i][k];
        }
    }

    /* Determinant is product of diagonal elements */
    for (i = 0; i < n; i++)
        det *= temp[i][i];

    return (int)det;
}

/**
 * @brief Registers a new operation in the system.
 * @param op Operation structure containing name and function pointer.
 */
void register_operation(Operation op) 
{
    if (operation_count < MAX_OPERATIONS) 
        operations[operation_count++] = op;
}

/**
 * @brief Executes a registered operation by index.
 * @param index Index of the operation in the registry.
 * @param a First integer input.
 * @param b Second integer input.
 * @return Result of the operation. Returns 0 if index is invalid or function is NULL.
 */
int execute_operation(int index, int a, int b) 
{
    if (index >= 0 && index < operation_count && operations[index].func != NULL)
        return operations[index].func(a, b);
    return 0;
}

/**
 * @brief Gets the total number of registered operations.
 * @return Number of registered operations.
 */
int get_operations_count(void) 
{ 
    return operation_count; 
}

/**
 * @brief Gets the name of a registered operation by index.
 * @param index Index of the operation.
 * @return Operation name, or NULL if index is invalid.
 */
const char* get_operation_name(int index) 
{
    if (index >= 0 && index < operation_count) 
        return operations[index].name;
    return NULL;
}

