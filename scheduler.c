/**
 * @file scheduler.c
 * @brief Cooperative round-robin task scheduler implementation.
 *
 * This module provides a minimal cooperative scheduler that simulates
 * the behavior of a very simple RTOS task manager. It allows tasks
 * (functions with no arguments and no return) to be registered and
 * repeatedly executed in sequence.
 *
 * Limitations:
 *   - No preemption (tasks must yield quickly).
 *   - No priority management (strict round-robin).
 *   - Maximum number of tasks limited by MAX_TASKS.
 *
 * This approach is lightweight and commonly used in embedded
 * systems with limited resources where a full RTOS is not required.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#include "scheduler.h"

#define MAX_TASKS 10   /**< Maximum number of tasks supported by the scheduler. */

/** @brief Array holding registered tasks. */
static task_func tasks[MAX_TASKS];

/** @brief Number of currently registered tasks. */
static int task_count = 0;

/**
 * @brief Registers a new task with the scheduler.
 *
 * Tasks are simple functions of type `void task(void)`. They are
 * stored in the scheduler's task list and executed in round-robin order.
 *
 * @param task Function pointer to the task.
 */
void add_task(task_func task) 
{
    if(task_count < MAX_TASKS) 
        tasks[task_count++] = task;
}

/**
 * @brief Starts the scheduler loop.
 *
 * Executes all registered tasks in a continuous round-robin loop.
 * This loop never returns and must be terminated by external means
 * (e.g., system reset, power down).
 *
 * @note Since this is a cooperative scheduler, each task must
 *       complete quickly to avoid blocking other tasks.
 */
void scheduler_loop(void) 
{
    int i;
    while(1) 
    {
        for(i = 0; i < task_count; i++) 
            tasks[i]();
    }
}

