/**
 * @file    misc.h
 * @brief   TODO
 *
 * @details TODO
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
    MISC_ERROR_INIT,         /**< No initializate correctly */
    MISC_ERROR_PARM,         /**< Incorrect param in function */

    /* Maximal value in this enumerate */
    MISC_MAX                 /**< Guard for iterations */

} misc_error_e;

/*
 *   STRUCTS
 */

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
char* Misc_Strtok_R(char *str, const char *delim, char **nextp);
misc_error_e Misc_Parser_Double(double* value, char* token);
misc_error_e Misc_Parser_Uint(uint64_t* value, char* token);

#ifdef __cplusplus
}
#endif

#endif