/**
 * @file log.h
 * @brief Logging functions for operations.
 *
 * Provides functionality to log operation execution,
 * including timestamp, operation name, and result.
 *
 * Logs are appended to "operations.log" file.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#ifndef LOG_H
#define LOG_H

/**
 * @brief Logs the result of an operation to file and optionally console.
 *
 * @param operation_id Index of the operation executed (used to get operation name).
 * @param result Result of the operation.
 *
 * Appends a line to "operations.log" with the timestamp,
 * operation name, and result.
 */
void log_result(int operation_id, int result);

#endif /* LOG_H */

