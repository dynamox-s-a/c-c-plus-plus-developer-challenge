/**
 * @file terminal_interface.c
 * @brief Implementation of terminal-based HMI
 */

#include "hmi/terminal_interface.h"
#include "hmi/input_validator.h"
#include "operation_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

#define MAX_VALUES 100

/**
 * @brief Display main menu
 */
static void display_menu(void) {
    printf("\n");
    printf("========================================\n");
    printf("           MAIN MENU\n");
    printf("========================================\n");
    operation_manager_list();
    printf("0. Exit\n");
    printf("========================================\n");
}

/**
 * @brief Get input values from user
 */
static int get_input_values(const Operation* op, double* values, int* count) {
    printf("\n--- Input for operation: %s ---\n", op->name);
    
    if (op->supports_array) {
        // Ask if single value or array
        printf("Enter values (space or comma-separated):\n");
        printf("Examples: 1.0 2.0 3.0  or  1,2,3\n");
        
        if (input_read_array("> ", values, MAX_VALUES, count) != 0) {
            return -1;
        }
        
        printf("Read %d value(s)\n", *count);
    }
    else {
        // Single value only
        printf("Enter single value:\n");
        
        if (input_read_double("> ", &values[0]) != 0) {
            return -1;
        }
        
        *count = 1;
    }
    
    return 0;
}

/**
 * @brief Display result
 */
static void display_result(const Operation* op, const double* values, int count, double result) {
    printf("\n");
    printf("========================================\n");
    printf("           RESULT\n");
    printf("========================================\n");
    printf("Operation: %s\n", op->name);
    printf("Input: ");
    
    if (count == 1) {
        printf("%.6g\n", values[0]);
    }
    else {
        printf("[");
        for (int i = 0; i < count; i++) {
            printf("%.6g", values[i]);
            if (i < count - 1) printf(", ");
        }
        printf("]\n");
    }
    
    printf("Result: %.6g\n", result);
    printf("========================================\n");
}

/**
 * @brief Main HMI loop
 */
void hmi_run(void) {
    double values[MAX_VALUES];
    int count;
    double result;
    int choice;
    int running = 1;
    
    printf("Welcome to Dynamox Calculator!\n");
    printf("Type values carefully. Use Ctrl+C to force exit.\n");
    
    while (running) {
        // Display menu
        display_menu();
        
        // Get user choice
        int num_ops = operation_manager_get_count();
        if (input_read_int("Select operation (0 to exit): ", 0, num_ops, &choice) != 0) {
            printf("Invalid input. Please try again.\n");
            continue;
        }
        
        // Check for exit
        if (choice == 0) {
            printf("\nExiting...\n");
            running = 0;
            continue;
        }
        
        // Get operation (adjust index: menu is 1-based, array is 0-based)
        const Operation* op = operation_manager_get(choice - 1);
        if (op == NULL) {
            fprintf(stderr, "ERROR: Invalid operation\n");
            continue;
        }
        
        // Get input values
        if (get_input_values(op, values, &count) != 0) {
            printf("Invalid input. Please try again.\n");
            logger_log_error(op->name, "Invalid input");
            continue;
        }
        
        // Execute operation
        printf("\nExecuting...\n");
        if (operation_manager_execute(choice - 1, values, count, &result) != 0) {
            fprintf(stderr, "Operation failed. See error above.\n");
            logger_log_error(op->name, "Execution failed");
            continue;
        }

        // Log success
        logger_log_success(op->name, values, count, result);
        
        // Display result
        display_result(op, values, count, result);
        
        // Pause before showing menu again
        printf("\nPress Enter to continue...");
        input_clear_buffer();
        getchar();
    }
}