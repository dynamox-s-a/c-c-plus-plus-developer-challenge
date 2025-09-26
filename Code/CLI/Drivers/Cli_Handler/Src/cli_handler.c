/**
 * @file    cli_handler.c
 * @brief   CLI Handler - Receive commands and process
 *
 * @details Receive a command from stream rx and return answare in stream tx
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "cli_handler.h"

#include "arithmetic_add.h"
#include "arithmetic_determinant_n.h"
#include "arithmetic_divide.h"
#include "arithmetic_log_n.h"
#include "arithmetic_multiply.h"
#include "arithmetic_power_n.h"
#include "arithmetic_root_n.h"
#include "arithmetic_subtract.h"

#include "misc.h"

#include "stdarg.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
 */
extern cli_handler_commands_t Arithmetic_Add_Cte;
extern cli_handler_commands_t Arithmetic_Determinant_N_Cte;
extern cli_handler_commands_t Arithmetic_Devide_Cte;
extern cli_handler_commands_t Arithmetic_Log_N_Cte;
extern cli_handler_commands_t Arithmetic_Multiply_Cte;
extern cli_handler_commands_t Arithmetic_Power_N_Cte;
extern cli_handler_commands_t Arithmetic_Root_N_Cte;
extern cli_handler_commands_t Arithmetic_Subtract_Cte;

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler);

static cli_handler_error_e command_decimal(cli_handler_t* cli_handler, char* saveptr);
static cli_handler_error_e command_clear(cli_handler_t* cli_handler, char* saveptr);
static cli_handler_error_e command_help(cli_handler_t* cli_handler, char* saveptr);
static cli_handler_error_e command_exit(cli_handler_t* cli_handler, char* saveptr);

 /*
 *   CONSTANTS
 */

const cli_handler_commands_t Cli_Handler_Decimal = {
        .name = "Decimal",
        .description = "Change decimal configuration - Sintax \"Decimal [uint]\"\n",
        .func_ptr = command_decimal
};

const cli_handler_commands_t Cli_Handler_Clear = {
        .name = "Clear",
        .description = "Clear the value to ANS - Sintax \"Clear\"\n",
        .func_ptr = command_clear
};

const cli_handler_commands_t Cli_Handler_Help = {
        .name = "Help",
        .description = "Return all sintax of code - Sintax \"Help\"\n",
        .func_ptr = command_help
};

const cli_handler_commands_t Cli_Handler_Exit = {
        .name = "Exit",
        .description = "Exit of code - Sintax \"Exit\"\n",
        .func_ptr = command_exit
};

const cli_handler_commands_t* Commands[] = {

    /* Arithmetical commands */
    &Arithmetic_Add_Cte,
    &Arithmetic_Determinant_N_Cte,
    &Arithmetic_Devide_Cte,
    &Arithmetic_Log_N_Cte,
    &Arithmetic_Multiply_Cte,
    &Arithmetic_Power_N_Cte,
    &Arithmetic_Root_N_Cte,
    &Arithmetic_Subtract_Cte,

    /* Controller commands */
    &Cli_Handler_Decimal,
    &Cli_Handler_Clear,
    &Cli_Handler_Help,
    &Cli_Handler_Exit
};

/**
 * @brief This function initialize and configure handler CLI
 * 
 * @param cli_handler Pointer of handler CLI
 * @return cli_handler_error_e
 */
cli_handler_error_e Cli_Handler_Init(   cli_handler_t* cli_handler,
                                        cli_handler_config_t* config,
                                        void* param){

    /* Verify all input parammters */
    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(config);

    cli_handler->param = param;         /* Copy context for future use */
    cli_handler->config = *config;      /* Copy configuration */

    cli_handler->init = true;           /* Indicate success initializate */
    cli_handler->ans = 0.0f;            /* Initialize ANS with value 0.0*/

    Cli_Handler_Out_Msg(cli_handler, "Dynamox C/C++ Developer Challenge\n");

    return CLI_HANDLER_ERROR_OK;
}

/**
 * @brief Is a CLI handler, case multithread will stay in loop
 * 
 * @param cli_handler Pointer of handler CLI
 * @return cli_handler_error_e
 */
