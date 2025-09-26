/**
 * @file    stream.c
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
 * @brief Reentrant string tokenizer (public domain by Charlie Gordon).
 *
 * @param str    The string to tokenize, or NULL to continue tokenizing the previous string.
 * @param delim  The delimiter characters.
 * @param nextp  Pointer to a char* variable that stores the next position.
 * @return       Pointer to the next token, or NULL if no more tokens are found.
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