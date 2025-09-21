/**
 * @file    cli_handler.c
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
#include "cli_handler.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/*
 *   MACROS
 */

/* Verify NULL parameter */
#define CHECK_CLI_HANDLER_PTR(cli_handler)      do { if ((cli_handler) == NULL) return CLI_HANDLER_ERROR_PARM; } while(0)

/* Verify if initializate correctly */
#define CHECK_CLI_HANDLER_INIT(cli_handler)     do { if ((cli_handler)->init == false) return CLI_HANDLER_ERROR_INIT; } while(0)

#define CHECK_CLI_HANDLER_FUNC_RET(func)        do { int err = (func); if (err != (CLI_HANDLER_ERROR_OK)) return err; } while (0)

/*
 *   EXTERNAL VARIABLES
 */

 /*
 *   CONSTANTS
 */
const cli_handler_commands_t Commands[] = {

    /* Arithmetical commands */
    {"Add",             "Add the value to ANS - Sintax \"Add [0x00000000]\""},
    {"Subtract",        "Subtract the value to ANS - Sintax \"Subtract [0x00000000]\""},
    {"Divide",          "Divide the value to ANS - Sintax \"Divide [0x00000000]\""},
    {"Multiply",        "Multiply the value to ANS - Sintax \"Multiply [0x00000000]\""},
    {"Power_N",         "Power N the value to ANS - Sintax \"Power_N [0x00000000]\""},
    {"Root_N",          "Root N the value to ANS - Sintax \"Root_N [0x00000000]\""},
    {"Log_N",           "Log N the value to ANS - Sintax \"Log_N [0x00000000]\""},
    {"Determinant_N",   "Determinant N the value to ANS - Sintax \"Determinant [0x00000000]\""},
    {"Clear",           "Clear the value to ANS - Sintax \"Clear\""},

    /* Controller commands */
    {"Help",            "Return all sintax of code - Sintax \"Help\""},
    {"Exit",            "Exit of code - Sintax \"Exit\""}
};

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler);

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
cli_handler_error_e Cli_Handler_Init(   cli_handler_t* cli_handler,
                                        cli_handler_callback_t* callback, 
                                        cli_handler_config_t* config,
                                        void* param){

    /* Verify all input parammters */
    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(callback);
    CHECK_CLI_HANDLER_PTR(config);

    cli_handler->param = param;
    cli_handler->callback = *callback;
    cli_handler->config = *config;

    cli_handler->init = true;

    printf("Iniciou o CLI");

    return CLI_HANDLER_ERROR_OK;
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
cli_handler_error_e Cli_Handler_Check(cli_handler_t* cli_handler){

    CHECK_CLI_HANDLER_PTR(cli_handler);     /* Verify null poiter */
    CHECK_CLI_HANDLER_INIT(cli_handler);    /* Verify no intialized */

    do { /* Do this function in loop case multithread */

        CHECK_CLI_HANDLER_FUNC_RET(check_receive_cmd(cli_handler)); /* Verify if receive a command */
    } 
    while (cli_handler->config.multithread == true);

    return CLI_HANDLER_ERROR_OK;
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler){

    char* stream_token = strtok(cli_handler->config.stream, " "); /* Get first command */

    for (size_t i = 0; i < CLI_HANDLER_CMD_MAX; i++){

        if(strcmp(stream_token, Commands[i].name) == 0){

            printf("Received command: %s\n", cli_handler->config.stream);
        }
    }

    return CLI_HANDLER_ERROR_OK;
}