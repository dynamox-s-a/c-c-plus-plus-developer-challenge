/**
 * @file logger.h
 * @brief Persistent operation logging system
 * 
 * Logs all operations to CSV file for audit and analysis.
 * Format: timestamp,operation,input,result,status
 */

#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the logger system
 * @return 0 on success, -1 on failure
 */
int logger_init(void);

/**
 * @brief Log a successful operation
 * @param operation_name Name of the operation
 * @param values Input values array
 * @param count Number of input values
 * @param result Operation result
 * @return 0 on success, -1 on failure
 */
int logger_log_success(const char* operation_name, 
                       const double* values, 
                       int count, 
                       double result);

/**
 * @brief Log a failed operation
 * @param operation_name Name of the operation
 * @param error_message Error description
 * @return 0 on success, -1 on failure
 */
int logger_log_error(const char* operation_name, const char* error_message);

/**
 * @brief Close the logger and flush any pending writes
 */
void logger_close(void);

#ifdef __cplusplus
}
#endif

#endif // LOGGER_H