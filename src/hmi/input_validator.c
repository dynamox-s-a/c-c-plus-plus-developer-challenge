/**
 * @file input_validator.c
 * @brief Implementation of input validation utilities
 */

#include "hmi/input_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Clear input buffer
 */
void input_clear_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Check if input was truncated by fgets
 * @param line Buffer with input
 * @param size Buffer size
 * @return 1 if truncated, 0 if complete
 */
static int is_input_truncated(const char* line, size_t size) {
    size_t len = strlen(line);
    
    // If buffer is full and doesn't end with newline, it was truncated
    if (len == size - 1 && line[len - 1] != '\n') {
        return 1;
    }
    
    return 0;
}


/**
 * @brief Read integer with validation
 */
int input_read_int(const char* prompt, int min, int max, int* value) {
    char line[MAX_INPUT_LINE];
    
    printf("%s", prompt);
    
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return -1;
    }

    // Detect truncation
    if (is_input_truncated(line, sizeof(line))) {
        fprintf(stderr, "ERROR: Input too long (max %d characters)\n", MAX_INPUT_LINE-1);
        input_clear_buffer();
        return -1;
    }
    
    // Try to parse integer
    char* endptr;
    long val = strtol(line, &endptr, 10);
    
    // Check if conversion was successful
    if (endptr == line || (*endptr != '\n' && *endptr != '\0')) {
        fprintf(stderr, "ERROR: Invalid number format\n");
        return -1;
    }
    
    // Check range
    if (val < min || val > max) {
        fprintf(stderr, "ERROR: Value must be between %d and %d\n", min, max);
        return -1;
    }
    
    *value = (int)val;
    return 0;
}

/**
 * @brief Read double with validation
 */
int input_read_double(const char* prompt, double* value) {
    char line[MAX_INPUT_LINE];
    
    printf("%s", prompt);
    
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return -1;
    }

    // Detect truncation
    if (is_input_truncated(line, sizeof(line))) {
        fprintf(stderr, "ERROR: Input too long (max %d characters)\n", MAX_INPUT_LINE-1);
        input_clear_buffer();
        return -1;
    }
    
    // Try to parse double
    char* endptr;
    double val = strtod(line, &endptr);
    
    // Check if conversion was successful
    if (endptr == line || (*endptr != '\n' && *endptr != '\0')) {
        fprintf(stderr, "ERROR: Invalid number format\n");
        return -1;
    }
    
    *value = val;
    return 0;
}

/**
 * @brief Read array of doubles
 */
int input_read_array(const char* prompt, double* values, int max_count, int* count) {
    char line[MAX_INPUT_LINE];
    
    printf("%s", prompt);
    
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return -1;
    }
    
    // Detect truncation
    if (is_input_truncated(line, sizeof(line))) {
        fprintf(stderr, "ERROR: Input too long (max %d characters)\n", MAX_INPUT_LINE-1);
        fprintf(stderr, "       Try entering fewer values or use scientific notation\n");
        fprintf(stderr, "       Example: 1e6 instead of 1000000\n");
        input_clear_buffer();
        return -1;
    }

    // Parse space or comma-separated values
    *count = 0;
    char* token = strtok(line, " ,\t\n");
    
    while (token != NULL && *count < max_count) {
        char* endptr;
        double val = strtod(token, &endptr);
        
        if (endptr == token || *endptr != '\0') {
            fprintf(stderr, "ERROR: Invalid number '%s'\n", token);
            return -1;
        }
        
        values[*count] = val;
        (*count)++;
        
        token = strtok(NULL, " ,\t\n");
    }

    // Warn if hit max values limit
    if (*count == max_count && token != NULL) {
        fprintf(stderr, "WARNING: Input truncated to %d values (limit reached)\n", max_count);
        fprintf(stderr, "         Remaining values ignored\n");
    }
    
    if (*count == 0) {
        fprintf(stderr, "ERROR: No values entered\n");
        return -1;
    }
    
    return 0;
}