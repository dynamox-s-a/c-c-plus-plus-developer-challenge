/**
 * @file    arithmetic_add.c
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

/*
 *   MACROS
 */

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
static cli_handler_error_e command_add(cli_handler_t* cli_handler, char* saveptr){

    double value;
    char* token;

    while ((token = Misc_Strtok_R(NULL, "[,]", &saveptr)) != NULL) {

        parser_double(&value, token);
        printf("Value str = %f\n", value);
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}