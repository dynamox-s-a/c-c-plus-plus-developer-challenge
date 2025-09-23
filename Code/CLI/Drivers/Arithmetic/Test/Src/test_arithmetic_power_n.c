/**
 * @file    arithmetic_power_n.c
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
#include "arithmetic_power_n.h"

#include "cli_handler.h"

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
const cli_handler_commands_t Arithmetic_Power_N_Cte = {
        .name = "Power_N",
        .description = "Power N the value to ANS - Sintax \"Power_N [int, int, ...]\"\n",
        .func_ptr = Arithmetic_Power_N
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e Arithmetic_Power_N(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_power_n");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}