/**
 * @file main.c
 * @brief Entry point for the Dynamox Arithmetic Calculator.
 *
 * Initializes all subsystems (registry, logger), registers the
 * built-in operations, and launches the HMI main loop.
 *
 * Architecture:
 *   main.c
 *     ├── lib/registry   - Operation registration & lookup
 *     ├── lib/operations  - Scalar/array arithmetic operations
 *     ├── lib/matrix      - Matrix operations (determinant, etc.)
 *     ├── lib/logger      - Persistent operation log
 *     └── lib/hmi         - Console-based Human-Machine Interface
 */

#include "lib/registry/registry.h"
#include "lib/operations/operations.h"
#include "lib/matrix/matrix.h"
#include "lib/logger/logger.h"
#include "lib/hmi/hmi.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Wrapper to adapt matrix_determinant to the registry's matrix_fn signature.
 *
 * Validates that the matrix is square before computing the determinant.
 */
static int determinant_wrapper(double **mat, size_t rows, size_t cols, double *result)
{
    if (rows != cols) {
        fprintf(stderr, "  Error: determinant requires a square matrix (%zux%zu given).\n",
                rows, cols);
        return (int)MATRIX_ERR_INVALID_DIM;
    }
    return (int)matrix_determinant(mat, rows, result);
}

/**
 * @brief Register all built-in operations into the registry.
 */
static void register_builtin_operations(void)
{
    registry_add_matrix_op("Determinant (Matrix)", determinant_wrapper);
    registry_add_array_op("Sum (Array)", INPUT_ARRAY, (op_array_fn)op_sum);
    registry_add_array_op("Mean / Average (Array)", INPUT_ARRAY, (op_array_fn)op_mean);
    registry_add_array_op("Minimum (Array)", INPUT_ARRAY, (op_array_fn)op_min);
    registry_add_array_op("Maximum (Array)", INPUT_ARRAY, (op_array_fn)op_max);
    registry_add_array_op("Factorial (Single Value)", INPUT_SINGLE, (op_array_fn)op_factorial);
}

int main(void)
{
    /* Initialize subsystems */
    registry_init();
    register_builtin_operations();

    if (logger_init() != 0) {
        fprintf(stderr, "Warning: logging is disabled (could not open log file).\n");
    }

    printf("\n  =============================================\n");
    printf("    Dynamox Arithmetic Calculator v1.0\n");
    printf("    Developed for the C/C++ Developer Challenge\n");
    printf("  =============================================\n");

    /* Run the HMI event loop */
    hmi_run();

    /* Cleanup */
    logger_close();

    return EXIT_SUCCESS;
}
