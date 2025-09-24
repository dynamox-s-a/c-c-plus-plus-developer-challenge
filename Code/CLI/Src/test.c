/**
 * @file    test.c
 * @brief   TODO
 *
 * @details TODO
 *
 * @author  Emerson Isaias da Silva
 * @date    23-09-2025
 */

/*
 *   INCLUDES
 */
#include "unity.h"

#include "test_arithmetic_add.h"
#include "test_arithmetic_divide.h"
#include "test_arithmetic_determinant_n.h"
#include "test_arithmetic_log_n.h"
#include "test_arithmetic_multiply.h"
#include "test_arithmetic_power_n.h"
#include "test_arithmetic_root_n.h"
#include "test_arithmetic_subtract.h"
#include "test_cli_handler.h"
#include "test_logs_file.h"
#include "test_misc.h"

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

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
int test_function(void) {
    
    Test_Arithmetic_Add();
    Test_Arithmetic_Subtract();
    Test_Arithmetic_Multiply();
    Test_Arithmetic_Divide();
    Test_Arithmetic_Log_N();
    Test_Arithmetic_Determinant_N();
    Test_Arithmetic_Power_N();
    Test_Arithmetic_Root_N();
    Test_Cli_Handler();
    Test_Logs_File();
    Test_Misc();

    return 0;
}