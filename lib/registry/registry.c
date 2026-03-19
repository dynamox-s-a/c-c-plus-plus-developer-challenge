/**
 * @file registry.c
 * @brief Implementation of the operation registry.
 */

#include "registry.h"

#include <string.h>
#include <stdio.h>

/** Internal storage for registered operations */
static operation_t g_operations[REGISTRY_MAX_OPERATIONS];
static size_t g_count = 0;

void registry_init(void)
{
    memset(g_operations, 0, sizeof(g_operations));
    g_count = 0;
}

int registry_add_array_op(const char *name, input_mode_t mode, op_array_fn func)
{
    if (name == NULL || func == NULL) {
        return -1;
    }
    if (g_count >= REGISTRY_MAX_OPERATIONS) {
        fprintf(stderr, "Error: operation registry is full.\n");
        return -1;
    }
    if (mode == INPUT_MATRIX) {
        fprintf(stderr, "Error: use registry_add_matrix_op for matrix operations.\n");
        return -1;
    }

    operation_t *op = &g_operations[g_count];
    strncpy(op->name, name, REGISTRY_NAME_MAX_LEN - 1);
    op->name[REGISTRY_NAME_MAX_LEN - 1] = '\0';
    op->input_mode = mode;
    op->fn.array_fn = func;

    g_count++;
    return 0;
}

int registry_add_matrix_op(const char *name, op_matrix_fn func)
{
    if (name == NULL || func == NULL) {
        return -1;
    }
    if (g_count >= REGISTRY_MAX_OPERATIONS) {
        fprintf(stderr, "Error: operation registry is full.\n");
        return -1;
    }

    operation_t *op = &g_operations[g_count];
    strncpy(op->name, name, REGISTRY_NAME_MAX_LEN - 1);
    op->name[REGISTRY_NAME_MAX_LEN - 1] = '\0';
    op->input_mode = INPUT_MATRIX;
    op->fn.matrix_fn = func;

    g_count++;
    return 0;
}

size_t registry_count(void)
{
    return g_count;
}

const operation_t *registry_get(size_t index)
{
    if (index >= g_count) {
        return NULL;
    }
    return &g_operations[index];
}
