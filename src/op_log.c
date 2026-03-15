#include <stdio.h>
#include <time.h>
#include "op_log.h"

#define LOG_PATH "output/operations.log"

static void write_log(const char *status, const char *operation,
                      const char *detail, const char *result_text) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    FILE *f = fopen(LOG_PATH, "a");

    if (f == NULL) return;

    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d | %s | %s | %s | %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            status, operation, detail, result_text);

    fclose(f);
}

void oplog_append_success(const char *operation, const char *detail, double result) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.6f", result);
    write_log("OK", operation, detail, buf);
}

void oplog_append_error(const char *operation, const char *detail, const char *error_msg) {
    write_log("ERR", operation, detail, error_msg);
}