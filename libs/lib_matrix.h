
#ifndef LIB_MATRIX_H_INCLUDED
#define LIB_MATRIX_H_INCLUDED

#include "matrix.h"

class Lib_matrix
{
private:        
    Lib_matrix() = delete; // Prevent to instantiate the class.

public:
    /** @brief Function to calculate the sum of two matrices. 
     *          The sum only can be calculated if both matrices
     *          have the same order. 
     *  @param a First matrix.
     *  @param b Second matrix.
    */
    static Matrix sumMatrix(const Matrix& a, const Matrix& b);
    
    /** @brief Function to calculate the difference of two matrices. 
     *          The function subtract Matrix b from Matrix a. 
     *          The difference only can be calculated if both matrices
     *          have the same order. 
     *  @param a First matrix.
     *  @param b Second matrix.
     *  @return The difference matrix of a and b.
     */
    static Matrix subtractMatrix(const Matrix& a, const Matrix& b);

    /** @brief Function to calculate the product of two matrices. 
     *          The product only can be calculated if the number of columns
     *          of the first matrix is equal to the number of rows of the second matrix. 
     *  @param a First matrix.
     *  @param b Second matrix.
     *  @return The product matrix of a and b.
     */
    static Matrix multiplyMatrix(const Matrix& a, const Matrix& b);

    /**
     * @brief Function to verify if a matriz is a triangular matrix.
     * @param a Test matrix.
     * @return True if matrix is lower or upper triangular matrix.       
     */
    static bool isTriangular(const Matrix& a);

    /**
     * @brief Function to check if the matriz is a null matrix
     * @param a Matrix to test.
     * @param True if the passed matrix is a null matrix, otherwise 
     *        False. 
     */
    static bool isNull(const Matrix& a);

    /**
     * @brief Function to calcule the product of a primary diagonal from a pivot 
     *        cordinate. 
     * @param a Matrix to calculate diagonal product.
     * @param pivotRow Index of pivot line.
     * @param pivotCol Index of pivot column.
     * @return Product of diagonal elements started in pivot point.
    */
    static double primaryDiagonalProduct(const Matrix& a, size_t pivotRow, size_t pivotCol);

    /**
     * @brief Function to calcule the product of a secundary diagonal from a pivot 
     *        cordinate. 
     * @param a Matrix to calculate diagonal product.
     * @param pivotRow Index of pivot line.
     * @param pivotCol Index of pivot column.
     * @return Product of diagonal elements started in pivot point.
    */
    static double secondaryDiagonalProduct(const Matrix& a, size_t pivotRow, size_t pivotCol);

    // Determinant calculation based in Leibniz formule 
    /**
     * @brief Function to calculate the determinant of a n x n matrix 
     * @param a matrix with (n x n) order to calculate the determinat value.
     * @return Determinant value of matrix a.
     * */ 
    static double determinant(const Matrix& a); 

    /**
     * @brief Function to calculte the determinat of 3 x 3 matrix based on Sarrus Method
     * @param a Matrix with n x n order to calculate the determinat value.
     * @return Determinant value of matrix a.
     */
    static double determinantSarrusMethod(const Matrix& a); 

    /**
     * @brief Function to calculate the determinat of n x n greater than order 4. 
     *        Based in the Laplace Expansion Method.
     * @param a Matrix to calculate the determinat value.
     * @return The determinant value of matrix a.
     */
    static double determinantLaplaceExpansion(const Matrix& a);
};      


#endif // LIB_MATRIX_H_INCLUDED