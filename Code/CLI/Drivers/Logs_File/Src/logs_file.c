/**
 * @file    logs_file.c
 * @brief   TODO
 *
 * @details TODO
 *
 * @author  Emerson Isaias da Silva
 * @date    22-09-2025
 */

/*
 *   INCLUDES
 */
#include "logs_file.h"

#include "misc.h"


/*
 *   MACROS
 */

/* Verify NULL parameter */
#define LOGS_FILE_HANDLER_PTR(logs_file_handler)      do { if ((logs_file_handler) == NULL) return LOGS_FILE_HANDLER_ERROR_PARM; } while(0)

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */

/*
 *   CONSTANTS
 */

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
logs_file_handler_error_e Logs_File_Handler_Init(logs_file_handler_t* logs_file_handler, char* stream){

    FILE *file = fopen("log.txt", "a");

    return LOGS_FILE_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief TODO
 * 
 * @param logs_file_handler Pointer of handler logs file
 * @return logs_file_handler_error_e
 */
logs_file_handler_error_e Logs_File_Handler_Check(logs_file_handler_t* logs_file_handler){

    char* token;
    char* saveptr;

    LOGS_FILE_HANDLER_PTR(logs_file_handler);
    LOGS_FILE_HANDLER_PTR(logs_file_handler->file);

    if(strchr(logs_file_handler->stream, '\n') != NULL){

        token = Misc_Strtok_R(logs_file_handler->stream, "\n", &saveptr);

        while (token != NULL) {

            printf("%s\n", token);
            fprintf(logs_file_handler->file, "%s\n", token);
            fclose(logs_file_handler->file);

            token = Misc_Strtok_R(NULL, "\n", &saveptr);
        }
        logs_file_handler->stream[0] = '\0';
    }

    return LOGS_FILE_HANDLER_ERROR_OK; /* Return OK */
}