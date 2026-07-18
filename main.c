/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include "hmi.h"

int main() {
    int option = 0;
    do {
        show_menu();
        printf("Escolha uma opção: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                handle_single_value_operation();
                break;
            case 2:
                handle_matrix_operation();
                break;
            case 3:
                printf("Funcionalidade de criação de operação futura\n");
                break;
            case 4:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (option != 4);

    return 0;
}