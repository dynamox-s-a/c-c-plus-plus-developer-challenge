/**
 * @file test_simple.c
 * @brief Simple unit tests to verify core functionality.
 *
 * This file can be compiled and run independently to verify
 * that the core mathematical operations work correctly.
 */

#include "lib/operations/operations.h"
#include "lib/matrix/matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TEST_TOLERANCE 1e-9

static int test_count = 0;
static int test_passed = 0;

static void test_assert(const char *test_name, int condition)
{
    test_count++;
    if (condition) {
        printf("✓ %s\n", test_name);
        test_passed++;
    } else {
        printf("✗ %s\n", test_name);
    }
}

static void test_operations(void)
{
    printf("\n=== Testing Operations Library ===\n");
    
    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result;
    
    /* Test sum */
    op_sum(values, 5, &result);
    test_assert("Sum of [1,2,3,4,5] = 15", fabs(result - 15.0) < TEST_TOLERANCE);
    
    /* Test mean */
    op_mean(values, 5, &result);
    test_assert("Mean of [1,2,3,4,5] = 3", fabs(result - 3.0) < TEST_TOLERANCE);
    
    /* Test min */
    op_min(values, 5, &result);
    test_assert("Min of [1,2,3,4,5] = 1", fabs(result - 1.0) < TEST_TOLERANCE);
    
    /* Test max */
    op_max(values, 5, &result);
    test_assert("Max of [1,2,3,4,5] = 5", fabs(result - 5.0) < TEST_TOLERANCE);
    
    /* Test factorial */
    double fact_input = 5.0;
    op_factorial(&fact_input, 1, &result);
    test_assert("Factorial of 5 = 120", fabs(result - 120.0) < TEST_TOLERANCE);
    
    /* Test factorial of 0 */
    fact_input = 0.0;
    op_factorial(&fact_input, 1, &result);
    test_assert("Factorial of 0 = 1", fabs(result - 1.0) < TEST_TOLERANCE);
}

static void test_matrix(void)
{
    printf("\n=== Testing Matrix Library ===\n");
    
    /* Test 2x2 determinant */
    double **mat2x2;
    matrix_alloc(&mat2x2, 2, 2);
    
    mat2x2[0][0] = 1.0; mat2x2[0][1] = 2.0;
    mat2x2[1][0] = 3.0; mat2x2[1][1] = 4.0;
    
    double det;
    matrix_determinant(mat2x2, 2, &det);
    test_assert("Det([[1,2],[3,4]]) = -2", fabs(det - (-2.0)) < TEST_TOLERANCE);
    
    matrix_free(mat2x2, 2);
    
    /* Test 3x3 identity matrix */
    double **mat3x3;
    matrix_alloc(&mat3x3, 3, 3);
    
    /* Identity matrix */
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat3x3[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    
    matrix_determinant(mat3x3, 3, &det);
    test_assert("Det(3x3 identity) = 1", fabs(det - 1.0) < TEST_TOLERANCE);
    
    matrix_free(mat3x3, 3);
    
    /* Test singular matrix (all zeros) */
    double **mat_zero;
    matrix_alloc(&mat_zero, 2, 2);
    
    mat_zero[0][0] = 0.0; mat_zero[0][1] = 0.0;
    mat_zero[1][0] = 0.0; mat_zero[1][1] = 0.0;
    
    matrix_determinant(mat_zero, 2, &det);
    test_assert("Det(zero matrix) = 0", fabs(det - 0.0) < TEST_TOLERANCE);
    
    matrix_free(mat_zero, 2);
}

int main(void)
{
    printf("Dynamox Calculator - Simple Unit Tests\n");
    printf("======================================\n");
    
    test_operations();
    test_matrix();
    
    printf("\n=== Test Results ===\n");
    printf("Passed: %d/%d tests\n", test_passed, test_count);
    
    if (test_passed == test_count) {
        printf("✓ All tests passed!\n");
        return EXIT_SUCCESS;
    } else {
        printf("✗ Some tests failed.\n");
        return EXIT_FAILURE;
    }
}
