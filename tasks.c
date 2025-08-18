/**
 * @file tasks.c
 * @brief Task implementations for the simulated RTOS.
 *
 * This module defines the tasks used in the cooperative scheduler:
 *  - @ref task_ui : Handles user input and prepares operation data.
 *  - @ref task_operations : Executes the selected operation.
 *  - @ref task_logging : Logs the operation and displays results.
 *
 * Each task is designed to be short and non-blocking to fit within
 * the cooperative round-robin scheduler model.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#include "tasks.h"
#include "ui.h"
#include "operations.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

/* === Internal State Variables === */
static int user_choice = 0;
static int add_a = 0, add_b = 0;
static int arr[MAX_ARRAY_SIZE], arr_size = 0;
static int matrix[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], matrix_size = 0;
static int result = 0;
static int log_ready = 0;

/**
 * @brief Task for user interface handling.
 *
 * This task queries the user for an operation choice and
 * gathers the necessary input depending on the selection:
 *  - Choice 0 or 1 ? two numbers
 *  - Choice 2 ? array input
 *  - Choice 3 ? matrix input
 *  - Choice 4 ? exit application
 */
void task_ui(void) 
{
    user_choice = get_user_choice();

    if (user_choice == 0 || user_choice == 1) 
	{
        get_two_numbers_input(&add_a, &add_b);
    }
    else if (user_choice == 2) 
	{
        get_array_input(arr, &arr_size);
    }
    else if (user_choice == 3) 
	{
        get_matrix_input(matrix, &matrix_size);
    }
    else if (user_choice == 4) 
	{
        printf("Exiting...\n");
        exit(0);
    }
}

/**
 * @brief Task for executing operations.
 *
 * Based on the user choice set in @ref task_ui,
 * this task performs:
 *  - Addition
 *  - Average
 *  - Mode
 *  - Determinant
 *
 * The result is stored in an internal variable and
 * marked as ready for logging.
 */
void task_operations(void) 
{
    if (user_choice == 0)
        result = execute_operation(0, add_a, add_b);
    else if (user_choice == 1)
        result = execute_operation(1, add_a, add_b);
    else if (user_choice == 2)
        result = calculate_mode(arr, arr_size);
    else if (user_choice == 3)
        result = calculate_determinant(matrix, matrix_size);

    log_ready = 1;
}

/**
 * @brief Task for logging and displaying results.
 *
 * If a result is ready (set by @ref task_operations),
 * this task:
 *  - Logs the operation with timestamp and result.
 *  - Prints the result to the user.
 */
void task_logging(void) 
{
    if (log_ready) 
	{
        log_result(user_choice, result);
        printf("Result: %d\n", result);
        log_ready = 0;
    }
}