cli_handler_error_e Cli_Handler_Check(cli_handler_t* cli_handler){

    CHECK_CLI_HANDLER_PTR(cli_handler);     /* Verify null poiter */
    CHECK_CLI_HANDLER_INIT(cli_handler);    /* Verify no intialized */

    Mutex_Lock(&cli_handler->config.stream_rx->mutex); /* Lock stream */

    do { /* Do this function in loop case multithread */

        if(strlen(cli_handler->config.stream_rx->buffer) > 0){

            CHECK_CLI_HANDLER_FUNC_RET(check_receive_cmd(cli_handler)); /* Verify if receive a command */
        }
    }
    while (cli_handler->config.multithread == true);

    Mutex_Unlock(&cli_handler->config.stream_rx->mutex); /* Unlock stream */

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

cli_handler_error_e Cli_Handler_Out_Msg(cli_handler_t* cli_handler, const char* format, ...) {

    char _buffer[512];

    Mutex_Lock(&cli_handler->config.stream_tx->mutex);

    va_list args;
    va_start(args, format);
    vsnprintf(_buffer, sizeof(_buffer), format, args);
    va_end(args);

    strcat(cli_handler->config.stream_tx->buffer, _buffer);

    Mutex_Unlock(&cli_handler->config.stream_tx->mutex);

    return CLI_HANDLER_ERROR_OK;
}

/**
 * @brief This function will check if receive a correct command and process
 * 
 * @param cli_handler Pointer of handler CLI
 * @return cli_handler_error_e
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler){

    bool receive_cmd = false;
    char* saveptr;
    char* stream_token;

    stream_token = Misc_Strtok_R(cli_handler->config.stream_rx->buffer, " ", &saveptr); /* Get first command */

    for (size_t i = 0; i < sizeof(Commands)/sizeof(cli_handler_commands_t*); i++){  /* Search the command */

        if(strcmp(stream_token, Commands[i]->name) == 0){   /* Case is a correct command */

            Commands[i]->func_ptr(cli_handler, saveptr);    /* Process command recived*/
            receive_cmd = true;                             /* Set flag receive to indicate cmd OK*/

            break;
        }
    }

    if(receive_cmd == false){   /* Case this command dont have in list */

        Cli_Handler_Out_Msg(cli_handler, "Verify sintax command, send \"Help\" for more information.\n");
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief This command change the precision output stream of ANS
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return cli_handler_error_e
 */
static cli_handler_error_e command_decimal(cli_handler_t* cli_handler, char* saveptr){
    
    uint32_t decimal;
    char* token;

    token = Misc_Strtok_R(NULL, "[]", &saveptr);    /* Remove [] after command */

    CHECK_CLI_HANDLER_PTR(token);                   /* Verify if have data to parse */

    Misc_Parser_Uint(&decimal, token);              /* Convert value to unsigned int */

    cli_handler->config.decimal = decimal;          /* Set decimal configuration */

    Cli_Handler_Out_Msg(cli_handler, "New decimal resolution is %d\n", decimal);

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief This command clear ANS variable
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return cli_handler_error_e
 */
static cli_handler_error_e command_clear(cli_handler_t* cli_handler, char* saveptr){
    
    cli_handler->ans = 0.0f; /* Reset ANS to value 0.0 */

    Cli_Handler_Out_Msg(cli_handler, "ANS = %.*f\n", cli_handler->config.decimal, cli_handler->ans);

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief This command return all information about others commands
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return cli_handler_error_e
 */
static cli_handler_error_e command_help(cli_handler_t* cli_handler, char* saveptr){

    for(size_t i = 0; i < sizeof(Commands)/sizeof(cli_handler_commands_t*); i++){ /* List all desciption commnads */

        Cli_Handler_Out_Msg(cli_handler, "%-20s - %s", Commands[i]->name, Commands[i]->description);
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief This command deinitilize handler
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return cli_handler_error_e
 */
static cli_handler_error_e command_exit(cli_handler_t* cli_handler, char* saveptr){
    
    cli_handler->init = false;      /* Deinitialize the handler */

    Cli_Handler_Out_Msg(cli_handler, "Finish application");

    return CLI_HANDLER_ERROR_OK;    /* Return OK */
}
