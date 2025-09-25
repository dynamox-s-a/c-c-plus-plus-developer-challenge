/**
 * @file    main.c
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
#include "cli_handler.h"

#include "logs_file.h"
#include "misc.h"

#include "stdio.h"
#include "string.h"


/*
 *   DEFINES
 */
#define MAIN_STREAM_SIZE        4096

/*
 *   MACROS
 */


/*
 *   EXTERNAL VARIABLES
 */

 /*
 *   CONSTANTS
 */

/*
 *   LOCAL FUNCTIONS
 */
void receive_terminal_command(cli_handler_t* cli_handler, char* stream_in, size_t size_stream);

/**
 * @brief Functon to start aplication
 *
 * @return 0
 */
int main(void) {

    logs_file_handler_t logs_file_handler;  /* Log file handler */
    cli_handler_t cli_handler;              /* CLI handler */

    char stream_in[MAIN_STREAM_SIZE];   /* Stream where send commands to CLI */
    char stream_out[MAIN_STREAM_SIZE];  /* Stream where receive answere from CLI */

    memset(stream_in, '\0', MAIN_STREAM_SIZE);  /* Clean Stream rx CLI */
    memset(stream_out, '\0', MAIN_STREAM_SIZE); /* Clean Stream tx CLI */

    cli_handler_config_t config_cli = {     /* Configuration of CLI */
        .multithread = false,           /* Flag to use multithread */
        .decimal = 5,                   /* Decimal of ANS when answer a command */
        .stream_rx = stream_in,         /* Set stream where send commands to CLI */
        .stream_tx = stream_out         /* Ser stream where receive answere from CLI */

        /* Mutex */
    };

    logs_file_handler_config_t config_logs = {

        .file_name = "log.txt",         /* File name to save logs */
        .stream = stream_out,           /* Stream where receive logs */
        .terminal = false                 /* Flag to print log in terminal */
    };

    Cli_Handler_Init(&cli_handler, &config_cli, NULL);        /* Initialize handler CLI */
    Logs_File_Handler_Init(&logs_file_handler, &config_logs); /* Initialize handler log file */

    while(1){   /* Infinit loop */

        receive_terminal_command(&cli_handler, stream_in, sizeof(stream_in)); /* Receive command from terminal */
        Logs_File_Handler_Check(&logs_file_handler); /* Check if have a new log to save */
    }

    Logs_File_Handler_Close(&logs_file_handler); /* Close log file handler */

    return 0;
}

/**
 * @brief This function will check if receive a correct command and process
 * 
 * @param cli_handler Pointer of handler CLI
 * @param stream_in Stream rx data
 * @param size_stream Sizeof stream
 */
void receive_terminal_command(cli_handler_t* cli_handler, char* stream_in, size_t size_stream){

    if (fgets(stream_in, size_stream, stdin) != NULL) { /* Get terminal commands */

        stream_in[strcspn(stream_in, "\n")] = 0;        /* Remove \n of stream and set as 0 */

        if(strlen(stream_in) > 0){                      /* Verify CLI case have a new command */

            Cli_Handler_Check(cli_handler);             /* Verify handler CLI */
        }
    }
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void setUp(void) {
    
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void tearDown(void) {
    
}