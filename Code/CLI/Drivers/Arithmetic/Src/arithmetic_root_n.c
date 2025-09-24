/**
 * @file    arithmetic_add.c
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
#include "arithmetic_root_n.h"

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
const cli_handler_commands_t Arithmetic_Root_N_Cte = {
        .name = "Root_N",
        .description = "Root N the value to ANS - Sintax \"Root_N [0x00000000]\"\n",
        .func_ptr = Arithmetic_Root_N
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e Arithmetic_Root_N(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_root_n");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}