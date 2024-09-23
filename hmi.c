#include <stdio.h>
#include "hmi.h"
#include "operations.h"
#include "log.h"

void show_menu() {
    printf("Selecione a operação:\n");
    printf("1 - Soma de valores\n");
    printf("2 - Determinante de uma matriz\n");
    printf("3 - Criar nova operação\n");
    printf("4 - Sair\n");
}

void handle_single_value_operation() {
    int n;
    printf("Quantos valores deseja inserir?\n");
    scanf("%d", &n);

    double values[n];
    for (int i = 0; i < n; ++i) {
        printf("Digite o valor %d: ", i + 1);
        scanf("%lf", &values[i]);
    }

    double result = calculate_sum(values, n);
    printf("Resultado: %lf\n", result);

    log_operation("Soma", result);
}

void handle_matrix_operation() {
    int size;
    printf("Digite o tamanho da matriz (ex: 2 para matriz 2x2): ");
    scanf("%d", &size);

    double matrix[size * size];
    for (int i = 0; i < size * size; ++i) {
        printf("Digite o elemento %d: ", i + 1);
        scanf("%lf", &matrix[i]);
    }

    double result = calculate_determinant(matrix, size);
    printf("Determinante: %lf\n", result);

    log_operation("Determinante", result);
}