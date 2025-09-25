/**
 * @file    arithmetic_power_n.h
 * @brief   Implementation of the power (exponentiation) command for the CLI calculator.
 *
 * @details
 * This module implements the "Power_N" command for the CLI calculator, allowing users to raise the current ANS value to the power of one or more input values.
 * It parses input tokens, converts them to double, performs the exponentiation, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

#ifndef CLI_DRIVERS_ROOT_N_H /* Include guard */
#define CLI_DRIVERS_ROOT_N_H

#ifdef __cplusplus  /* C++ guard */
extern "C" {
#endif

/*
 *   INCLUDES
 */
#include "cli_handler.h"

/*
 *   DEFINES
 */

/*
 *   MACROS
 */

/*
 *   ENUMS
 */

/*
 *   STRUCTS
 */

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
cli_handler_error_e Arithmetic_Root_N(cli_handler_t* cli_handler, char* saveptr);

#ifdef __cplusplus
}
#endif

#endif