//standard library
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "operations.h"//operations header
#include "typedef.h"//menu defines

float sum(float *list, unsigned int list_size){
    float result = 0.0;
    for (int i = 0; i < list_size; i++){
        result += list[i];
    }

    return result;
}

float subtract(float *list, unsigned int list_size){
    float result = list[0];
    for (int i = 1; i < list_size; i++){
        result -= list[i];
    }
    return result;
}

float multiply(float *list, unsigned int list_size){
    float result = list[0];
    for (int i = 1; i < list_size; i++){
        result *= list[i];
    }
    return result;
}

float divide(float *list, unsigned int list_size){
    float result = list[0];
    for (int i = 1; i < list_size; i++){
        result /= list[i];
    }
    return result;
}

void get_submatrix(float *matrix, float *submatrix, unsigned int n, unsigned int excluded_row, unsigned int excluded_col) {
    unsigned int sub_i = 0;
    for (unsigned int i = 0; i < n; i++) {
        if (i == excluded_row) {
            continue; // Ignora a linha excluída
        }
        for (unsigned int j = 0; j < n; j++) {
            if (j == excluded_col) {
                continue; // Ignora a coluna excluída
            }
            submatrix[sub_i] = matrix[i * n + j]; // Preenche a submatriz
            sub_i++;
        }
    }
}


float determinant(float *matrix, unsigned int matrix_size) {
    if (matrix_size == 1) {//if matrix is 1x1
        return matrix[0];
    }

    if (matrix_size == 2) {//if matrix is 2x2
        return (matrix[0] * matrix[3]) - (matrix[1] * matrix[2]);
    }

    float det = 0.0;
    float *submatrix = (float*)malloc((matrix_size - 1) * (matrix_size - 1) * sizeof(float));//allocate memory for operation
    if (submatrix == NULL) {
        //TODO: error allocation memory
        exit(1);
    }

    for (unsigned int col = 0; col < matrix_size; col++) {
        get_submatrix(matrix, submatrix, matrix_size, 0, col);//remove line and column for cofactor

        float cofactor = matrix[col] * determinant(submatrix, matrix_size - 1);//get cofactor

        if (col % 2 == 0) {
            det += cofactor;//positive sign for pair columns
        } else {
            det -= cofactor;//negative sign for unpair columns
        }
    }

    free(submatrix);//free memory
    return det;
}

//TODO: if new operation is added, add new function here

float operation_result(float *list, unsigned int list_size, unsigned int operation_choice){

    switch (operation_choice){
        case SUM:
            //TODO: check float overflow
            return sum(list, list_size);;
        case SUBTRACT:
            //TODO: check float overflow
            return subtract(list, list_size);
        case MULTIPLY:
            //TODO: check float overflow
            return multiply(list, list_size);
        case DIVIDE:
            //TODO: check if devide by zero
            return divide(list, list_size);
        case DETERMINANT:
            return determinant(list, list_size);
        //TODO: if new operation is added, add new case here
        default:
            break;
    }
    return 0.0;
}
