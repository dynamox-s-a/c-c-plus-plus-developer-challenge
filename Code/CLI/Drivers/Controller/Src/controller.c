/**
 * @file    controller.c
 * @brief   Utility functions for string parsing and conversion.
 *
 * @details
 * This module provides miscellaneous helper functions, including:
 *   - A reentrant string tokenizer (strtok_r)
 *   - String to double and unsigned integer parsers with hexadecimal support
 *   - Getter functions to retrieve function pointers for unit testing
 * 
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "controller.h"

#include "cli_handler.h"
#include "logs_file.h"
#include "mutex.h"
#include "stream.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 *   DEFINES
 */

/*
 *   MACROS
 */
/* Verify NULL parameter */
#define CHECK_CONTROLLER_PTR(controller_ptr)        do { if ((controller_ptr) == NULL) return CONTROLLER_ERROR_PARM; } while(0)

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
static controller_error_e receive_terminal_command(stream_handler_t* stream_handler);

/*
 *   CONSTANTS
 */

/*
 *   UNIT TEST
 */


/*
 *   IMPLEMENTATION
 */

/**
 * @brief Reentrant string tokenizer (public domain by Charlie Gordon).
 *
 * @param str    The string to tokenize, or NULL to continue tokenizing the previous string.
 * @param delim  The delimiter characters.
 * @param nextp  Pointer to a char* variable that stores the next position.
 * @return       Pointer to the next token, or NULL if no more tokens are found.
 */
controller_error_e Controller_Init(controller_handler_t* controller) {
    
    CHECK_CONTROLLER_PTR(controller); /* Check controller pointer */

    cli_handler_config_t config_cli = {         /* Configuration of CLI */
        .multithread = false,                   /* Flag to use multithread */
        .decimal = 5,                           /* Decimal of ANS when answer a command */
        .stream_rx = &controller->stream_in,    /* Set stream where send commands to CLI */
        .stream_tx = &controller->stream_out    /* Set stream where receive answer from CLI */
    };

    logs_file_handler_config_t config_logs = {

        .file_name = "log.txt",             /* File name to save logs */
        .stream = &controller->stream_out,  /* Stream where receive logs */
        .terminal = false                   /* Flag to print log in terminal */
    };

    Stream_Init(&controller->stream_in, controller->buffer_in, CONTROLLER_STREAM_SIZE);   /* Initialize stream rx CLI */
    Stream_Init(&controller->stream_out, controller->buffer_out, CONTROLLER_STREAM_SIZE);  /* Initialize stream tx CLI */

    Cli_Handler_Init(&controller->cli_handler, &config_cli, NULL);        /* Initialize handler CLI */
    Logs_File_Handler_Init(&controller->logs_file_handler, &config_logs); /* Initialize handler log file */

    return CONTROLLER_ERROR_OK;
}

/**
 * @brief Parses a string token into a double value.
 *
 *        Supports decimal and hexadecimal (prefix "0x" or "0X") formats.
 *
 * @param value Pointer to the double to store the result.
 * @param token String token to parse.
 * @return      MISC_ERROR_OK on success, MISC_ERROR_PARM if a parameter is NULL.
 */
controller_error_e Controller_Check(controller_handler_t* controller) {

    CHECK_CONTROLLER_PTR(controller);

    receive_terminal_command(&controller->stream_in); /* Receive command from terminal */

    Cli_Handler_Check(&controller->cli_handler);             /* Verify handler CLI */
    Logs_File_Handler_Check(&controller->logs_file_handler); /* Check if have a new log to save */

    return CONTROLLER_ERROR_OK; /* Return OK */
}

/**
 * @brief This function will check if receive a correct command and process
 * 
 * @param cli_handler Pointer of handler CLI
 * @param stream_in Stream rx data
 * @param size_stream Sizeof stream
 */
static controller_error_e receive_terminal_command(stream_handler_t* stream_handler){

    Mutex_Lock(&stream_handler->mutex); /* Lock stream */

    if (fgets(stream_handler->buffer, stream_handler->size, stdin) != NULL) { /* Get terminal commands */

        stream_handler->buffer[strcspn(stream_handler->buffer, "\n")] = '\0';        /* Remove \n of stream and set as 0 */
    }

    Mutex_Unlock(&stream_handler->mutex);  /* Unlock stream */

    return CONTROLLER_ERROR_OK; /* Return OK */
}
