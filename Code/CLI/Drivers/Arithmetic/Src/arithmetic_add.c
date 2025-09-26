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

/*
 *   INCLUDES
 */
#include "arithmetic_add.h"

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
static cli_handler_error_e add_values(double* result, const double value1, const double value2);

/*
 *   CONSTANTS
 */

/**
 * @brief Constant structure defining the "Add" CLI command.
 *
 * @details
 *  - name:        Command name ("Add")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Add_Cte = {
        .name = "Add",
        .description = "Add the value to ANS - Sintax \"Add [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Add
};

/**
 * @brief Implements the "Add" CLI command.
 *
 * @details
 * Parses the input stream for double values, adds each value to the current ANS,
 * and prints the result to the CLI output. Supports multiple values separated by ',' or within brackets.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Add(cli_handler_t* cli_handler, char* saveptr){

    misc_vector_t* vector;

    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(saveptr);

    Misc_Get_Vector(vector, saveptr);

    if(vector != NULL){

        for (size_t i = 0; i < vector->size; i++){

            add_values(&cli_handler->ans, cli_handler->ans, vector->data[i]);
            Cli_Handler_Out_Msg(cli_handler, "ANS + %.*f = %.*f\n", cli_handler->config.decimal, vector->data[i], cli_handler->config.decimal, cli_handler->ans); /* Print result */
        }
    }
    else {

        Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
    }

    Misc_Free_Vector(vector); /* Freeing resources */

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Adds two double values and stores the result.
 *
 * @param result Pointer to store the result of the addition.
 * @param value1 First operand (current ANS value).
 * @param value2 Second operand (value to add).
 * @return       CLI_HANDLER_ERROR_OK on success.
 */
static cli_handler_error_e add_values(double* result, const double value1, const double value2) {

    *result = value1 + value2; /* Add values */

    return CLI_HANDLER_ERROR_OK;
}