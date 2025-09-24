/**
 * @file    arithmetic_subtract.c
 * @brief   TODO
 *
 * @details TODO
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_subtract.h"

#include "cli_handler.h"

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

/*
 *   CONSTANTS
 */
const cli_handler_commands_t Arithmetic_Subtract_Cte = {
        .name = "Subtract",
        .description = "Subtract the value to ANS - Sintax \"Subtract [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Subtract
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
cli_handler_error_e Arithmetic_Subtract(cli_handler_t* cli_handler, char* saveptr){
    
    double value;
    char* token;

    while ((token = Misc_Strtok_R(NULL, "[,]", &saveptr)) != NULL) {

        Misc_Parser_Double(&value, token);

        cli_handler->ans -= value;

        CLI_STRING_STREAM_TX(cli_handler, "ANS - %.*f = %.*f\n", cli_handler->config.decimal, value, cli_handler->config.decimal, cli_handler->ans);
    }
}