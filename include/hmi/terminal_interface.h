/**
 * @file terminal_interface.h
 * @brief Terminal-based Human Machine Interface
 */

#ifndef TERMINAL_INTERFACE_H
#define TERMINAL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run the interactive terminal interface
 * 
 * This function runs the main loop:
 * 1. Show menu
 * 2. Get user selection
 * 3. Get input values
 * 4. Execute operation
 * 5. Display result
 * 6. Repeat or exit
 */
void hmi_run(void);

#ifdef __cplusplus
}
#endif

#endif // TERMINAL_INTERFACE_H