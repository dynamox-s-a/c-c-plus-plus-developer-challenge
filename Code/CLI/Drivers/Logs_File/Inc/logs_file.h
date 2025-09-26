/**
 * @file    logs_file.h
 * @brief   Implementation of log file handling for CLI application.
 *
 * @details
 * This module provides functions to initialize, check, and save log entries
 * to a file. It uses a handler structure to manage file operations and
 * supports parsing log streams line by line.
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
#include "misc.h"
#include "stream.h"

#include "stdio.h"
#include "stdbool.h"

/*
 *   DEFINES
 */
#define LOGS_FILE_DATETIME_STR_LEN 20

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

typedef struct{

    stream_handler_t* stream;
    char* file_name;
    bool terminal;

}logs_file_handler_config_t;

/**
 * @brief Handler of logs file
 */
struct logs_file_handler_t{

    FILE* file;
    logs_file_handler_config_t config;
};

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
logs_file_handler_error_e Logs_File_Handler_Init(logs_file_handler_t* logs_file_handler, logs_file_handler_config_t* config);
logs_file_handler_error_e Logs_File_Handler_Check(logs_file_handler_t* logs_file_handler);
logs_file_handler_error_e Logs_File_Handler_Close(logs_file_handler_t* logs_file_handler);
#ifdef __cplusplus
}
#endif

#endif