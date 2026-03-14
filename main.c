#include <stdio.h>
#include "calculator.h"
#include "determinant.h"
#include "input_utils.h"

static void handle_sum_two_values(void) {
    double a = 0.0, b = 0.0;

    if (!read_double("Primeiro valor: ", &a) ||
        !read_double("Segundo valor: ", &b)) {
        printf("Entrada invalida.\n");
        return;
    }
    printf("Resultado: %.6f\n", sum_two_values(a, b));
}

static void handle_multiply_two_values(void) {
    double a = 0.0, b = 0.0;

    if (!read_double("Primeiro valor: ", &a) ||
        !read_double("Segundo valor: ", &b)) {
        printf("Entrada invalida.\n");
        return;
    }
    printf("Resultado: %.6f\n", multiply_two_values(a, b));
}

static void handle_sum_array(void) {
    int size = 0, i = 0;
    double values[MAX_ARRAY_SIZE];
    char prompt[32];

    if (!read_int("Tamanho do array: ", &size) || size < 1 || size > MAX_ARRAY_SIZE) {
        printf("Tamanho invalido.\n");
        return;
    }

    for (i = 0; i < size; i++) {
        snprintf(prompt, sizeof(prompt), "Valor[%d]: ", i);
        if (!read_double(prompt, &values[i])) {
            printf("Entrada invalida.\n");
            return;
        }
    }
    printf("Resultado: %.6f\n", sum_array(values, size));
}

static void handle_determinant(void) {
    int n = 0, i = 0, j = 0;
    double matrix[MAX_MATRIX_DIMENSION][MAX_MATRIX_DIMENSION];
    double det = 0.0;
    char prompt[32];

    if (!read_int("Dimensao da matriz (n): ", &n) || n < 1 || n > MAX_MATRIX_DIMENSION) {
        printf("Dimensao invalida.\n");
        return;
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            snprintf(prompt, sizeof(prompt), "A[%d][%d]: ", i, j);
            if (!read_double(prompt, &matrix[i][j])) {
                printf("Entrada invalida.\n");
                return;
            }
        }
    }

    if (calculate_determinant_gauss(n, matrix, &det) != DET_SUCCESS) {
        printf("Erro ao calcular determinante.\n");
        return;
    }

    printf("Determinante: %.6f\n", det);
}

static void print_menu(void) {
    printf("\n=== Calculadora Dynamox ===\n");
    printf("1. Somar dois valores\n");
    printf("2. Multiplicar dois valores\n");
    printf("3. Somar um array de valores\n");
    printf("4. Calcular determinante\n");
    printf("0. Sair\n");
}

int main(void) {
    int option = -1;

    while (1) {
        print_menu();

        if (!read_int("Opcao: ", &option)) {
            printf("Entrada invalida.\n");
            continue;
        }

        if (option == 0) {
            printf("Encerrando.\n");
            return 0;
        } else if (option == 1) {
            handle_sum_two_values();
        } else if (option == 2) {
            handle_multiply_two_values();
        } else if (option == 3) {
            handle_sum_array();
        } else if (option == 4) {
            handle_determinant();
        } else {
            printf("Opcao nao reconhecida.\n");
        }
    }
}