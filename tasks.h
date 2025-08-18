/**
 * @file tasks.h
 * @brief Task definitions for the cooperative scheduler.
 *
 * This header defines the application tasks used by the scheduler:
 *  - @ref task_ui : Collects user input.
 *  - @ref task_operations : Executes selected operations.
 *  - @ref task_logging : Logs and displays results.
 *
 * Tasks are executed in a round-robin manner by the scheduler.
 * Each task must be short and non-blocking to ensure responsiveness.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#ifndef TASKS_H
#define TASKS_H

/**
 * @brief User Interface Task.
 *
 * Reads the user menu choice and collects required input values
 * (numbers, array, or matrix).
 * If the exit option is chosen, the system will terminate.
 */
void task_ui(void);

/**
 * @brief Operations Execution Task.
 *
 * Executes the operation selected by the user and
 * stores the result for logging.
 */
void task_operations(void);

/**
 * @brief Logging Task.
 *
 * Logs the operation, timestamp, and result.
 * Also prints the result to the user interface.
 */
void task_logging(void);

#endif /* TASKS_H */

