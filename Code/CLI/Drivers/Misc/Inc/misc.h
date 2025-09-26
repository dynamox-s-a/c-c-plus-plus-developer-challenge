/**
 * @file    misc.h
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

    MISC_ERROR_OK = 0,       /**< Return success */
    MISC_ERROR_PARM,         /**< Incorrect param in function */
    MISC_ERROR_ALLOC,        /**< Memory allocation error */

    /* Maximal value in this enumerate */
    MISC_ERROR_MAX           /**< Guard for iterations */

} misc_error_e;

/*
 *   STRUCTS
 */
typedef struct{

    double* data;
    size_t size;

} misc_vector_t;

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
char* Misc_Strtok_R(char *str, const char *delim, char **nextp);
misc_error_e Misc_Parser_Double(double* value, char* token);
misc_error_e Misc_Parser_Uint(uint32_t* value, char* token);

misc_error_e Misc_Get_Vector(misc_vector_t* vector, char* saveptr);
misc_error_e Misc_Free_Vector(misc_vector_t* vector);

/*
 *   UNIT TEST
 */

// Define a function type for the internal function
typedef char* (*Func_Misc_Strtok_R)(char*, const char*, char**);
typedef misc_error_e (*Func_Misc_Parser_Double)(double*, char*);
typedef misc_error_e (*Func_Misc_Parser_Uint)(uint32_t*, char*);

Func_Misc_Strtok_R get_strtok_internal(void);
Func_Misc_Parser_Double get_parser_double_internal(void);
Func_Misc_Parser_Uint get_parser_uint_internal(void);

#ifdef __cplusplus
}
#endif

#endif