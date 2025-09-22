/**
 * @file    cli_handler.h
 * @brief   TODO
 *
 * @details TODO
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

#ifndef CLI_DRIVERS_CLI_HANDLER_H /* Include guard */
#define CLI_DRIVERS_CLI_HANDLER_H

#ifdef __cplusplus  /* C++ guard */
extern "C" {
#endif

/*
 *   INCLUDES
 */
#include "stdbool.h"

/*
 *   DEFINES
 */

/*
 *   MACROS
 */

/*
 *   ENUMS
 */

 /**
 * @brief Enumerate of errors
 */
typedef enum {

    CLI_HANDLER_ERROR_OK = 0,       /**< Return success */
    CLI_HANDLER_ERROR_INIT,         /**< No initializate correctly */
    CLI_HANDLER_ERROR_PARM,         /**< Incorrect param in function */

    /* Maximal value in this enumerate */
    CLI_HANDLER_MAX                 /**< Guard for iterations */

} cli_handler_error_e;

typedef enum {

    CLI_HANDLER_CMD_ADD,            /**< Calculate sum */
    CLI_HANDLER_CMD_SUBTRACT,       /**< Calculate subtract */
    CLI_HANDLER_CMD_DIVIDE,         /**< Calculate divide */
    CLI_HANDLER_CMD_MULTIPLY,       /**< Calculate multiply */
    CLI_HANDLER_CMD_POWER_N,        /**< Calculate power */
    CLI_HANDLER_CMD_ROOT_N,         /**< Calculate root N */
    CLI_HANDLER_CMD_LOG_N,          /**< Calculate log N */
    CLI_HANDLER_CMD_DETERMINANT_N,  /**< Calculate determinant N */
    CLI_HANDLER_CMD_CLEAR,          /**< Clear ANS */
    CLI_HANDLER_CMD_HELP,           /**< Return information of application */
    CLI_HANDLER_CMD_EXIT,           /**< Exit of application */

    /* Maximal value in this enumerate */
    CLI_HANDLER_CMD_MAX             /**< Total number of commands */

} cli_handler_commans_e;

/*
 *   STRUCTS
 */

 /* Forward declaration*/
typedef struct cli_handler_t cli_handler_t;

/**
 * @brief Commands of cli
 */
typedef struct {

    const char* name;                                               /**< Command */
    const char* description;                                        /**< Description of command */
    cli_handler_error_e (*func_ptr)(cli_handler_t* cli_handler, char* saveptr);   /**< Function pointer */

} cli_handler_commands_t;

/**
 * @brief Cli Handlers callback
 */
typedef struct {



} cli_handler_callback_t;

/**
 * @brief Cli Handlers config
 */
typedef struct {

    bool multithread;   /**< Flag indicates app is multithread */
    bool logs;          /**< Enable logs to save */
    char* stream_rx;    /**< Strem of receive commands */
    char* stream_tx;    /**< Strem of send answer */

} cli_handler_config_t;

/**
 * @brief Handler of cli
 */
struct cli_handler_t{

    cli_handler_config_t config;        /**< Configuration */
    cli_handler_callback_t callback;    /**< Coordenada Y */
    void* param;

    bool init;          /**< Set true when initialize succesful */
    double ans;
};

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
cli_handler_error_e Cli_Handler_Init(   cli_handler_t* cli_handler, 
                                        cli_handler_callback_t* callback, 
                                        cli_handler_config_t* config, 
                                        void* param);
                                        
cli_handler_error_e Cli_Handler_Check(cli_handler_t* cli_handler);


#ifdef __cplusplus
}
#endif

#endif