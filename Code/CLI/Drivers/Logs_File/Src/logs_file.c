/**
 * @file    logs_file.c
 * @brief   Implementation of log file handling for CLI application.
 *
 * @details
 * This module provides functions to initialize, check, and save log entries
 * to a file. It uses a handler structure to manage file operations and
 * supports parsing log streams line by line. It also provides utilities for
 * formatting date and time for log entries.
 *
 * @author  Emerson Isaias da Silva
 * @date    22-09-2025
 */

/*
 *   INCLUDES
 */
#include "logs_file.h"

#include "misc.h"
#include "mutex.h"

#include "time.h"
#include "stdio.h"
#include "string.h"

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
static logs_file_handler_error_e save_string(logs_file_handler_t* logs_file_handler, char* token);
static logs_file_handler_error_e get_data_time_now(char* datatime);
static logs_file_handler_error_e print_terminal(logs_file_handler_t* logs_file_handler, char* token);

/*
 *   CONSTANTS
 */

/**
 * @brief Initializes the log file handler.
 *
 *        Opens the log file for appending and prepares the handler for use.
 *
 * @param logs_file_handler Pointer to the log file handler structure.
 * @param stream            Pointer to the stream buffer.
 * @return                  LOGS_FILE_HANDLER_ERROR_OK on success, error code otherwise.
 */
logs_file_handler_error_e Logs_File_Handler_Init(logs_file_handler_t* logs_file_handler, logs_file_handler_config_t* config) {
   
    FILE *file = fopen(config->file_name, "a");

    LOGS_FILE_HANDLER_PTR(logs_file_handler);
    LOGS_FILE_HANDLER_PTR(file);

    logs_file_handler->file = file;
    logs_file_handler->config = *config;

    Logs_File_Handler_Check(logs_file_handler);

    return LOGS_FILE_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Checks the log file handler's stream for new log entries.
 *
 *        If a newline is found in the stream, splits the stream into lines
 *        and saves each line to the log file.
 *
 * @param logs_file_handler Pointer to the log file handler structure.
 * @return                  LOGS_FILE_HANDLER_ERROR_OK on success, error code otherwise.
 */
logs_file_handler_error_e Logs_File_Handler_Check(logs_file_handler_t* logs_file_handler) {
    
    char* token;
    char* saveptr;

    LOGS_FILE_HANDLER_PTR(logs_file_handler);       /* Check logs_file_handler pointer */
    LOGS_FILE_HANDLER_PTR(logs_file_handler->config.stream); /* Check stream pointer */

    Mutex_Lock(&logs_file_handler->config.stream->mutex);   /* Lock stream */

    if (strchr(logs_file_handler->config.stream->buffer, '\n') != NULL) {    /* Check for new line */

        token = Misc_Strtok_R(logs_file_handler->config.stream->buffer, "\n", &saveptr);

        while (token != NULL) {

            save_string(logs_file_handler, token);      /* Save log entry to file */
            print_terminal(logs_file_handler, token);   /* Print log entry to terminal */

            token = Misc_Strtok_R(NULL, "\n", &saveptr);
        }
        logs_file_handler->config.stream->buffer[0] = '\0';
    }

    Mutex_Unlock(&logs_file_handler->config.stream->mutex); /* Unlock stream */
    
    return LOGS_FILE_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Closes the log file associated with the handler.
 *
 * @param logs_file_handler Pointer to the log file handler structure.
 * @return                  LOGS_FILE_HANDLER_ERROR_OK on success, error code otherwise.
 */
logs_file_handler_error_e Logs_File_Handler_Close(logs_file_handler_t* logs_file_handler) {
    
    LOGS_FILE_HANDLER_PTR(logs_file_handler);       /* Check logs_file_handler pointer */
    LOGS_FILE_HANDLER_PTR(logs_file_handler->file); /* Check file pointer */

    fclose(logs_file_handler->file);    /* Close log file */
    logs_file_handler->file = NULL;     /* Set file pointer to NULL */

    return LOGS_FILE_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Saves a single log entry (token) to the log file.
 *
 *        Writes the token as a new line in the log file with a timestamp and closes the file.
 *
 * @param logs_file_handler Pointer to the log file handler structure.
 * @param token             String to be saved in the log file.
 * @return                  LOGS_FILE_HANDLER_ERROR_OK on success, error code otherwise.
 */
static logs_file_handler_error_e save_string(logs_file_handler_t* logs_file_handler, char* token) {

    char datatime[LOGS_FILE_DATETIME_STR_LEN];

    logs_file_handler->file = fopen(logs_file_handler->config.file_name, "a");

    LOGS_FILE_HANDLER_PTR(logs_file_handler);
    LOGS_FILE_HANDLER_PTR(logs_file_handler->file);
    LOGS_FILE_HANDLER_PTR(token);

    get_data_time_now(datatime);    /* Get current date and time */

    fprintf(logs_file_handler->file, "%s - %s\n", datatime, token); /* Write log entry */

    fclose(logs_file_handler->file);    /* Close log file */

    return LOGS_FILE_HANDLER_ERROR_OK;  /* Return OK */
}

/**
 * @brief Gets the current date and time as a formatted string.
 *
 * @param datatime Pointer to a buffer to store the formatted date/time string.
 * @return         LOGS_FILE_HANDLER_ERROR_OK on success.
 */
static logs_file_handler_error_e get_data_time_now(char* datatime) {

    time_t now;              /* Stores seconds since 1/1/1970 (epoch) */
    struct tm *time_info;    /* Structure with decomposed date/time */

    time(&now);                      /* Get current time */
    time_info = localtime(&now);     /* Convert to local time */

    strftime(datatime, LOGS_FILE_DATETIME_STR_LEN, "%Y-%m-%d %H:%M:%S", time_info); /* Format date/time as string */

    return LOGS_FILE_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Prints a log entry with a timestamp to the terminal.
 *
 *        Retrieves the current date and time, formats it, and prints it along with the log token
 *        to the terminal (stdout).
 *
 * @param logs_file_handler Pointer to the log file handler structure (checked for NULL).
 * @param token             String to be printed as the log entry (checked for NULL).
 * @return                  LOGS_FILE_HANDLER_ERROR_OK on success, error code otherwise.
 */
static logs_file_handler_error_e print_terminal(logs_file_handler_t* logs_file_handler, char* token) {

    char datatime[LOGS_FILE_DATETIME_STR_LEN];

    LOGS_FILE_HANDLER_PTR(logs_file_handler);
    LOGS_FILE_HANDLER_PTR(token);

    if(logs_file_handler->config.terminal == true){

        get_data_time_now(datatime);    /* Get current date and time */

        printf("%s - %s\n", datatime, token); /* Print log entry to terminal */
    }

    return LOGS_FILE_HANDLER_ERROR_OK;  /* Return OK */
}
