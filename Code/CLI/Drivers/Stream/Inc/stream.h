/**
 * @file    stream.c
 * @brief   Stream buffer handler implementation for CLI application.
 *
 * @details
 * This module provides functions to initialize and manage a stream buffer handler,
 * including mutex protection and buffer clearing. It is designed for use in CLI or
 * embedded applications where safe and efficient stream management is required.
 *
 * @author  Emerson Isaias da Silva
 * @date    25-09-2025
 */

#ifndef CLI_DRIVERS_STREAM_H /* Include guard */
#define CLI_DRIVERS_STREAM_H

#ifdef __cplusplus  /* C++ guard */
extern "C" {
#endif

/*
 *   INCLUDES
 */
#include "mutex.h"

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

/*
 *   DEFINES
 */

/*
 *   MACROS
 */

/*
 *   ENUMS
 */
typedef enum {

    STREAM_ERROR_OK = 0,       /**< Return success */
    STREAM_ERROR_INIT,         /**< No initializate correctly */
    STREAM_ERROR_PARM,         /**< Incorrect param in function */

    /* Maximal value in this enumerate */
    STREAM_ERROR_MAX           /**< Guard for iterations */

} stream_error_e;

/*
 *   STRUCTS
 */

/* Stream structure */
typedef struct {
    
    char* buffer;           /**< Stream buffer */
    size_t size;            /**< Stream size */
    mutex_handler_t mutex;  /**< Flag indicates stream state of mutex */

} stream_handler_t;
/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
stream_error_e Stream_Init(stream_handler_t* stream, char* buffer, size_t size);

/*
 *   UNIT TEST
 */

#ifdef __cplusplus
}
#endif

#endif