/**
 * @file    controller.h
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

#ifndef CLI_DRIVERS_MISC_H /* Include guard */
#define CLI_DRIVERS_MISC_H

#ifdef __cplusplus  /* C++ guard */
extern "C" {
#endif

/*
 *   INCLUDES
 */
#include "cli_handler.h"
#include "logs_file.h"
#include "misc.h"
#include "stream.h"

#include "stdbool.h"
#include "stdint.h"

/*
 *   DEFINES
 */
#define CONTROLLER_STREAM_SIZE        (uint32_t)4096  /* Size of the controller stream */

/*
 *   MACROS
 */

/*
 *   ENUMS
 */
typedef enum {

    CONTROLLER_ERROR_OK = 0,       /**< Return success */
    CONTROLLER_ERROR_INIT,         /**< No initializate correctly */
    CONTROLLER_ERROR_PARM,         /**< Incorrect param in function */

    /* Maximal value in this enumerate */
    CONTROLLER_ERROR_MAX           /**< Guard for iterations */

} controller_error_e;

/*
 *   STRUCTS
 */

/* Stream structure */
typedef struct {
    
    char buffer_in[CONTROLLER_STREAM_SIZE];   /* Stream where send commands to CLI */
    char buffer_out[CONTROLLER_STREAM_SIZE];  /* Stream where receive answere from CLI */

    logs_file_handler_t logs_file_handler;  /* Log file handler */
    cli_handler_t cli_handler;              /* CLI handler */
    
    stream_handler_t stream_in;   /* Stream rx CLI */
    stream_handler_t stream_out;  /* Stream tx CLI */

} controller_handler_t;
/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */

controller_error_e Controller_Init(controller_handler_t* controller);
controller_error_e Controller_Check(controller_handler_t* controller);

/*
 *   UNIT TEST
 */

#ifdef __cplusplus
}
#endif

#endif