/**
 * @file main.c
 * @brief Entry point for Dynamox Calculator
 * 
 * This is the main entry point as required by technical specifications.
 * Initializes the system and starts the HMI (Human Machine Interface).
 */

#include <stdio.h>
#include <stdlib.h>
#include "operation_manager.h"
#include "hmi/terminal_interface.h"
#include "logger.h"
#include "version.h"

// Forward declarations
int system_init(void);
void system_cleanup(void);

int main(void) {
    printf("===========================================\n");
    printf("   Dynamox Embedded Calculator v%s\n", VERSION_STRING);
    printf("===========================================\n\n");
    
    // Initialize system components
    if (system_init() != 0) {
        fprintf(stderr, "ERROR: System initialization failed\n");
        return EXIT_FAILURE;
    }
    
    printf("System initialized successfully\n");
    printf("Starting interactive interface...\n\n");
    
    // Run the Human Machine Interface
    hmi_run();
    
    // Cleanup before exit
    system_cleanup();
    
    printf("\nThank you for using Dynamox Calculator!\n");
    
    return EXIT_SUCCESS;
}

/**
 * @brief Initialize system components
 * @return 0 on success, 1 on failure
 */
int system_init(void) {
    // Initialize operation manager
    if (operation_manager_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize operation manager\n");
        return EXIT_FAILURE;
    }

    // Initialize logger
    if (logger_init() != 0) {
        fprintf(stderr, "WARNING: Failed to initialize logger\n");
        fprintf(stderr, "         Operations will not be logged\n");
        // Continue anyway - logging is not critical
    }
    
    return 0;
}

/**
 * @brief Cleanup system resources
 */
void system_cleanup(void) {
    logger_close();    
    // TODO: Free any allocated resources (if needed)
}
