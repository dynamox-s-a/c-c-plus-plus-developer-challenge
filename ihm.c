//standard library
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ihm.h"//ihm header
#include "typedef.h"//menu defines

float get_user_input_float(void) {
    float user_input;
    int check_float;

    while (1) {
        check_float = scanf("%f", &user_input); // check if number is a float

        if (check_float == 1) {
            return user_input;
        } else {
            printf("invalid input, please insert a float:\n");
            while (getchar() != '\n');
        }
    }

    return 0.0;
}

unsigned int get_user_input_uint(void) {
    int user_input;

    while (1) {
        if (scanf("%u", &user_input) == 1 && user_input > 0) {
            printf("user input: %u\n", user_input);
            return (unsigned int)user_input;
        } else {
            printf("invalid input, please insert a unsignal int:\n");
            while (getchar() != '\n');
        }
    }
}

void print_main_menu(void){
    printf("-------------------------------------------------\n");
    printf("-\t\tOperations\n");
    printf("-------------------------------------------------\n");
    printf("\t1. Sum\n");
    printf("\t2. Subtract\n");
    printf("\t3. Multiply\n");
    printf("\t4. Divide\n");
    printf("\t5. Determinant\n");
    //TODO: if new operation is added, add new print here and change exit number
    printf("\t6. Exit\n");
    printf("-------------------------------------------------\n");
    printf("Operations= ");
}

float* get_float_list(int n, char prefix){

    float *list = (float*)malloc(n * sizeof(float));// maloc list of floats

    if (list == NULL) { //check if list is allocated
        printf("ERROR - MEMORY ALLOCATION\n");
        //TODO: RETURN ERROR FLAG TO NOT END A PROGRAM
        exit(1);
    }

    for (int i = 0; i < n; i++) {//loop to read
        printf("Input %c%d float of array:\n", prefix, i + 1);
        list[i] = get_user_input_float();
    }

    return list; // Retorna o ponteiro para o array
}

//IHM
float* operation_get_values(unsigned int operation_choice, unsigned int* list_size){
    printf("-------------------------------------------------\n");

    switch (operation_choice){
        case SUM:
            printf(" Sum operation: a1+a2+...+aN = result\n");
            printf(" How many numbers do you want to sum?\n");
            *list_size = get_user_input_uint();
            //TODO: check if list_size > 0
            return get_float_list(*list_size, 'a');
        case SUBTRACT:
            printf(" Subtract operation: a1-a2-...-aN = result\n");
            printf(" How many numbers do you want to subtract?\n");
            *list_size = get_user_input_uint();
            //TODO: check if list_size > 0
            return get_float_list(*list_size, 'a');
        case MULTIPLY:
            printf(" Multiply operation: a1*a2*...*aN = result\n");
            printf(" How many numbers do you want to multiply?\n");
            *list_size = get_user_input_uint();
            //TODO: check if list_size > 0
            return get_float_list(*list_size, 'a');
        case DIVIDE:
            printf(" Divide operation: a1/a2/.../aN = result\n");
            printf(" How many numbers do you want to divide?\n");
            *list_size = get_user_input_uint();
            //TODO: check if list_size > 0
            return get_float_list(*list_size, 'a');
        case DETERMINANT:
            printf(" Determinant operation:\n");
            printf(" |a1   a2   ...  aX|\n");
            printf(" |aX+1 aX+2 ...  aY| = result\n");
            printf(" |aY+1 aY+2 ...  aZ|\n");
            printf(" |aZ+1 ...  ... ...|\n");
            printf(" What is the size of the matrix?\n");
            *list_size = get_user_input_uint();
            //TODO: check if list_size > 0
            return get_float_list((*list_size)*(*list_size), 'a');
        //TODO: if new operation is added, add new case here
        default:
            break;
    }
    return NULL;
}

void print_result(float result, float *list, unsigned int list_size, unsigned int operation_choice, int file_created){
    printf("-------------------------------------------------\n");

    switch (operation_choice){
        case SUM:
            printf(" Result of sum operation:\n");
            printf("%f", list[0]);
            for(int i=1; i<list_size; i++){
                printf(" + %f", list[i]);
            }
            printf(" = %f\n", result);
            break;
        case SUBTRACT:
            printf(" Result of subtract operation:\n");
            printf("%f", list[0]);
            for(int i=1; i<list_size; i++){
                printf(" - %f", list[i]);
            }
            printf(" = %f\n", result);
            break;
        case MULTIPLY:
           printf(" Result of multiply operation:\n");
            printf("%f", list[0]);
            for(int i=1; i<list_size; i++){
                printf(" * %f", list[i]);
            }
            printf(" = %f\n", result);
            break;
        case DIVIDE:
            printf(" Result of divide operation:\n");
            printf("%f", list[0]);
            for(int i=1; i<list_size; i++){
                printf(" / %f", list[i]);
            }
            printf(" = %f\n", result);
            break;
        case DETERMINANT:
            printf(" Result of matrix determinant operation:\n");
            printf(" result = %f\n", result);
            break;
        //TODO: if new operation is added, add new case here
        default:
            break;
    }

    if (!file_created){
        printf("Result cannot be saved in file \"log.txt\"\n");
    }
}
