/**
 * @file    arithmetic_multiply.c
 * @brief   Implementation of the multiplication command for the CLI calculator.
 *
 * @details
 * This module implements the "Multiply" command for the CLI calculator, allowing users to multiply the current ANS value by one or more input values.
 * It parses input tokens, converts them to double, performs the multiplication, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_multiply.h"

#include "cli_handler.h"
#include "misc.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
cli_handler_error_e multiply_values(double* result, const double value1, const double value2);

/*
 *   CONSTANTS
 */
/**
 * @brief Constant structure defining the "Multiply" CLI command.
 *
 * @details
 *  - name:        Command name ("Multiply")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Multiply_Cte = {
        .name = "Multiply",
        .description = "Multiply the value to ANS - Sintax \"Multiply [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Multiply
};

/**
 * @brief Implements the "Multiply" CLI command.
 *
 * @details
 * Parses the input stream for double values, multiplies the current ANS by each value,
 * and prints the result to the CLI output. Supports multiple values separated by ',' or within brackets.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Multiply(cli_handler_t* cli_handler, char* saveptr){
    
    double value;
    char* token;

    while ((token = Misc_Strtok_R(NULL, "[,]", &saveptr)) != NULL) {

        Misc_Parser_Double(&value, token);

        multiply_values(&cli_handler->ans, cli_handler->ans, value);

        Cli_Handler_Out_Msg(cli_handler, "ANS * %.*f = %.*f\n", cli_handler->config.decimal, value, cli_handler->config.decimal, cli_handler->ans);
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Multiplies two double values and stores the result.
 *
 * @param result Pointer to store the result of the multiplication.
 * @param value1 First operand (current ANS value).
 * @param value2 Second operand (value to multiply).
 * @return       CLI_HANDLER_ERROR_OK on success.
 */
cli_handler_error_e multiply_values(double* result, const double value1, const double value2) {

    *result = value1 * value2;

    return CLI_HANDLER_ERROR_OK;
}
