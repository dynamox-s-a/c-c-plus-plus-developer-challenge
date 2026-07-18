#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "log.h"

void log_operation(const char* operation_name, double result) {
    FILE* file = fopen("operation_log.txt", "a");
    if (file == NULL) {
        printf("Erro ao abrir arquivo de log.\n");
        return;
    }

    time_t now = time(NULL);
    fprintf(file, "%s - Operação: %s, Resultado: %lf\n", ctime(&now), operation_name, result);
    fclose(file);
}