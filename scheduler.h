/**
 * @file scheduler.h
 * @brief Cooperative round-robin task scheduler interface.
 *
 * This header defines the interface for a minimal cooperative
 * scheduler used to simulate a simple RTOS in embedded systems.
 *
 * Tasks are simple functions of type `void task(void)`. They are
 * registered using @ref add_task and executed continuously in a
 * round-robin manner by @ref scheduler_loop.
 *
 * This approach is widely used in baremetal embedded systems where:
 *   - Resource constraints make a full RTOS infeasible.
 *   - Deterministic cooperative task execution is sufficient.
 *
 * @note Since this scheduler is cooperative, tasks must be written
 *       to complete quickly and avoid blocking execution.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

/**
 * @brief Function pointer type for tasks.
 *
 * A task is any function with no parameters and no return value,
 * e.g.:
 * @code
 * void my_task(void) 
 * {
 *     // do something
 * }
 * @endcode
 */
typedef void (*task_func)(void);

/**
 * @brief Registers a task with the scheduler.
 *
 * Tasks are executed in the order they are registered.
 *
 * @param task Function pointer to the task to be added.
 */
void add_task(task_func task);

/**
 * @brief Starts the scheduler loop.
 *
 * Executes all registered tasks in a round-robin loop.
 * This function never returns.
 */
void scheduler_loop(void);

#endif /* SCHEDULER_H */

