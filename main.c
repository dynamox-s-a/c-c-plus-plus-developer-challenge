#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "typedef.h"
#include "ihm.h"
#include "log.h"
#include "operations.h"

//MAIN
int main (){
    unsigned int operation_choice;
    void *list = NULL;
    unsigned int list_size;
    float result = 0.0;

    FILE *log_file = open_log_file("logfile.txt");

    while(true){
        print_main_menu();//print main menu
        operation_choice = get_user_input_uint();//get menu choice
        if(operation_choice == EXIT){//exit
            close_log_file(log_file);
            exit(1);
        }
        list = operation_get_values(operation_choice, &list_size);//get operation values
        result = operation_result(list, list_size, operation_choice);//get operation result
        print_result(result, list, list_size, operation_choice, log_file != NULL);//print operation result
        if(log_file != NULL){//if log file is opened, create a log
            log_result(log_file, result, list, list_size, operation_choice);
        }
        free(list);//free operantion values list
    }
    return 0;
}
