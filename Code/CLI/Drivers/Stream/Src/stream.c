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

/*
 *   INCLUDES
 */
#include "stream.h"

#include "stdlib.h"
#include "string.h"

/*
 *   DEFINES
 */

/*
 *   MACROS
 */

/* Verify NULL parameter */
#define CHECK_STREAM_PTR(stream_ptr)       do { if ((stream_ptr) == NULL) return STREAM_ERROR_PARM; } while(0)

/* Verify value */
#define CHECK_STREAM_VALID(value, expected) do { if ((value) == (expected)) return STREAM_ERROR_PARM; } while(0)

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */

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
 * @brief Initializes a stream buffer handler.
 *
 *        Sets up the stream buffer, clears its contents, and initializes the associated mutex.
 *
 * @param stream Pointer to the stream handler structure.
 * @param buffer Pointer to the buffer to be managed by the stream handler.
 * @param size   Size of the buffer in bytes.
 * @return       STREAM_ERROR_OK on success, STREAM_ERROR_PARM if any parameter is invalid.
 */
stream_error_e Stream_Init(stream_handler_t* stream, char* buffer, size_t size) {

    CHECK_STREAM_PTR(stream);       /* Check stream pointer */
    CHECK_STREAM_PTR(buffer);       /* Check buffer pointer */
    CHECK_STREAM_VALID(size, 0);    /* Check size valid */

    Mutex_Init(&stream->mutex);     /* Initialize mutex */

    stream->buffer = buffer;        /* Set stream buffer */

    memset(stream->buffer, '\0', size);  /* Clean Stream buffer */

    stream->size = size;        /* Set stream size */

    return STREAM_ERROR_OK;     /* Return success */
}