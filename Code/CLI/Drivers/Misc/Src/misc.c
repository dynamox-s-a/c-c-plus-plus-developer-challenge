/**
 * @file    misc.c
 * @brief   Utility functions for string parsing and conversion.
 *
 * @details
 * This module provides miscellaneous helper functions, including:
 *   - A reentrant string tokenizer (strtok_r)
 *   - String to double and unsigned integer parsers with hexadecimal support
 *   - Getter functions to retrieve function pointers for unit testing
 *   - Dynamic vector allocation and freeing for parsed values
 * 
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "misc.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 *   MACROS
 */
/* Verify NULL parameter */
#define CHECK_MISC_PTR(misc_ptr)        do { if ((misc_ptr) == NULL) return MISC_ERROR_PARM; } while(0)

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

/**
 * @brief Returns a pointer to the reentrant string tokenizer function.
 * @return Function pointer to Misc_Strtok_R.
 */
Func_Misc_Strtok_R get_strtok_internal(void) {
    return Misc_Strtok_R;
}

/**
 * @brief Returns a pointer to the double parser function.
 * @return Function pointer to Misc_Parser_Double.
 */
Func_Misc_Parser_Double get_parser_double_internal(void) {
    return Misc_Parser_Double;
}

/**
 * @brief Returns a pointer to the unsigned integer parser function.
 * @return Function pointer to Misc_Parser_Uint.
 */
Func_Misc_Parser_Uint get_parser_uint_internal(void) {
    return Misc_Parser_Uint;
}

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
char* Misc_Strtok_R(char *str, const char *delim, char **nextp) {
    
    char *ret;

    if (str == NULL){

        str = *nextp;
    }

    str += strspn(str, delim);

    if (*str == '\0'){

        return NULL;
    }

    ret = str;

    str += strcspn(str, delim);

    if (*str){

        *str++ = '\0';
    }

    *nextp = str;

    return ret;
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
misc_error_e Misc_Parser_Double(double* value, char* token){

    CHECK_MISC_PTR(token);
    CHECK_MISC_PTR(value);

    if (strncmp(token, "0x", 2) == 0 || strncmp(token, "0X", 2) == 0) {
        
        *value = (double)strtol(token, NULL, 16);
    }
    else {
        *value = atof(token);
    }

    return MISC_ERROR_OK; /* Return OK */
}

/**
 * @brief Parses a string token into an unsigned 32-bit integer value.
 *
 *        Supports decimal and hexadecimal (prefix "0x" or "0X") formats.
 *
 * @param value Pointer to the uint32_t to store the result.
 * @param token String token to parse.
 * @return      MISC_ERROR_OK on success, MISC_ERROR_PARM if a parameter is NULL.
 */
misc_error_e Misc_Parser_Uint(uint32_t* value, char* token){

    CHECK_MISC_PTR(token);
    CHECK_MISC_PTR(value);

    if (strncmp(token, "0x", 2) == 0 || strncmp(token, "0X", 2) == 0) {
        
        *value = (uint32_t)strtol(token, NULL, 16);
    }
    else {
        *value = (uint32_t)atoi(token);
    }

    return MISC_ERROR_OK; /* Return OK */
}

misc_error_e Misc_Get_Vector(misc_vector_t* vector, char* saveptr){

    char* token;

    CHECK_MISC_PTR(saveptr);

    vector->data = NULL;
    vector->size = 0;

    while ((token = Misc_Strtok_R(NULL, "[,]", &saveptr)) != NULL) { /* Tokenize input */

        double *tmp = realloc(vector->data, (vector->size + 1) * sizeof(double));

        if (tmp == NULL) {

            free(vector->data); /* Free vector memory */
            free(vector);       /* Free vector structure */

            return MISC_ERROR_ALLOC; /* Return error allocation */
        }
        vector->data = tmp;
        Misc_Parser_Double(&vector->data[vector->size], token);
        vector->size++;
    }
    return MISC_ERROR_OK;
}

/**
 * @brief Frees the memory allocated for a misc_vector_t structure.
 *
 * @param vector Pointer to the misc_vector_t structure to free.
 * @return       MISC_ERROR_OK.
 */
misc_error_e Misc_Free_Vector(misc_vector_t* vector) {
    free(vector->data);

    return MISC_ERROR_OK; /* Return OK */
}