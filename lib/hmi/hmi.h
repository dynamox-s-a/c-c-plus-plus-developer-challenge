/**
 * @file hmi.h
 * @brief Human-Machine Interface library.
 *
 * Provides a console-based menu system for selecting and
 * executing operations. Handles user input, dispatches
 * operations, and displays results.
 *
 * Future improvements:
 *   - Graphical UI (e.g., using ncurses or a lightweight GUI toolkit)
 *   - Command-line argument mode (non-interactive / batch)
 *   - Localization / multi-language support
 */

#ifndef HMI_H
#define HMI_H

/**
 * @brief Display the main menu and return the user's choice.
 *
 * @return Selected menu option (1-based), or 0 on invalid input.
 */
int hmi_show_main_menu(void);

/**
 * @brief Execute a scalar/array operation selected by the user.
 *
 * Prompts for input mode (single value or array), reads values,
 * executes the operation, displays the result, and logs it.
 *
 * @param[in] op_index  Zero-based index of the operation in the registry.
 */
void hmi_execute_array_op(int op_index);

/**
 * @brief Execute a matrix operation selected by the user.
 *
 * Prompts for matrix dimensions, reads the matrix,
 * executes the operation, displays the result, and logs it.
 *
 * @param[in] op_index  Zero-based index of the operation in the registry.
 */
void hmi_execute_matrix_op(int op_index);

/**
 * @brief Run the main HMI loop until the user exits.
 */
void hmi_run(void);

/**
 * @brief Safely read a double from stdin with error handling.
 *
 * @param[out] value  Pointer to store the read value.
 * @return 0 on success, -1 on invalid input.
 */
int hmi_read_double(double *value);

/**
 * @brief Safely read a positive integer from stdin with error handling.
 *
 * @param[out] value  Pointer to store the read value.
 * @return 0 on success, -1 on invalid input.
 */
int hmi_read_positive_int(int *value);

#endif /* HMI_H */
