/**
 * @file    arithmetic_determinant_n.c
 * @brief   Implementation of the determinant calculation command for the CLI calculator.
 *
 * @details
 * This module implements the "Determinant_N" command for the CLI calculator, allowing users to compute
 * the determinant of a square matrix provided as a flat vector. It parses input tokens, checks matrix validity,
 * performs the determinant calculation recursively, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

#ifndef CLI_DRIVERS_ARITHMETIC_DETERMINANT_N_H /* Include guard */
#define CLI_DRIVERS_ARITHMETIC_DETERMINANT_N_H

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
cli_handler_error_e Arithmetic_Determinant_N(cli_handler_t* cli_handler, char* saveptr);

#ifdef __cplusplus
}
#endif

#endif