/**
 * @file operations.h
 * @brief Definitions for mathematical operations and operation management system.
 *
 * This header provides the interface for basic mathematical operations (addition,
 * average, mode, determinant) and a plug-in style registration system that allows
 * new operations to be added dynamically.
 *
 * Determinant Calculation:
 *   - Implemented using Gaussian elimination (O(n³)).
 *
 * @author Sergio
 * @date 2025-08-17
 */

#ifndef OPERATIONS_H
#define OPERATIONS_H

/** @brief Maximum allowed matrix dimension for determinant calculation (n x n). */
#define MAX_MATRIX_SIZE 10

/** @brief Maximum number of operations that can be registered. */
#define MAX_OPERATIONS 10

/**
 * @struct Operation
 * @brief Represents a mathematical operation that can be executed.
 *
 * Each operation has:
 *   - A descriptive name.
 *   - A function pointer to its implementation (binary operations).
 */
typedef struct 
{
    char name[30];                        /**< Name of the operation (e.g., "Addition"). */
    int (*func)(int a, int b);            /**< Binary function pointer (e.g., Addition, Average). */
} Operation;

/* ==== Operation Functions ==== */

/**
 * @brief Adds two numbers.
 * @param a First number.
 * @param b Second number.
 * @return Sum of a and b.
 */
int add_two_numbers(int a, int b);

/**
 * @brief Calculates the average of two numbers.
 * @param a First number.
 * @param b Second number.
 * @return Integer average of a and b.
 */
int calculate_average(int a, int b);

/**
 * @brief Calculates the mode of an array of integers.
 * @param arr Input array.
 * @param size Number of elements in array.
 * @return Mode of the array.
 */
int calculate_mode(int arr[], int size);

/**
 * @brief Calculates the determinant of a square matrix using Gaussian elimination.
 * @param matrix Input matrix (up to MAX_MATRIX_SIZE x MAX_MATRIX_SIZE).
 * @param n Dimension of the matrix (n x n).
 * @return Determinant of the matrix.
 */
int calculate_determinant(int matrix[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], int n);

/* ==== Plug-in System Functions ==== */

/**
 * @brief Registers a new operation in the system.
 * @param op Operation to register.
 */
void register_operation(Operation op);

/**
 * @brief Executes a registered operation by index.
 * @param index Index of the operation in the list.
 * @param a First operand.
 * @param b Second operand.
 * @return Result of the operation, or 0 if invalid.
 */
int execute_operation(int index, int a, int b);

/**
 * @brief Gets the number of registered operations.
 * @return Count of registered operations.
 */
int get_operations_count(void);

/**
 * @brief Gets the name of a registered operation by index.
 * @param index Index of the operation in the list.
 * @return Name of the operation, or NULL if invalid.
 */
const char* get_operation_name(int index);

#endif /* OPERATIONS_H */

