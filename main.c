#include <stdio.h>
#include "calculator.h"

static int handle_sum_two_values(void) {
    double a = 0.0, b = 0.0;

    printf("Digite o primeiro valor: ");
    scanf("%lf", &a);
    printf("Digite o segundo valor: ");
    scanf("%lf", &b);

    printf("Resultado: %.6f\n", sum_two_values(a, b));
    return 1;
}

static void print_menu(void) {
    printf("\n=== Calculadora Dynamox ===\n");
    printf("1. Somar dois valores\n");
    printf("0. Sair\n");
}

int main(void) {
    int option = -1;

    while (1) {
        print_menu();
        printf("Escolha uma opcao: ");
        scanf("%d", &option);

        if (option == 0) {
            printf("Encerrando.\n");
            return 0;
        } else if (option == 1) {
            handle_sum_two_values();
        } else {
            printf("Opcao invalida.\n");
        }
    }
}