#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "input_utils.h"

static int only_whitespace(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

int read_int(const char *prompt, int *out) {
    char buffer[64];
    char *end;
    long value;

    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);

    value = strtol(buffer, &end, 10);

    if (end == buffer) return 0;             /* nao leu nada */
    if (!only_whitespace(end)) return 0;     /* sobrou lixo ex: "12abc" */

    *out = (int)value;
    return 1;
}

int read_double(const char *prompt, double *out) {
    char buffer[64];
    char *end;

    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);

    *out = strtod(buffer, &end);

    if (end == buffer) return 0;
    if (!only_whitespace(end)) return 0;

    return 1;
}