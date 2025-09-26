/**
 * @file    arithmetic_determinant_n.c
 * @brief   TODO
 *
 * @details TODO
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

/*
 *   MACROS
 */

/*
 *   EXTERNAL VARIABLES
 */

/*
 *   LOCAL FUNCTIONS PROTOTYPE
 */

/*
 *   CONSTANTS
 */
const cli_handler_commands_t Arithmetic_Determinant_N_Cte = {
        .name = "Determinant_N",
        .description = "Determinant N the value to ANS - Sintax \"Determinant [0x00000000]\"\n",
        .func_ptr = Arithmetic_Determinant_N
};

/**
 * @brief TODO
 * 
 * @param cli_handler Pointer of handler CLI
 * @param token Pointer of token receive
 * @return TODO
 */
cli_handler_error_e Arithmetic_Determinant_N(cli_handler_t* cli_handler, char* saveptr){
    
    printf("command_determinant_n");
    return CLI_HANDLER_ERROR_OK; /* Return OK */
}
/*
    Based in book: 
    "Numerical Analysis 9th Edition"
    by Richard L. Burden, J. Douglas Faires 
    
    Step 1:
    - Select l11 and u11 satisfying l11u11 = a11 .
    - If l11u11 = 0 then OUTPUT (‘Factorization impossible’);

    Step 2:
    - For j = 2, . . . , n set u1j = a1j/l11 ; (First row of U.)
    - lj1 = aj1/u11 . (First column of L.)

    Step 3:

*/

/* Validação pré cálculo
    a matrix tem que ser nxn
*/


/*
    Verificação caso trivial
    Se tiver 2 linhas iguais é 0
    Se tiver uma linha toda 0 é 0
    Se tiver uma coluna toda 0 é 0
    Se tiver uma linha ou coluna com multiplos de outra é 0
    Se tiver uma linha ou coluna com soma de outras é 0
*/

/*
    - Eliminação de Gauss
    - Triangulação do vetor utilizando 
    - Cálculo do determinante pelos cofatores
*/