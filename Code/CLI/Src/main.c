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

    char stream[100];

    cli_handler_t cli_handler;
    cli_handler_callback_t callback;

    cli_handler_config_t config = {
        .logs = true,
        .multithread = false,
        .stream = stream
    };
    
    Cli_Handler_Init(&cli_handler, &callback, &config, NULL);

    while(1){

        if (fgets(stream, sizeof(stream), stdin) != NULL) {

            stream[strcspn(stream, "\n")] = 0; /* Remove \n of stream and set as 0 */
            Cli_Handler_Check(&cli_handler);
        }
    }

    return 0;
}