//standard library
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "log.h"//log header
#include "typedef.h"//menu defines

FILE* open_log_file(const char *filename) {
    FILE *log_file = fopen(filename, "a");//append

    if (log_file == NULL) {//check if file is opened, if not return error
        return NULL;
    }

    time_t current_time;
    char *time_str;

    //get time
    time(&current_time);
    time_str = ctime(&current_time);
    time_str[strlen(time_str) - 1] = '\0';//remove lf

    fprintf(log_file, "[%s]-OPEN\n\n", time_str);

    return log_file;
}

void log_result(FILE *log_file, float result, float *list, unsigned int list_size, unsigned int operation_choice){
    time_t current_time;
    char *time_str;
    unsigned int temp_list_size = list_size;

    //get time
    time(&current_time);
    time_str = ctime(&current_time);
    time_str[strlen(time_str) - 1] = '\0';//remove lf

    char prefix[4] = "  ";
    switch (operation_choice){
        case SUM:
            strcpy(prefix, "SUM");
            break;
        case SUBTRACT:
            strcpy(prefix, "SUB");
            break;
        case MULTIPLY:
            strcpy(prefix, "MUL");
            break;
        case DIVIDE:
            strcpy(prefix, "DIV");
            break;
        case DETERMINANT:
            temp_list_size *= list_size;
            printf("temp_list_size = %d\n", temp_list_size);
            strcpy(prefix, "DET");
            break;
        //TODO: if new operation is added, add new case here
        default:
            strcpy(prefix, "ERR");  //if error
            break;
    }
    //separete variable with semicolon
    fprintf(log_file, "[%s]-%s:\n", time_str, prefix);
    fprintf(log_file, "%f", list[0]);
    for(int i=1; i<temp_list_size; i++){
        fprintf(log_file, ",%f", list[i]);
    }
    fprintf(log_file, ";r=%f\n\n", result);
}

void close_log_file(FILE *log_file) {
    if (log_file != NULL) {
        time_t current_time;
        char *time_str;

        time(&current_time);
        time_str = ctime(&current_time);

        time_str[strlen(time_str) - 1] = '\0';

        fprintf(log_file, "[%s]-CLOSE\n\n", time_str);

        fclose(log_file);
    }
}
