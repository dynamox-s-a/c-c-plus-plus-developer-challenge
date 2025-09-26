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
#include "controller.h"

#include "stdio.h"
#include "string.h"


/*
 *   DEFINES
 */

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

    controller_handler_t controller;    /* Controller handler */

    Controller_Init(&controller);       /* Initialize controller */

    while(1){   /* Infinit loop */

        Controller_Check(&controller);  /* Check controller */
    }

    return 0;
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