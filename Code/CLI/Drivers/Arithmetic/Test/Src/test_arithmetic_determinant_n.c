/**
 * @file    arithmetic_determinant_n.c
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
#include "arithmetic_determinant_n.h"

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
const cli_handler_commands_t Arithmetic_Determinant_N_Cte = {
        .name = "Determinant_N",
        .description = "Determinant N the value to ANS - Sintax \"Determinant [0x00000000]\"\n",
        .func_ptr = Arithmetic_Determinant_N
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e Arithmetic_Determinant_N(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_determinant_n");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}