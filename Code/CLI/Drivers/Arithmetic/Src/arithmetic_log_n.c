/**
 * @file    arithmetic_log_n.c
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
#include "arithmetic_log_n.h"

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
const cli_handler_commands_t Arithmetic_Log_N_Cte = {
        .name = "Log_N",
        .description = "Log N the value to ANS - Sintax \"Log_N [0x00000000]\"\n",
        .func_ptr = Arithmetic_Log_N
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
cli_handler_error_e Arithmetic_Log_N(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_log_n");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}