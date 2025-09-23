/**
 * @file    logs_file.h
 * @brief   TODO
 *
 * @details TODO
 *
 * @author  Emerson Isaias da Silva
 * @date    22-09-2025
 */

#ifndef CLI_DRIVERS_LOGS_FILE_H /* Include guard */
#define CLI_DRIVERS_LOGS_FILE_H

#ifdef __cplusplus  /* C++ guard */
extern "C" {
#endif

/*
 *   INCLUDES
 */
#include "stdio.h"

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

    LOGS_FILE_HANDLER_ERROR_OK = 0,       /**< Return success */
    LOGS_FILE_HANDLER_ERROR_INIT,         /**< No initializate correctly */
    LOGS_FILE_HANDLER_ERROR_PARM,         /**< Incorrect param in function */

    /* Maximal value in this enumerate */
    LOGS_FILE_HANDLER_ERROR_MAX           /**< Guard for iterations */

} logs_file_handler_error_e;

/*
 *   STRUCTS
 */
/* Forward declaration*/
typedef struct logs_file_handler_t logs_file_handler_t;

/**
 * @brief Handler of logs file
 */
struct logs_file_handler_t{

    FILE* file;
    char* stream;
};

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
logs_file_handler_error_e Logs_File_Handler_Init(logs_file_handler_t* logs_file_handler, char* stream);
logs_file_handler_error_e Logs_File_Handler_Check(logs_file_handler_t* logs_file_handler, char* stream);

#ifdef __cplusplus
}
#endif

#endif