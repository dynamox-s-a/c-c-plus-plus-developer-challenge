/**
 * @file    arithmetic_power_n.c
 * @brief   Implementation of the power (exponentiation) command for the CLI calculator.
 *
 * @details
 * This module implements the "Power_N" command for the CLI calculator, allowing users to raise the current ANS value to the power of one or more input values.
 * It parses input tokens, converts them to double, performs the exponentiation, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_power_n.h"

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
cli_handler_error_e power_n_values(double* result, const double value1, const double value2);

/*
 *   CONSTANTS
 */
/**
 * @brief Constant structure defining the "Power_N" CLI command.
 *
 * @details
 *  - name:        Command name ("Power_N")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Power_N_Cte = {
        .name = "Power_N",
        .description = "Power N the value to ANS - Sintax \"Power_N [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Power_N
};

/**
 * @brief Implements the "Power_N" CLI command.
 *
 * @details
 * Parses the input stream for double values, raises the current ANS to the power of each value,
 * and prints the result to the CLI output. Supports multiple values separated by ',' or within brackets.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Power_N(cli_handler_t* cli_handler, char* saveptr){
    
    misc_vector_t* vector;

    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(saveptr);

    Misc_Get_Vector(vector, saveptr);
    
    if(vector != NULL){

        for (size_t i = 0; i < vector->size; i++){

            power_n_values(&cli_handler->ans, cli_handler->ans, vector->data[i]);
            Cli_Handler_Out_Msg(cli_handler, "ANS ^ %.*f = %.*f\n", cli_handler->config.decimal, vector->data[i], cli_handler->config.decimal, cli_handler->ans);
        }
    }
    else {

        Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
    }

    Misc_Free_Vector(vector); /* Freeing resources */

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Raises one double value to the power of another and stores the result.
 *
 * @param result Pointer to store the result of the exponentiation.
 * @param value1 Base value (current ANS value).
 * @param value2 Exponent value.
 * @return       CLI_HANDLER_ERROR_OK on success.
 */
cli_handler_error_e power_n_values(double* result, const double value1, const double value2) {

    *result = pow(value1, value2);

    return CLI_HANDLER_ERROR_OK;
}
