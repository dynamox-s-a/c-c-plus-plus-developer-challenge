/**
 * @file    arithmetic_determinant_n.c
 * @brief   Implementation of the determinant calculation command for the CLI calculator.
 *
 * @details
 * This module implements the "Determinant_N" command for the CLI calculator, allowing users to compute
 * the determinant of a square matrix provided as a flat vector. It parses input tokens, checks matrix validity,
 * performs the determinant calculation recursively, and prints the result to the CLI output stream.
 *
 * @author  Emerson Isaias da Silva
 * @date    21-09-2025
 */

/*
 *   INCLUDES
 */
#include "arithmetic_determinant_n.h"

#include "cli_handler.h"
#include "misc.h"

#include "math.h"

/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */
/**
 * @brief Recursively calculates the determinant of a square matrix.
 *
 * @param result Pointer to store the determinant result.
 * @param matrix Pointer to the matrix data (flattened, row-major order).
 * @param n      Size of the matrix (n x n).
 * @return       CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e determinant(double* result, double *matrix, uint8_t n);

/*
 *   CONSTANTS
 */
/**
 * @brief CLI command constant for the "Determinant_N" operation.
 *
 * @details
 *  - name:        Command name ("Determinant_N")
 *  - description: Command description and usage syntax
 *  - func_ptr:    Pointer to the function that implements the command
 */
const cli_handler_commands_t Arithmetic_Determinant_N_Cte = {
        .name = "Determinant_N",
        .description = "Determinant N the value to ANS - Sintax \"Determinant [double, double, ...]\"\n",
        .func_ptr = Arithmetic_Determinant_N
};

 /**
 * @brief Handles the "Determinant_N" CLI command.
 *
 * @details
 * Parses the input stream for matrix values, checks if the matrix is square,
 * and computes its determinant. Prints the result or an error message to the CLI output.
 *
 * @param cli_handler Pointer to the CLI handler structure.
 * @param saveptr     Pointer to the input stream to parse.
 * @return            CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e Arithmetic_Determinant_N(cli_handler_t* cli_handler, char* saveptr){

    double size;
    misc_vector_t* vector;

    CHECK_CLI_HANDLER_PTR(cli_handler);
    CHECK_CLI_HANDLER_PTR(saveptr);

    Misc_Get_Vector(vector, saveptr);

    if(vector != NULL){

        size = sqrt((double)vector->size); /* Calculate the size of the matrix */

        if(fmod(size, 1.0) != 0){

            Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
            return CLI_HANDLER_ERROR_PARM; /* Return error */
        }

        determinant(&cli_handler->ans, vector->data, (uint8_t)size); /* Calculate determinant */
        Cli_Handler_Out_Msg(cli_handler, "Determinant = %.*f\n", cli_handler->config.decimal, cli_handler->ans); /* Print result */
    }
    else {

        Cli_Handler_Out_Msg(cli_handler, "Error sintaxe or allocation error\n"); /* Freeing resources */
    }

    return CLI_HANDLER_ERROR_OK; /* Return OK */
}

/**
 * @brief Recursively calculates the determinant of a square matrix.
 *
 * @details
 * Uses Laplace expansion to compute the determinant for matrices larger than 2x2.
 * Allocates submatrices dynamically for recursion. Handles base cases for 1x1 and 2x2 matrices.
 *
 * @param result Pointer to store the determinant result.
 * @param matrix Pointer to the matrix data (flattened, row-major order).
 * @param n      Size of the matrix (n x n).
 * @return       CLI_HANDLER_ERROR_OK on success, error code otherwise.
 */
cli_handler_error_e determinant(double* result, double *matrix, uint8_t n) {

    double value = 0;
    double det = 0;
    double sign;

    int subSize;
    int subIndex;

    double *submatrix;
    double sub_result;

    CHECK_CLI_HANDLER_PTR(result);
    CHECK_CLI_HANDLER_PTR(matrix);

    /* Base cases */
    if (n == 1){
        value = matrix[0]; /* Determinant of 1x1 */
        *result = value;
        return CLI_HANDLER_ERROR_OK;
    }
    if (n == 2) {
        value = matrix[0]*matrix[3] - matrix[1]*matrix[2]; /* Determinant of 2x2 */
        *result = value;
        return CLI_HANDLER_ERROR_OK;
    }

    for (int p = 0; p < n; p++) { /* Loop over columns */

        subSize = (n-1)*(n-1);  /* Size of the submatrix */
        submatrix = malloc(subSize * sizeof(double)); /* Allocate memory for the submatrix */

        CHECK_CLI_HANDLER_PTR(submatrix);

        subIndex = 0;

        for (int i = 1; i < n; i++) {

            for (int j = 0; j < n; j++) {   /* Loop over columns */

                if (j == p) continue;   /* Skip column p */
                submatrix[subIndex++] = matrix[i*n + j];    /* Fill the submatrix */
            }
        }

        sign = (p % 2 == 0) ? 1 : -1; /* Determine the sign */

        determinant(&sub_result, submatrix, n-1); /* Recursive call */
        det += sign * matrix[p] * sub_result; /* Update determinant */
        free(submatrix);    /* Free the submatrix memory */
    }

    *result = det;

    return CLI_HANDLER_ERROR_OK;
}