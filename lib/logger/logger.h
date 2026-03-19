/**
 * @file logger.h
 * @brief Persistent operation log library.
 *
 * Logs every operation execution to a file with timestamp,
 * operation name, inputs, and result. Provides functionality
 * to view past logs.
 *
 * Future improvements:
 *   - Log rotation (limit file size)
 *   - Binary log format for embedded targets with limited storage
 *   - Log filtering by operation type or date range
 *   - Export to CSV
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>

/** Default log file path */
#define LOG_FILE_PATH "operations.log"

/** Maximum length of a single log entry */
#define LOG_ENTRY_MAX_LEN 512

/**
 * @brief Initialize the logger. Creates the log file if it does not exist.
 *
 * @return 0 on success, -1 on failure.
 */
int logger_init(void);

/**
 * @brief Log a scalar/array operation.
 *
 * @param[in] op_name  Name of the operation.
 * @param[in] values   Input values array.
 * @param[in] count    Number of input values.
 * @param[in] result   Result of the operation.
 */
void logger_log_array_op(const char *op_name, const double *values,
                         size_t count, double result);

/**
 * @brief Log a matrix operation.
 *
 * @param[in] op_name  Name of the operation.
 * @param[in] rows     Number of rows.
 * @param[in] cols     Number of columns.
 * @param[in] result   Result of the operation.
 */
void logger_log_matrix_op(const char *op_name, size_t rows, size_t cols,
                          double result);

/**
 * @brief Display the last N log entries to stdout.
 *
 * @param[in] count  Number of entries to display (0 = all).
 */
void logger_show(size_t count);

/**
 * @brief Close the logger and release resources.
 */
void logger_close(void);

#endif /* LOGGER_H */
