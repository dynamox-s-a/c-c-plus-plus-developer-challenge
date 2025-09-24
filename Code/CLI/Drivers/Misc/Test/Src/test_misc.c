/**
 * @file    test_misc.h.c
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
#include "test_misc.h"

#include "misc.h"
#include "unity.h"

#include "stddef.h"


/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
void test_Misc_Strtok_R(void);
void test_Misc_Parser_Double(void);
void test_Misc_Parser_Double_NullPointer(void);
void test_Misc_Parser_Integer(void);
/*
 *   CONSTANTS
 */

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void test_Misc_Strtok_R(void) {

    char str[] = "Hello, world! This is a test.";
    const char delim[] = " ,.!?";
    char *nextp = NULL;
    char *token;

    Func_Misc_Strtok_R Test_Misc_Strtok_R;
    Test_Misc_Strtok_R = get_strtok_internal();

    token = Test_Misc_Strtok_R(str, delim, &nextp);
    TEST_ASSERT_EQUAL_STRING("Hello", token);

    token = Test_Misc_Strtok_R(NULL, delim, &nextp);
    TEST_ASSERT_EQUAL_STRING("world", token);

    token = Test_Misc_Strtok_R(NULL, delim, &nextp);
    TEST_ASSERT_EQUAL_STRING("This", token);

    token = Test_Misc_Strtok_R(NULL, delim, &nextp);
    TEST_ASSERT_EQUAL_STRING("is", token);

    token = Test_Misc_Strtok_R(NULL, delim, &nextp);
    TEST_ASSERT_EQUAL_STRING("a", token);

    token = Test_Misc_Strtok_R(NULL, delim, &nextp);
    TEST_ASSERT_EQUAL_STRING("test", token);

    token = Test_Misc_Strtok_R(NULL, delim, &nextp);
    TEST_ASSERT_NULL(token);
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void test_Misc_Parser_Double(void) {

    double value;
    misc_error_e err;

    Func_Misc_Parser_Double Test_Misc_Parser_Double;
    Test_Misc_Parser_Double = get_parser_double_internal();

    err = Test_Misc_Parser_Double(&value, "123.456");
    TEST_ASSERT_EQUAL(MISC_ERROR_OK, err);
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 123.456, value);

    err = Test_Misc_Parser_Double(&value, "0x1A");
    TEST_ASSERT_EQUAL(MISC_ERROR_OK, err);
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 26.0, value);

    err = Test_Misc_Parser_Double(&value, "invalid");
    TEST_ASSERT_EQUAL(MISC_ERROR_OK, err);
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 0.0, value); // atof returns 0.0 for invalid input
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void test_Misc_Parser_Double_NullPointer(void) {

    double value;
    misc_error_e err;

    Func_Misc_Parser_Double Test_Misc_Parser_Double;
    Test_Misc_Parser_Double = get_parser_double_internal();

    err = Test_Misc_Parser_Double(NULL, "123.456");
    TEST_ASSERT_EQUAL(MISC_ERROR_PARM, err);

    err = Test_Misc_Parser_Double(&value, NULL);
    TEST_ASSERT_EQUAL(MISC_ERROR_PARM, err);

    err = Test_Misc_Parser_Double(NULL, NULL);
    TEST_ASSERT_EQUAL(MISC_ERROR_PARM, err);
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void test_Misc_Parser_Integer(void) {

    int value;
    misc_error_e err;

    Func_Misc_Parser_Uint Test_Misc_Parser_Uint;
    Test_Misc_Parser_Uint = get_parser_uint_internal();

    err = Test_Misc_Parser_Uint(NULL, "123");
    TEST_ASSERT_EQUAL(MISC_ERROR_PARM, err);

    err = Test_Misc_Parser_Uint(&value, NULL);
    TEST_ASSERT_EQUAL(MISC_ERROR_PARM, err);

    err = Test_Misc_Parser_Uint(NULL, NULL);
    TEST_ASSERT_EQUAL(MISC_ERROR_PARM, err);
}

/**
 * @brief TODO
 * 
 * @param TODO
 * @return TODO
 */
void Test_Misc(void) {

    RUN_TEST(test_Misc_Strtok_R);
    RUN_TEST(test_Misc_Parser_Double);
    RUN_TEST(test_Misc_Parser_Double_NullPointer);
    RUN_TEST(test_Misc_Parser_Integer);
}
