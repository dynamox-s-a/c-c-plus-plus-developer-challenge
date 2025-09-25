/**
 * @file    mutex.h
 * @brief   Simple mutex implementation for mutual exclusion.
 *
 * @details
 * This module provides a basic mutex (mutual exclusion) mechanism for synchronizing access
 * to shared resources. It includes functions to initialize, lock, and unlock a mutex handler.
 * The implementation uses a boolean flag and busy-waiting for demonstration or single-threaded use only.
 *
 * @author  Emerson Isaias da Silva
 * @date    25-09-2025
 */

#ifndef CLI_DRIVERS_MUTEX_H /* Include guard */
#define CLI_DRIVERS_MUTEX_H

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
typedef enum {

    MUTEX_ERROR_OK = 0,       /**< Return success */
    MUTEX_ERROR_INIT,         /**< No initializate correctly */
    MUTEX_ERROR_PARM,         /**< Incorrect param in function */

    /* Maximal value in this enumerate */
    MUTEX_ERROR_MAX           /**< Guard for iterations */

} mutex_error_e;

/*
 *   STRUCTS
 */

/* Mutex structure */
typedef struct {

    bool blocked;         /**< Flag indicates if mutex is blocked */

} mutex_handler_t;
/*
 *   UNIONS
 */

/*
 *   GLOBAL FUNCTIONS
 */

mutex_error_e Mutex_Init(mutex_handler_t* mutex);       /* Initialize mutex */
mutex_error_e Mutex_Lock(mutex_handler_t* mutex);       /* Lock mutex */
mutex_error_e Mutex_Unlock(mutex_handler_t* mutex);     /* Unlock mutex */

/*
 *   UNIT TEST
 */

#ifdef __cplusplus
}
#endif

#endif