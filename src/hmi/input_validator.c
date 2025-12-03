/**
 * @file input_validator.c
 * @brief Implementation of input validation utilities
 */

#include "hmi/input_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_LINE 256

/**
 * @brief Clear input buffer
 */
void input_clear_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
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
    
    if (*count == 0) {
        fprintf(stderr, "ERROR: No values entered\n");
        return -1;
    }
    
    return 0;
}