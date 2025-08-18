/**
 * @file log.c
 * @brief Implements logging functionality for operations.
 *
 * Each operation executed is logged with timestamp, operation name, and result.
 * Logs are written to "operations.log" in append mode.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#include <stdio.h>
#include <time.h>
#include "operations.h"
#include "log.h"

/**
 * @brief Logs the result of an operation to a file.
 *
 * Opens "operations.log" in append mode and writes a formatted line:
 * [YYYY-MM-DD HH:MM:SS] Operation: <name>, Result: <value>
 *
 * @param operation_id Index of the operation executed.
 * @param result Result of the operation.
 */
void log_result(int operation_id, int result) 
{
    FILE *fp = fopen("operations.log", "a");
    if(fp == NULL) 
        return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_str[26];
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "[%s] Operation: %s, Result: %d\n",
            time_str, get_operation_name(operation_id), result);

    fclose(fp);
}

