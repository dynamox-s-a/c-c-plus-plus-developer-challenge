/**
 * @file ui.h
 * @brief User interface functions for input and menu handling.
 *
 * Provides functions to get user choices, numeric input,
 * array input, and matrix input for operations.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#ifndef UI_H
#define UI_H

#include "operations.h"

/// Maximum number of elements allowed in array input
#define MAX_ARRAY_SIZE 50

/**
 * @brief Displays the menu and reads the user's choice.
 *
 * Menu options:
 * 1. Addition (2 integers)
 * 2. Average (2 integers)
 * 3. Mode (array input)
 * 4. Determinant (square matrix)
 * 5. Exit
 *
 * Handles invalid input and ensures choice is within range.
 *
 * @return Choice index (0-based)
 */
int get_user_choice(void);

/**
 * @brief Reads two integer numbers from user input.
 *
 * Handles invalid input and retries until valid integers are provided.
 *
 * @param a Pointer to first integer
 * @param b Pointer to second integer
 */
void get_two_numbers_input(int *a, int *b);

/**
 * @brief Reads an array of integers from user input.
 *
 * Prompts for array size (1..MAX_ARRAY_SIZE) and elements.
 * Handles invalid input.
 *
 * @param arr Array to store the elements
 * @param size Pointer to store the array size
 */
void get_array_input(int arr[], int *size);

/**
 * @brief Reads a square matrix from user input.
 *
 * Prompts for matrix size (1..MAX_MATRIX_SIZE) and elements.
 * Handles invalid input.
 *
 * @param matrix 2D array to store matrix elements
 * @param size Pointer to store matrix size
 */
void get_matrix_input(int matrix[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], int *size);

#endif /* UI_H */

