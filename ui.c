/**
 * @file ui.c
 * @brief Implements user interface functions for menu and input.
 *
 * Functions to read user choice, two numbers, arrays, and square matrices
 * with input validation and range checking.
 *
 * @author Sergio
 * @date 2025-08-17
 */

#include <stdio.h>
#include "ui.h"

/**
 * @brief Displays the menu and reads the user's choice.
 *
 * Menu options:
 * 1. Addition (2 integers)
 * 2. Average (2 integers)
 * 3. Mode (array input)
 * 4. Determinant (square matrix)
 * 5. Exit
 *
 * Handles invalid input and ensures choice is within range.
 *
 * @return Choice index (0-based)
 */
int get_user_choice(void) 
{
    int choice;
    
    printf("\n=== Dynamox C/C++ Developer Challenge ===\n");
    
    while(1) 
    {
        printf("\n--- MENU ---\n");
        printf("1. Addition (Enter 2 integers)\n");
        printf("2. Average (Enter 2 integers)\n");
        printf("3. Mode (Enter multiple integers, size up to %d)\n", MAX_ARRAY_SIZE);
        printf("4. Determinant (Enter square matrix, max size %d x %d)\n", MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if(scanf("%d", &choice) != 1) 
        { 
            while(getchar() != '\n'); 
            printf("Invalid input!\n"); 
            continue; 
        }
        if(choice < 1 || choice > 5) 
        { 
            printf("Choice out of range!\n"); 
            continue; 
        }
        break;
    }
    return choice - 1;
}

/**
 * @brief Reads two integer numbers from user input.
 *
 * Handles invalid input and retries until valid integers are provided.
 *
 * @param a Pointer to first integer
 * @param b Pointer to second integer
 */
void get_two_numbers_input(int *a, int *b) 
{
    while(1) 
    {
        printf("Enter first number: ");
        if(scanf("%d", a) != 1) 
        { 
            while(getchar() != '\n'); 
            printf("Invalid input!\n"); 
            continue; 
        }
        break;
    }
    while(1) 
    {
        printf("Enter second number: ");
        if(scanf("%d", b) != 1) 
        { 
            while(getchar() != '\n'); 
            printf("Invalid input!\n"); 
            continue; 
        }
        break;
    }
}

/**
 * @brief Reads an array of integers from user input.
 *
 * Prompts for array size (1..MAX_ARRAY_SIZE) and elements.
 * Handles invalid input.
 *
 * @param arr Array to store the elements
 * @param size Pointer to store the array size
 */
void get_array_input(int arr[], int *size) 
{
    int i;
    while(1) 
    {
        printf("Enter number of elements (1-%d): ", MAX_ARRAY_SIZE);
        if(scanf("%d", size) != 1) 
        { 
            while(getchar() != '\n'); 
            printf("Invalid input!\n"); 
            continue; 
        }
        if(*size < 1 || *size > MAX_ARRAY_SIZE) 
        { 
            printf("Size out of range!\n"); 
            continue; 
        }
        break;
    }
    for(i = 0; i < *size; i++) 
    {
        while(1) 
        {
            printf("Element [%d]: ", i);
            if(scanf("%d", &arr[i]) != 1) 
            { 
                while(getchar() != '\n'); 
                printf("Invalid input!\n"); 
                continue; 
            }
            break;
        }
    }
}

/**
 * @brief Reads a square matrix from user input.
 *
 * Prompts for matrix size (1..MAX_MATRIX_SIZE) and elements.
 * Handles invalid input.
 *
 * @param matrix 2D array to store matrix elements
 * @param size Pointer to store matrix size
 */
void get_matrix_input(int matrix[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], int *size) 
{
    int i, j;
    while(1) 
    {
        printf("Enter matrix size (1-%d): ", MAX_MATRIX_SIZE);
        if(scanf("%d", size) != 1) 
        { 
            while(getchar() != '\n'); 
            printf("Invalid input!\n"); 
            continue; 
        }
        if(*size < 1 || *size > MAX_MATRIX_SIZE) 
        { 
            printf("Size out of range!\n"); 
            continue; 
        }
        break;
    }
    for(i = 0; i < *size; i++) 
    {
        for(j = 0; j < *size; j++) 
        {
            while(1) 
            {
                printf("Element [%d][%d]: ", i, j);
                if(scanf("%d", &matrix[i][j]) != 1) 
                { 
                    while(getchar() != '\n'); 
                    printf("Invalid input!\n"); 
                    continue; 
                }
                break;
            }
        }
    }
}

