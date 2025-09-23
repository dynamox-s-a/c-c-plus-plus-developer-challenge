/**
 * @file    arithmetic_devide.c
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
#include "arithmetic_divide.h"

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
const cli_handler_commands_t Arithmetic_Devide_Cte = {
        .name = "Divide",
        .description = "Divide the value to ANS - Sintax \"Divide [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Devide
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e Arithmetic_Devide(cli_handler_t* cli_handler, char* saveptr){
    
    double value;
    char* token;

    while ((token = Misc_Strtok_R(NULL, "[,]", &saveptr)) != NULL) {

        Misc_Parser_Double(&value, token);

        cli_handler->ans /= value;

        printf("ANS / %.*f = %.*f\n", cli_handler->config.decimal, value, cli_handler->config.decimal, cli_handler->ans);
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}