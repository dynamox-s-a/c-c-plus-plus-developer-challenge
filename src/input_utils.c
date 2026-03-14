#include <stdio.h>
#include <stdlib.h>
#include "input_utils.h"

int read_int(const char *prompt, int *out) {
    char buffer[64];
    char *end;

    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);

    *out = (int)strtol(buffer, &end, 10);
    return end != buffer;
}

int read_double(const char *prompt, double *out) {
    char buffer[64];
    char *end;

    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);

    *out = strtod(buffer, &end);
    return end != buffer;
}