/**
 * @file    arithmetic_divide.c
 * @brief   Implementation of the division command for the CLI calculator.
 *
 * @details
 * This module implements the "Divide" command for the CLI calculator, allowing users to divide the current ANS value by one or more input values.
 * It parses input tokens, converts them to double, performs the division, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_divide.h"

#include "cli_handler.h"
#include "misc.h"

#include "stdlib.h"

/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
cli_handler_error_e divide_values(double* result, const double value1, const double value2);

/*
 *   CONSTANTS
 */
/**
 * @brief Constant structure defining the "Divide" CLI command.
 *
 * @details
 *  - name:        Command name ("Divide")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Devide_Cte = {
        .name = "Divide",
        .description = "Divide the value to ANS - Sintax \"Divide [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Divide
};

/**
 * @brief Implements the "Divide" CLI command.
 *
 * @details
 * Parses the input stream for double values, divides the current ANS by each value,
 * and prints the result to the CLI output. Supports multiple values separated by ',' or within brackets.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Divide(cli_handler_t* cli_handler, char* saveptr){

    misc_vector_t* vector;

    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(saveptr);

    Misc_Get_Vector(vector, saveptr);
    
    if(vector != NULL){

        for (size_t i = 0; i < vector->size; i++){

            divide_values(&cli_handler->ans, cli_handler->ans, vector->data[i]); /* Divide values */
            Cli_Handler_Out_Msg(cli_handler, "ANS / %.*f = %.*f\n", cli_handler->config.decimal, vector->data[i], cli_handler->config.decimal, cli_handler->ans);
        }
    }
    else {

        Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
    }    

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Divides two double values and stores the result.
 *
 * @param result Pointer to store the result of the division.
 * @param value1 Dividend (current ANS value).
 * @param value2 Divisor (value to divide by).
 * @return       CLI_HANDLER_ERROR_OK on success.
 */
cli_handler_error_e divide_values(double* result, const double value1, const double value2) {
 
    *result = value1 / value2; /* Divide values */

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}
