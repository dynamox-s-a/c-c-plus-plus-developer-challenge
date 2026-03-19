/**
 * @file logger.c
 * @brief Implementation of persistent operation log.
 */

#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

/** Internal file handle for the log */
static FILE *g_log_file = NULL;

int logger_init(void)
{
    g_log_file = fopen(LOG_FILE_PATH, "a");
    if (g_log_file == NULL) {
        fprintf(stderr, "Warning: could not open log file '%s'.\n", LOG_FILE_PATH);
        return -1;
    }
    return 0;
}

/**
 * @brief Get a formatted timestamp string.
 */
static void get_timestamp(char *buf, size_t buf_size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t != NULL) {
        strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", t);
    } else {
        strncpy(buf, "unknown", buf_size);
    }
}

void logger_log_array_op(const char *op_name, const double *values,
                         size_t count, double result)
{
    if (g_log_file == NULL || op_name == NULL) {
        return;
    }

    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(g_log_file, "[%s] %s | inputs:", timestamp, op_name);

    for (size_t i = 0; i < count; i++) {
        fprintf(g_log_file, " %.6g", values[i]);
        if (i < count - 1) {
            fprintf(g_log_file, ",");
        }
    }

    fprintf(g_log_file, " | result: %.6g\n", result);
    fflush(g_log_file);
}

void logger_log_matrix_op(const char *op_name, size_t rows, size_t cols,
                          double result)
{
    if (g_log_file == NULL || op_name == NULL) {
        return;
    }

    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(g_log_file, "[%s] %s | matrix: %zux%zu | result: %.6g\n",
            timestamp, op_name, rows, cols, result);
    fflush(g_log_file);
}

void logger_show(size_t count)
{
    FILE *f = fopen(LOG_FILE_PATH, "r");
    if (f == NULL) {
        printf("  No operation log found.\n");
        return;
    }

    /* Read all lines into a circular buffer if count > 0 */
    if (count == 0) {
        /* Show all entries */
        char line[LOG_ENTRY_MAX_LEN];
        printf("\n  ===== Operation Log =====\n");
        while (fgets(line, sizeof(line), f) != NULL) {
            printf("  %s", line);
        }
        printf("  =========================\n\n");
    } else {
        /*
         * Show last N entries.
         * Strategy: read all lines, keep the last 'count' in a ring buffer.
         */
        char **lines = NULL;
        size_t total = 0;
        size_t capacity = 64;

        lines = (char **)malloc(capacity * sizeof(char *));
        if (lines == NULL) {
            fclose(f);
            printf("  Error: memory allocation failed.\n");
            return;
        }

        char buf[LOG_ENTRY_MAX_LEN];
        while (fgets(buf, sizeof(buf), f) != NULL) {
            if (total >= capacity) {
                capacity *= 2;
                char **tmp = (char **)realloc(lines, capacity * sizeof(char *));
                if (tmp == NULL) {
                    /* Free what we have and bail */
                    for (size_t i = 0; i < total; i++) {
                        free(lines[i]);
                    }
                    free(lines);
                    fclose(f);
                    printf("  Error: memory allocation failed.\n");
                    return;
                }
                lines = tmp;
            }
            lines[total] = (char *)malloc(strlen(buf) + 1);
            if (lines[total] != NULL) {
                strcpy(lines[total], buf);
            }
            total++;
        }

        size_t start = (total > count) ? (total - count) : 0;
        printf("\n  ===== Last %zu Log Entries =====\n", count);
        for (size_t i = start; i < total; i++) {
            if (lines[i] != NULL) {
                printf("  %s", lines[i]);
            }
        }
        printf("  ================================\n\n");

        for (size_t i = 0; i < total; i++) {
            free(lines[i]);
        }
        free(lines);
    }

    fclose(f);
}

void logger_close(void)
{
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
}
