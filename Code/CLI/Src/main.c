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

/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
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

    Cli_Handler_Init();

    while(1){

        Cli_Handler_Check();

    }

    return 0;
}