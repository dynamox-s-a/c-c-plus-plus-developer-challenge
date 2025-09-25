/**
 * @file    arithmetic_log_n.h
 * @brief   Implementation of the logarithm base N command for the CLI calculator.
 *
 * @details
 * This module implements the "Log_N" command for the CLI calculator, allowing users to compute the logarithm of the current ANS value with a specified base.
 * It parses input tokens, converts them to double, performs the logarithm calculation, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

#ifndef CLI_DRIVERS_ARITHMETIC_LOG_N_H /* Include guard */
#define CLI_DRIVERS_ARITHMETIC_LOG_N_H

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
cli_handler_error_e Arithmetic_Log_N(cli_handler_t* cli_handler, char* saveptr);

#ifdef __cplusplus
}
#endif

#endif