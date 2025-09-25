/**
 * @file    arithmetic_divide.h
 * @brief   Implementation of the division command for the CLI calculator.
 *
 * @details
 * This module implements the "Divide" command for the CLI calculator, allowing users to divide the current ANS value by one or more input values.
 * It parses input tokens, converts them to double, performs the division, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

#ifndef CLI_DRIVERS_ARITHMETIC_DEVIDE_H /* Include guard */
#define CLI_DRIVERS_ARITHMETIC_DEVIDE_H

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
cli_handler_error_e Arithmetic_Divide(cli_handler_t* cli_handler, char* saveptr);

#ifdef __cplusplus
}
#endif

#endif