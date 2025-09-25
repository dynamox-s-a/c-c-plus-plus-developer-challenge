/**
 * @file    mutex.c
 * @brief   Simple mutex implementation for mutual exclusion.
 *
 * @details
 * This module provides a basic mutex (mutual exclusion) mechanism for synchronizing access
 * to shared resources. It includes functions to initialize, lock, and unlock a mutex handler.
 * The implementation uses a boolean flag and busy-waiting for demonstration or single-threaded use only.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "mutex.h"

#include "stdbool.h"
#include "stdlib.h"


/*
 *   DEFINES
 */

/*
 *   MACROS
 */
/**
 * @brief Checks if a mutex pointer is NULL and returns MUTEX_ERROR_PARM if so.
 */
#define CHECK_MUTEX_PTR(mutex_ptr)        do { if ((mutex_ptr) == NULL) return MUTEX_ERROR_PARM; } while(0)

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */

/*
 *   CONSTANTS
 */

/*
 *   UNIT TEST
 */

/*
 *   IMPLEMENTATION
 */

/**
 * @brief Initializes a mutex handler.
 *
 *        Sets the mutex as unblocked (available).
 *
 * @param mutex Pointer to the mutex handler structure.
 * @return      MUTEX_ERROR_OK on success, MUTEX_ERROR_PARM if pointer is NULL.
 */
mutex_error_e Mutex_Init(mutex_handler_t* mutex) {
    CHECK_MUTEX_PTR(mutex); /* Check mutex pointer */
    mutex->blocked = false; /* Initialize mutex as unblocked */
    return MUTEX_ERROR_OK;  /* Return OK */
}

/**
 * @brief Locks the mutex handler (busy-wait).
 *
 *        Waits until the mutex is available, then locks it.
 *        (Not thread-safe in preemptive multitasking environments.)
 *
 * @param mutex Pointer to the mutex handler structure.
 * @return      MUTEX_ERROR_OK on success, MUTEX_ERROR_PARM if pointer is NULL.
 */
mutex_error_e Mutex_Lock(mutex_handler_t* mutex) {
    CHECK_MUTEX_PTR(mutex); /* Check mutex pointer */
    while (mutex->blocked == true) {
        /* Wait for mutex to be free */
    }
    mutex->blocked = true; /* Lock mutex */
    return MUTEX_ERROR_OK;  /* Return OK */
}

/**
 * @brief Unlocks the mutex handler.
 *
 *        Sets the mutex as unblocked (available).
 *
 * @param mutex Pointer to the mutex handler structure.
 * @return      MUTEX_ERROR_OK on success, MUTEX_ERROR_PARM if pointer is NULL.
 */
mutex_error_e Mutex_Unlock(mutex_handler_t* mutex) {
    CHECK_MUTEX_PTR(mutex); /* Check mutex pointer */
    mutex->blocked = false; /* Unlock mutex */
    return MUTEX_ERROR_OK;  /* Return OK */
}