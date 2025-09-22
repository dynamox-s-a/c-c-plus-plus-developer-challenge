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

#include "misc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
 *   LOCAL FUNCTIONS PROTOTYPE
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler);
static cli_handler_error_e parser_double(double* value, char* saveptr);

static cli_handler_error_e command_clear(cli_handler_t* cli_handler, char* saveptr);
static cli_handler_error_e command_help(cli_handler_t* cli_handler, char* saveptr);
static cli_handler_error_e command_exit(cli_handler_t* cli_handler, char* saveptr);

 /*
 *   CONSTANTS
 */
const cli_handler_commands_t Commands[] = {

    /* Arithmetical commands */
    {"Add",             "Add the value to ANS - Sintax \"Add [0x00000000]\"\n", command_add},
    {"Subtract",        "Subtract the value to ANS - Sintax \"Subtract [0x00000000]\"\n", command_subtract},
    {"Divide",          "Divide the value to ANS - Sintax \"Divide [0x00000000]\"\n", command_devide},
    {"Multiply",        "Multiply the value to ANS - Sintax \"Multiply [0x00000000]\"\n", command_multiply},
    {"Power_N",         "Power N the value to ANS - Sintax \"Power_N [0x00000000]\"\n", command_power_n},
    {"Root_N",          "Root N the value to ANS - Sintax \"Root_N [0x00000000]\"\n", command_root_n},
    {"Log_N",           "Log N the value to ANS - Sintax \"Log_N [0x00000000]\"\n", command_log_n},
    {"Determinant_N",   "Determinant N the value to ANS - Sintax \"Determinant [0x00000000]\"\n", command_determinant_n},
    {"Clear",           "Clear the value to ANS - Sintax \"Clear\"\n", command_clear},

    /* Controller commands */
    {"Help",            "Return all sintax of code - Sintax \"Help\"\n", command_help},
    {"Exit",            "Exit of code - Sintax \"Exit\"\n", command_exit}
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
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
    cli_handler->ans = 0.0;

    return CLI_HANDLER_ERROR_OK;
}

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @return TODO
 */
cli_handler_error_e Cli_Handler_Check(cli_handler_t* cli_handler){

    CHECK_CLI_HANDLER_PTR(cli_handler);     /* Verify null poiter */
    CHECK_CLI_HANDLER_INIT(cli_handler);    /* Verify no intialized */

    do { /* Do this function in loop case multithread */

        CHECK_CLI_HANDLER_FUNC_RET(check_receive_cmd(cli_handler)); /* Verify if receive a command */
    }
    while (cli_handler->config.multithread == true);

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @return TODO
 */
static cli_handler_error_e check_receive_cmd(cli_handler_t* cli_handler){

    bool receive_cmd = false;
    char* saveptr;
    char* stream_token;
    
    stream_token = Misc_Strtok_R(cli_handler->config.stream_rx, " ", &saveptr); /* Get first command */

    for (size_t i = 0; i < CLI_HANDLER_CMD_MAX; i++){ /* Search the command */

        if(strcmp(stream_token, Commands[i].name) == 0){ /* Case is a correct command */

            if(Commands[i].func_ptr(cli_handler, saveptr) == CLI_HANDLER_ERROR_OK){ 

                receive_cmd = true;
            }

            break;
        }
    }

    if(receive_cmd == false){

        /* Sintax error */
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief TODO
 * 
 * @param value Pointer of value double
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e parser_double(double* value, char* token){

    double parser;

    CHECK_CLI_HANDLER_PTR(token);

    if (strncmp(token, "0x", 2) == 0 || strncmp(token, "0X", 2) == 0) {
        
        *value = (double)strtol(token, NULL, 16);
    }
    else {
        *value = atof(token);
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e command_clear(cli_handler_t* cli_handler, char* saveptr){
    
    cli_handler->ans = 0.0;

    printf("command_clear");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e command_help(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_help");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
static cli_handler_error_e command_exit(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_exit");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}
