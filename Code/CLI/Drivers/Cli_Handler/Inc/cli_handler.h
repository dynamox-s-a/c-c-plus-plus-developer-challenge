/**
 * @file    cli_handler.h
 * @brief   CLI Handler - Receive commands and process
 *
 * @details Receive a command from stream rx and return answare in stream tx
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
#include "misc.h"
#include "stream.h"

#include "stdbool.h"
#include "stdint.h"

/*
 *   DEFINES
 */

/*
 *   MACROS
 */

/* Verify function return */
#define CHECK_CLI_HANDLER_FUNC_RET(func)        do { int err = (func); if (err != (CLI_HANDLER_ERROR_OK)) return err; } while (0)

/* Verify NULL parameter */
#define CHECK_CLI_HANDLER_PTR(cli_handler)      do { if ((cli_handler) == NULL) return CLI_HANDLER_ERROR_PARM; } while(0)

/* Verify if initializate correctly */
#define CHECK_CLI_HANDLER_INIT(cli_handler)     do { if ((cli_handler)->init == false) return CLI_HANDLER_ERROR_INIT; } while(0)


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
    CLI_HANDLER_CMD_DECIMAL,        /**< Change decimal answere */
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
 * @brief Cli Handlers config
 */
typedef struct {

    bool multithread;   /**< Flag indicates app is multithread */
    stream_handler_t* stream_rx;    /**< Strem of receive commands */
    stream_handler_t* stream_tx;    /**< Strem of send answer */
    uint32_t decimal;

} cli_handler_config_t;

/**
 * @brief Handler of cli
 */
struct cli_handler_t{

    cli_handler_config_t config;    /**< Configuration */
    void* param;                    /**< Used for recover context */

    bool init;                      /**< Set true when initialize succesful */
    double ans;                     /**< Value ANS calculated */
};

/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */
cli_handler_error_e Cli_Handler_Init(   cli_handler_t* cli_handler,
                                        cli_handler_config_t* config, 
                                        void* param);
                                        
cli_handler_error_e Cli_Handler_Check(cli_handler_t* cli_handler);
cli_handler_error_e Cli_Handler_Out_Msg(cli_handler_t* cli_handler, const char* format, ...);


#ifdef __cplusplus
}
#endif

#endif