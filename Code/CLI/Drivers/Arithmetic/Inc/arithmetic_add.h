/**
 * @file    arithmetic_add.c
 * @brief   Implementation of the addition command for the CLI calculator.
 *
 * @details
 * This module implements the "Add" command for the CLI calculator, allowing users to add one or more values to the current ANS value.
 * It parses input tokens, converts them to double, performs the addition, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

#ifndef CLI_DRIVERS_ARITHMETIC_ADD_H /* Include guard */
#define CLI_DRIVERS_ARITHMETIC_ADD_H

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

cli_handler_error_e Arithmetic_Add(cli_handler_t* cli_handler, char* saveptr);

#ifdef __cplusplus
}
#endif

#endif