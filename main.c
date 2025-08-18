/**
 * @file main.c
 * @brief Entry point of the Dynamox C/C++ Developer Challenge application.
 *
 *
 * @author Sergio
 * @date 2025-08-17
 */

#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "tasks.h"
#include "operations.h"

/**
 * @brief Application entry point.
 *
 * This function initializes the system by:
 *  - Registering the available operations (addition, average, mode, determinant).
 *  - Registering the tasks for user interface, operations, and logging.
 *  - Starting the scheduler loop which simulates task scheduling.
 *
 * @return int Returns 0 upon normal termination.
 */
int main(void) 
{
    Operation op_add = {"Addition", add_two_numbers};
    Operation op_avg = {"Average", calculate_average};
    Operation op_mode = {"Mode", NULL};
    Operation op_det = {"Determinant (matrix)", NULL};

    register_operation(op_add);
    register_operation(op_avg);
    register_operation(op_mode);
    register_operation(op_det);

    add_task(task_ui);
    add_task(task_operations);
    add_task(task_logging);

    scheduler_loop();

    return 0;
}

