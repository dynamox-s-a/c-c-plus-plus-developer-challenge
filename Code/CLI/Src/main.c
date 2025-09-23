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
#include <stdio.h>
#include "cli_handler.h"
#include "string.h"

/*
 *   DEFINES
 */
#define MAIN_STREAM_SIZE        100

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


/**
 * @brief Functon to start aplication
 *
 * @return 0
 */
int main(void) {

    char stream_in[MAIN_STREAM_SIZE];   /* Stream where send commands to CLI */
    char stream_out[MAIN_STREAM_SIZE];  /* Stream where receive answere from CLI */

    cli_handler_t cli_handler;          /* CLI handler */
    cli_handler_callback_t callback;

    cli_handler_config_t config = {     /* Configuration of CLI */
        .logs = true,                   /* Flag to use stream tx for logs */
        .multithread = false,           /* Flag to use multithread */
        .decimal = 5,                   /* Decimal of ANS when answer a command */
        .stream_rx = stream_in,         /* Set stream where send commands to CLI */
        .stream_tx = stream_out         /* Ser stream where receive answere from CLI */
    };
    
    Cli_Handler_Init(&cli_handler, &callback, &config, NULL);   /* Initialize handler CLI */

    while(1){   /* Infinit loop */

        if (fgets(stream_in, sizeof(stream_in), stdin) != NULL) { /* Get terminal commands */

            stream_in[strcspn(stream_in, "\n")] = 0;    /* Remove \n of stream and set as 0 */

            if(strlen(stream_in) > 0){                  /* Verify CLI case have a new command */

                Cli_Handler_Check(&cli_handler);        /* Verify handler CLI */
            }
        }
    }

    return 0;
}