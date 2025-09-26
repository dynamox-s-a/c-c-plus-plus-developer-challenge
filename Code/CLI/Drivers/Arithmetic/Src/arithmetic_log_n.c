/**
 * @file    arithmetic_log_n.c
 * @brief   Implementation of the logarithm base N command for the CLI calculator.
 *
 * @details
 * This module implements the "Log_N" command for the CLI calculator, allowing users to compute the logarithm of the current ANS value with a specified base.
 * It parses input tokens, converts them to double, performs the logarithm calculation, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_log_n.h"

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
cli_handler_error_e log_n_values(double* result, const double value1, const double value2);

/*
 *   CONSTANTS
 */

/**
 * @brief Constant structure defining the "Log_N" CLI command.
 *
 * @details
 *  - name:        Command name ("Log_N")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Log_N_Cte = {
        .name = "Log_N",
        .description = "Log N the value to ANS - Sintax \"Log_N [0x00000000]\"\n",
        .func_ptr = Arithmetic_Log_N
};

/**
 * @brief Implements the "Log_N" CLI command.
 *
 * @details
 * Parses the input stream for double values, computes the logarithm of the current ANS with the given base,
 * and prints the result to the CLI output. Supports multiple values separated by ',' or within brackets.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Log_N(cli_handler_t* cli_handler, char* saveptr){
    
    misc_vector_t* vector;

    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(saveptr);

    Misc_Get_Vector(vector, saveptr);
    
    if(vector != NULL){

        for (size_t i = 0; i < vector->size; i++){

            log_n_values(&cli_handler->ans, cli_handler->ans, vector->data[i]); /* Divide values */
            Cli_Handler_Out_Msg(cli_handler, "Log(ANS) Base %.*f = %.*f\n", cli_handler->config.decimal, vector->data[i], cli_handler->config.decimal, cli_handler->ans);
        }
    }
    else {

        Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
    }    

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Calculates the logarithm of value1 with base value2 and stores the result.
 *
 * @param result Pointer to store the result of the logarithm calculation.
 * @param value1 Value to compute the logarithm of (argument).
 * @param value2 Base of the logarithm.
 * @return       CLI_HANDLER_ERROR_OK on success.
 */
cli_handler_error_e log_n_values(double* result, const double value1, const double value2) {

    *result = log(value1) / log(value2); /* Logarithm base N values */

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}