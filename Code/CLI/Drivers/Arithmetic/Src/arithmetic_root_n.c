/**
 * @file    arithmetic_root_n.c
 * @brief   Implementation of the root (N-th root) command for the CLI calculator.
 *
 * @details
 * This module implements the "Root_N" command for the CLI calculator, allowing users to compute the N-th root of the current ANS value with one or more input values.
 * It parses input tokens, converts them to double, performs the root calculation, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_root_n.h"

#include "cli_handler.h"
#include "misc.h"

#include "math.h"
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
cli_handler_error_e root_n_values(double* result, const double value1, const double value2);

/*
 *   CONSTANTS
 */
/**
 * @brief Constant structure defining the "Root_N" CLI command.
 *
 * @details
 *  - name:        Command name ("Root_N")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Root_N_Cte = {
        .name = "Root_N",
        .description = "Root N the value to ANS - Sintax \"Root_N [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Root_N
};

/**
 * @brief Implements the "Root_N" CLI command.
 *
 * @details
 * Parses the input stream for double values, computes the N-th root of the current ANS for each value,
 * and prints the result to the CLI output. Supports multiple values separated by ',' or within brackets.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Root_N(cli_handler_t* cli_handler, char* saveptr) {
    
    misc_vector_t* vector;

    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(saveptr);

    Misc_Get_Vector(vector, saveptr);
    
    if(vector != NULL){

        for (size_t i = 0; i < vector->size; i++){

            root_n_values(&cli_handler->ans, cli_handler->ans, vector->data[i]);
            Cli_Handler_Out_Msg(cli_handler, "ANS ^ %.*f = %.*f\n", cli_handler->config.decimal, 1.0/vector->data[i], cli_handler->config.decimal, cli_handler->ans);
        }
    }
    else {

        Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
    }

    Misc_Free_Vector(vector); /* Freeing resources */

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Computes the N-th root of a value and stores the result.
 *
 * @param result Pointer to store the result of the root calculation.
 * @param value1 Value to compute the root of (current ANS value).
 * @param value2 Root degree (N).
 * @return       CLI_HANDLER_ERROR_OK on success.
 */
cli_handler_error_e root_n_values(double* result, const double value1, const double value2) {
    
    *result = pow(value1, 1.0 / value2);

    return CLI_HANDLER_ERROR_OK;
}
