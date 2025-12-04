/**
 * @file logger.c
 * @brief Implementation of persistent logging system
 * 
 * Logs operations to CSV file in logs/operations.csv
 * Creates directory and file if they don't exist.
 */

#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define LOG_DIR "../logs"
#define LOG_FILE "../logs/operations.csv"
#define MAX_INPUT_STR 512

static FILE* log_file = NULL;

/**
 * @brief Get current timestamp as string
 * @param buffer Buffer to store timestamp
 * @param size Buffer size
 */
static void get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

/**
 * @brief Create logs directory if it doesn't exist
 */
static int create_log_directory(void) {
    struct stat st = {0};
    
    if (stat(LOG_DIR, &st) == -1) {
        // Directory doesn't exist, create it
        if (mkdir(LOG_DIR, 0755) != 0) {
            fprintf(stderr, "ERROR: Failed to create log directory: %s\n", 
                    strerror(errno));
            return -1;
        }
        printf("Created log directory: %s\n", LOG_DIR);
    }
    
    return 0;
}

/**
 * @brief Check if log file exists
 * @return 0 The log file does not exist or cannot be read
 *         1 The log file exists (and is readable).
 */
static int log_file_exists(void) {
    FILE* f = fopen(LOG_FILE, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

/**
 * @brief Initialize logger
 */
int logger_init(void) {
    // Create directory if needed
    if (create_log_directory() != 0) {
        return -1;
    }
    
    // Check if file exists (to know if we need header)
    int file_exists = log_file_exists();
    
    // Open file in append mode. If the file does NOT exist, it is created.
    log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        fprintf(stderr, "ERROR: Failed to open log file: %s\n", strerror(errno));
        return -1;
    }
    
    // Write header if new file
    if (!file_exists) {
        fprintf(log_file, "timestamp,operation,input,result,status\n");
        fflush(log_file);
        printf("Created new log file: %s\n", LOG_FILE);
    }
    else {
        printf("Using existing log file: %s\n", LOG_FILE);
    }
    
    return 0;
}

/**
 * @brief Format input values as string
 */
static void format_input_values(const double* values, int count, char* buffer, size_t size) {
    if (count == 1) {
        snprintf(buffer, size, "%.6g", values[0]);
    }
    else {
        size_t pos = 0;
        pos += snprintf(buffer + pos, size - pos, "[");
        
        for (int i = 0; i < count && pos < size - 20; i++) {
            if (i > 0) {
                pos += snprintf(buffer + pos, size - pos, " ");
            }
            pos += snprintf(buffer + pos, size - pos, "%.6g", values[i]);
        }
        
        snprintf(buffer + pos, size - pos, "]");
    }
}

/**
 * @brief Log successful operation
 */
int logger_log_success(const char* operation_name, 
                       const double* values, 
                       int count, 
                       double result) {
    if (log_file == NULL) {
        fprintf(stderr, "WARNING: Logger not initialized\n");
        return -1;
    }
    
    // Get timestamp
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Format input values
    char input_str[MAX_INPUT_STR];
    format_input_values(values, count, input_str, sizeof(input_str));
    
    // Write log entry
    fprintf(log_file, "%s,%s,\"%s\",%.6g,success\n",
            timestamp,
            operation_name,
            input_str,
            result);
    
    // Flush to ensure it's written immediately
    fflush(log_file);
    
    return 0;
}

/**
 * @brief Log failed operation
 */
int logger_log_error(const char* operation_name, const char* error_message) {
    if (log_file == NULL) {
        fprintf(stderr, "WARNING: Logger not initialized\n");
        return -1;
    }
    
    // Get timestamp
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Write log entry
    fprintf(log_file, "%s,%s,\"%s\",,error\n",
            timestamp,
            operation_name,
            error_message);
    
    // Flush to ensure it's written immediately
    fflush(log_file);
    
    return 0;
}

/**
 * @brief Close logger
 */
void logger_close(void) {
    if (log_file != NULL) {
        fflush(log_file);
        fclose(log_file);
        log_file = NULL;
        printf("Logger closed\n");
    }
}