/**
 * @file    cli_handler.c
 * @brief   CLI Handler - Receive commands and process.
 *
 * @details
 * This module implements the CLI handler for the application. It receives commands from the RX stream,
 * parses and processes them, and returns responses in the TX stream. It supports arithmetic and controller
 * commands, manages command lookup, and provides thread-safe access to streams.
 *
 * Main features:
 *   - Command registration and lookup
 *   - Command parsing and dispatch
 *   - Output formatting and message sending
 *   - Thread-safe stream access using mutexes
 *   - Built-in commands: Decimal, Clear, Help, Exit
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

/**
 * @brief Table of all available commands (arithmetic and controller).
 */
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
 * @brief Initializes and configures the CLI handler.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param config      Pointer to the CLI handler configuration structure.
 * @param param       Optional user context pointer.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
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
 * @brief Main CLI handler loop. Processes received commands.
 *
 * @details
 * If multithreading is enabled, this function stays in a loop. It locks the RX stream,
 * checks for received commands, and dispatches them for processing.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
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

    Mutex_Unlock(&cli_handler->config.stream_rx->mutex);

    return CLI_HANDLER_ERROR_OK;
}

/**
 * @brief Sends a formatted message to the CLI TX stream (thread-safe).
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param format      Format string (printf-style).
 * @param ...         Arguments for the format string.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
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
 * @brief Checks and processes a received command from the RX stream.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler) {
    
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

    if (!receive_cmd) {
        Cli_Handler_Out_Msg(cli_handler, "Verify sintax command, send \"Help\" for more information.\n");
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Command: Change the decimal precision of ANS output.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
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
 * @brief Command: Clear the ANS variable.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
static cli_handler_error_e command_clear(cli_handler_t* cli_handler, char* saveptr){
    
    cli_handler->ans = 0.0f; /* Reset ANS to value 0.0 */

    Cli_Handler_Out_Msg(cli_handler, "ANS = %.*f\n", cli_handler->config.decimal, cli_handler->ans);

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Command: Show help for all available commands.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
static cli_handler_error_e command_help(cli_handler_t* cli_handler, char* saveptr){

    for(size_t i = 0; i < sizeof(Commands)/sizeof(cli_handler_commands_t*); i++){ /* List all desciption commnads */

        Cli_Handler_Out_Msg(cli_handler, "%-20s - %s", Commands[i]->name, Commands[i]->description);
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Command: Deinitialize the CLI handler and exit the application.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
static cli_handler_error_e command_exit(cli_handler_t* cli_handler, char* saveptr){
    
    cli_handler->init = false;      /* Deinitialize the handler */

    Cli_Handler_Out_Msg(cli_handler, "Finish application");

    return CLI_HANDLER_ERROR_OK;    /* Return OK */
}
