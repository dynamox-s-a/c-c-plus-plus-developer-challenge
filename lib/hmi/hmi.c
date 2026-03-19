/**
 * @file hmi.c
 * @brief Implementation of the Human-Machine Interface.
 */

#include "hmi.h"
#include "../registry/registry.h"
#include "../matrix/matrix.h"
#include "../logger/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Maximum number of array elements a user can input */
#define MAX_ARRAY_SIZE 1000

/** Maximum matrix dimension */
#define MAX_MATRIX_DIM 20

/**
 * @brief Flush remaining characters from stdin after invalid input.
 */
static void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int hmi_read_double(double *value)
{
    if (value == NULL) {
        return -1;
    }
    if (scanf("%lf", value) != 1) {
        flush_stdin();
        fprintf(stderr, "  Error: please enter a valid number.\n");
        return -1;
    }
    return 0;
}

int hmi_read_positive_int(int *value)
{
    if (value == NULL) {
        return -1;
    }
    if (scanf("%d", value) != 1 || *value <= 0) {
        flush_stdin();
        fprintf(stderr, "  Error: please enter a positive integer.\n");
        return -1;
    }
    return 0;
}

int hmi_show_main_menu(void)
{
    size_t count = registry_count();

    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║      ARITHMETIC CALCULATOR HMI       ║\n");
    printf("  ╠══════════════════════════════════════╣\n");

    for (size_t i = 0; i < count; i++) {
        const operation_t *op = registry_get(i);
        if (op != NULL) {
            printf("  ║  [%zu] %-32s ║\n", i + 1, op->name);
        }
    }

    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║  [%zu] View Operation Log             ║\n", count + 1);
    printf("  ║  [0] Exit                            ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("\n  Select an option: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        flush_stdin();
        return -1;
    }
    return choice;
}

void hmi_execute_array_op(int op_index)
{
    const operation_t *op = registry_get((size_t)op_index);
    if (op == NULL) {
        fprintf(stderr, "  Error: invalid operation index.\n");
        return;
    }

    double *values = NULL;
    size_t count = 0;

    if (op->input_mode == INPUT_SINGLE) {
        /* Single value input */
        count = 1;
        values = (double *)malloc(sizeof(double));
        if (values == NULL) {
            fprintf(stderr, "  Error: memory allocation failed.\n");
            return;
        }

        printf("\n  Enter a value: ");
        if (hmi_read_double(&values[0]) != 0) {
            free(values);
            return;
        }
    } else {
        /* Array input */
        int n;
        printf("\n  Enter the number of elements (1-%d): ", MAX_ARRAY_SIZE);
        if (hmi_read_positive_int(&n) != 0 || n > MAX_ARRAY_SIZE) {
            fprintf(stderr, "  Error: invalid array size.\n");
            return;
        }
        count = (size_t)n;

        values = (double *)malloc(count * sizeof(double));
        if (values == NULL) {
            fprintf(stderr, "  Error: memory allocation failed.\n");
            return;
        }

        printf("  Enter %zu value(s):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("    [%zu]: ", i + 1);
            if (hmi_read_double(&values[i]) != 0) {
                free(values);
                return;
            }
        }
    }

    /* Execute operation */
    double result = 0.0;
    int status = op->fn.array_fn(values, count, &result);

    if (status != 0) {
        printf("\n  Operation failed (error code: %d).\n", status);
        switch (status) {
            case 3: /* OP_ERR_DIV_ZERO */
                printf("  Reason: division by zero.\n");
                break;
            case 4: /* OP_ERR_INVALID_INPUT */
                printf("  Reason: invalid input for this operation.\n");
                break;
            default:
                printf("  Reason: check input values and try again.\n");
                break;
        }
    } else {
        printf("\n  ┌─────────────────────────────────┐\n");
        printf("  │  Result: %-22.6g  │\n", result);
        printf("  └─────────────────────────────────┘\n");

        /* Log the operation */
        logger_log_array_op(op->name, values, count, result);
    }

    free(values);
}

void hmi_execute_matrix_op(int op_index)
{
    const operation_t *op = registry_get((size_t)op_index);
    if (op == NULL) {
        fprintf(stderr, "  Error: invalid operation index.\n");
        return;
    }

    int rows_int, cols_int;

    printf("\n  Enter number of rows (1-%d): ", MAX_MATRIX_DIM);
    if (hmi_read_positive_int(&rows_int) != 0 || rows_int > MAX_MATRIX_DIM) {
        fprintf(stderr, "  Error: invalid dimension.\n");
        return;
    }

    printf("  Enter number of columns (1-%d): ", MAX_MATRIX_DIM);
    if (hmi_read_positive_int(&cols_int) != 0 || cols_int > MAX_MATRIX_DIM) {
        fprintf(stderr, "  Error: invalid dimension.\n");
        return;
    }

    size_t rows = (size_t)rows_int;
    size_t cols = (size_t)cols_int;

    /* Allocate matrix */
    double **mat = NULL;
    matrix_status_t mat_status = matrix_alloc(&mat, rows, cols);
    if (mat_status != MATRIX_OK) {
        fprintf(stderr, "  Error: could not allocate matrix (error %d).\n", mat_status);
        return;
    }

    /* Read matrix elements */
    printf("  Enter matrix elements:\n");
    mat_status = matrix_read(mat, rows, cols);
    if (mat_status != MATRIX_OK) {
        fprintf(stderr, "  Error: invalid matrix input.\n");
        matrix_free(mat, rows);
        return;
    }

    printf("\n  Input matrix:");
    matrix_print(mat, rows, cols);

    /* Execute operation */
    double result = 0.0;
    int status = op->fn.matrix_fn(mat, rows, cols, &result);

    if (status != 0) {
        printf("\n  Operation failed (error code: %d).\n", status);
        if (status == (int)MATRIX_ERR_INVALID_DIM) {
            printf("  Reason: this operation requires a square matrix.\n");
        }
    } else {
        printf("  ┌─────────────────────────────────┐\n");
        printf("  │  Result: %-22.6g  │\n", result);
        printf("  └─────────────────────────────────┘\n");

        logger_log_matrix_op(op->name, rows, cols, result);
    }

    matrix_free(mat, rows);
}

void hmi_run(void)
{
    int running = 1;

    while (running) {
        int choice = hmi_show_main_menu();

        if (choice == 0) {
            printf("\n  Goodbye!\n\n");
            running = 0;
        } else if (choice == (int)(registry_count() + 1)) {
            /* View log */
            int n;
            printf("  How many entries to show? (0 = all): ");
            if (scanf("%d", &n) != 1 || n < 0) {
                flush_stdin();
                printf("  Invalid input.\n");
                continue;
            }
            logger_show((size_t)n);
        } else if (choice >= 1 && choice <= (int)registry_count()) {
            int idx = choice - 1;
            const operation_t *op = registry_get((size_t)idx);
            if (op == NULL) {
                printf("  Error: operation not found.\n");
                continue;
            }

            if (op->input_mode == INPUT_MATRIX) {
                hmi_execute_matrix_op(idx);
            } else {
                hmi_execute_array_op(idx);
            }
        } else {
            printf("  Invalid option. Please try again.\n");
        }
    }
}
