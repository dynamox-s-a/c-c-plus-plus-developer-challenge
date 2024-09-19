#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "determinant.h"

using namespace std;

/*
@brief 
Function that extracts the submatrix from the main matrix as we can calculate the determinant through its cofactors.
@param[in] const vector<vector<double>>& matrix - References for the matrix that will be operate
@param[in] int p - index from the main Matrix (always zero in this application)
@param[in] int q - index from the main Matrix row
@param[in] int n - index from the main Matrix collum
@return cofactor submatrix
*/
vector<vector<double>> getSubMatrix(const vector<vector<double>>& matrix, int p, int q, int n) 
{
    vector<vector<double>> subMatrix(n - 1, vector<double>(n - 1));
    int row = 0, col = 0;

    for (int i = 0; i < n; i++) {
        if (i == p) continue;

        col = 0;
        for (int j = 0; j < n; j++) {
            if (j == q) continue;

            subMatrix[row][col] = matrix[i][j];
            col++;
        }
        row++;
    }
    return subMatrix;
}


/*
@brief 
function for the determinant calculus.
It uses a recursive reduction algorithm that reduces matrix sizes in cofactors until the whole matrix is done.
@param[in] const vector<vector<double>>& matrix - References for the matrix that will be operate
@param[in] int n - Matrix size
@return det - the value of the determinant.
*/
double determinant(const vector<vector<double>>& matrix, int n) 
{
    if (n == 1)
        return matrix[0][0];

    if (n == 2)
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

    double det = 0.0;
    int sign = 1;

    for (int i = 0; i < n; i++) {
        vector<vector<double>> subMatrix = getSubMatrix(matrix, 0, i, n);
        det += sign * matrix[0][i] * determinant(subMatrix, n - 1);
        sign = -sign;
    }

    return det;
}