/**
 * @file    misc.c
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
#include "misc.h"

#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"

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

// Getter: function to return the internal function pointer
Func_Misc_Strtok_R get_strtok_internal(void) {
    return Misc_Strtok_R;
}

Func_Misc_Parser_Double get_parser_double_internal(void) {
    return Misc_Parser_Double;
}

Func_Misc_Parser_Uint get_parser_uint_internal(void) {
    return Misc_Parser_Uint;
}

/* 
 * public domain strtok_r() by Charlie Gordon
 *
 *   from comp.lang.c  9/14/2007
 *
 *      http://groups.google.com/group/comp.lang.c/msg/2ab1ecbb86646684
 *
 *     (Declaration that it's public domain):
 *      http://groups.google.com/group/comp.lang.c/msg/7c7b39328fefab9c
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
 * @brief TODO
 * 
 * @param value Pointer of value double
 * @param token Pointer of token receive
 * @return TODO
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
 * @brief TODO
 * 
 * @param value Pointer of value double
 * @param token Pointer of token receive
 * @return TODO
 */
misc_error_e Misc_Parser_Uint(uint64_t* value, char* token){

    CHECK_MISC_PTR(token);
    CHECK_MISC_PTR(value);

    if (strncmp(token, "0x", 2) == 0 || strncmp(token, "0X", 2) == 0) {
        
        *value = (uint64_t)strtol(token, NULL, 16);
    }
    else {
        *value = (uint64_t)atoi(token);
    }

    return MISC_ERROR_OK; /* Return OK */
}