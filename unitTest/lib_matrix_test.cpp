#include <gtest/gtest.h>
#include "matrix.h"
#include "lib_matrix.h"

/// @brief Test the sum of two second order matrices.
TEST(LibMatrixTest, sumMatrix_order2)
{
    // Test the sum of two 2x2 matrices. 
    Matrix a(2, 2);
    a.at(0, 0) = 1; a.at(0, 1) = 2;
    a.at(1, 0) = 3; a.at(1, 1) = 4;

    Matrix b(2, 2);
    b.at(0, 0) = 5; b.at(0, 1) = 6;
    b.at(1, 0) = 7; b.at(1, 1) = 8;

    Matrix result = Lib_matrix::sumMatrix(a, b);

    EXPECT_DOUBLE_EQ(result.at(0, 0), 6.0);
    EXPECT_DOUBLE_EQ(result.at(0, 1), 8.0);
    EXPECT_DOUBLE_EQ(result.at(1, 0), 10.0);
    EXPECT_DOUBLE_EQ(result.at(1, 1), 12.0);
}

/// @brief Test the difference of two second order matrices.
TEST(LibMatrixTest, subtractMatrix_order2)
{
    // Test the difference of two 2x2 matrices. 
    Matrix a(2, 2);
    a.at(0, 0) = 1; a.at(0, 1) = 2;
    a.at(1, 0) = 3; a.at(1, 1) = 4;

    Matrix b(2, 2);
    b.at(0, 0) = 5; b.at(0, 1) = 6;
    b.at(1, 0) = 7; b.at(1, 1) = 8;

    Matrix result = Lib_matrix::subtractMatrix(a, b);

    EXPECT_DOUBLE_EQ(result.at(0, 0), -4.0);
    EXPECT_DOUBLE_EQ(result.at(0, 1), -4.0);
    EXPECT_DOUBLE_EQ(result.at(1, 0), -4.0);
    EXPECT_DOUBLE_EQ(result.at(1, 1), -4.0);
}

/// @brief Test the function to check triangular feature
TEST(LibMatrixTest, triangular_matrix_check)
{
    // Check lower triangular matrix
    Matrix a(2,2);
    a.at(0, 0) = 1; a.at(0, 1) = 2;
    a.at(1, 0) = 0; a.at(1, 1) = 4;

    EXPECT_EQ(true, Lib_matrix::isTriangular(a));
}

/// @brief Verify the integrit of isNull function.
TEST(LibMatrixTest, null_matrix_false_check)
{
    Matrix a(2,2);
    a.at(0, 0) = 1; a.at(0, 1) = 0;
    a.at(1, 0) = 0; a.at(1, 1) = 0;

    EXPECT_EQ(false, Lib_matrix::isNull(a));
}

/// @brief Verify the integrit of isNull function.
TEST(LibMatrixTest, null_matrix_true_check)
{
    Matrix a(2,2);
    
    a.at(0, 0) = 0; a.at(0, 1) = 0;
    a.at(1, 0) = 0; a.at(1, 1) = 0;

    EXPECT_EQ(true, Lib_matrix::isNull(a));
}

/// @brief Verify the integrit of primaryDiagonalProduct.
///        Function used to resolve Sarrus method.
TEST(LibMatrixTest, primaryDiagonalProduct)
{
    Matrix a(4,4);

    a.at(0,0) = 2; a.at(0,1) = 1.2; a.at(0,2) = 10; a.at(0,3) =  0;
    a.at(1,0) = 1; a.at(1,1) =   4; a.at(1,2) =  1; a.at(1,3) =  3;
    a.at(2,0) = 5; a.at(2,1) =  18; a.at(2,2) =  4; a.at(2,3) = -5;
    a.at(3,0) =-2; a.at(3,1) =   2; a.at(3,2) = -1; a.at(3,3) = -3;

    EXPECT_EQ(-96, Lib_matrix::primaryDiagonalProduct(a,0,0));
    EXPECT_EQ( 300, Lib_matrix::primaryDiagonalProduct(a,0,2));
    EXPECT_EQ( 0, Lib_matrix::primaryDiagonalProduct(a,0,3));
}

/// @brief Verify the integrit of primaryDiagonalProduct.
///        Function used to resolve Sarrus method.
TEST(LibMatrixTest, secundaryDiagonalProduct)
{
    Matrix a(4,4);

    a.at(0,0) = 2; a.at(0,1) = 1.2; a.at(0,2) = 10; a.at(0,3) =  0;
    a.at(1,0) = 1; a.at(1,1) =   4; a.at(1,2) =  1; a.at(1,3) =  3;
    a.at(2,0) = 5; a.at(2,1) =  18; a.at(2,2) =  4; a.at(2,3) = -5;
    a.at(3,0) =-2; a.at(3,1) =   2; a.at(3,2) = -1; a.at(3,3) = -3;

    EXPECT_EQ( 0 , Lib_matrix::secondaryDiagonalProduct(a,3,0));
    EXPECT_EQ( 48, Lib_matrix::secondaryDiagonalProduct(a,3,1));
    EXPECT_EQ( -600, Lib_matrix::secondaryDiagonalProduct(a,3,3));
}

/// @brief Verify the integrit of Laplace Method.
TEST(LibMatrixTest, laplaceExpansionMethod_null_matrix)
{
    Matrix a(2,2);
    
    a.at(0, 0) = 0; a.at(0, 1) = 0;
    a.at(1, 0) = 0; a.at(1, 1) = 0;

    EXPECT_EQ( 0, Lib_matrix::determinantLaplaceExpansion(a));
}

/// @brief Verify the integrit of Laplace Method.
TEST(LibMatrixTest, laplaceExpansionMethod_upper_triangular_matrix)
{
    Matrix a(3,3);
    
    a.at(0, 0) = 2; a.at(0, 1) = 3; a.at(0, 2) = 4;
    a.at(1, 0) = 0; a.at(1, 1) = 4; a.at(1, 2) = 6;
    a.at(2, 0) = 0; a.at(2, 1) = 0; a.at(2, 2) = 1;

    EXPECT_EQ( 8, Lib_matrix::determinantLaplaceExpansion(a));
}

/// @brief Verify the integrit of Laplace Method.
TEST(LibMatrixTest, laplaceExpansionMethod_4_order)
{
    Matrix a(4,4);

    a.at(0,0) = 1;  a.at(0,1) = 2;  a.at(0,2) = 3; a.at(0,3) =  4;
    a.at(1,0) = 5;  a.at(1,1) = 6;  a.at(1,2) = 7; a.at(1,3) =  8;
    a.at(2,0) = 9;  a.at(2,1) = 10; a.at(2,2) =11; a.at(2,3) = 12;
    a.at(3,0) =-13; a.at(3,1) = 14; a.at(3,2) =15; a.at(3,3) = 16;

    EXPECT_EQ( 0, Lib_matrix::determinantLaplaceExpansion(a));
}

/// @brief Verify the integrit of Laplace Method.
TEST(LibMatrixTest, laplaceExpansionMethod_5_order)
{
    Matrix a(5,5);

    a.at(0,0) = 1; a.at(0,1) = 0; a.at(0,2) = 2; a.at(0,3) =-1; a.at(0,4) = 3;
    a.at(1,0) = 2; a.at(1,1) = 1; a.at(1,2) = 0; a.at(1,3) = 4; a.at(1,4) =-2;
    a.at(2,0) = 0; a.at(2,1) = 3; a.at(2,2) = 1; a.at(2,3) = 2; a.at(2,4) = 1;
    a.at(3,0) = 1; a.at(3,1) =-1; a.at(3,2) = 2; a.at(3,3) = 0; a.at(3,4) = 2;
    a.at(4,0) = 3; a.at(4,1) = 2; a.at(4,2) =-3; a.at(4,3) = 1; a.at(4,4) = 0;
    
    //EXPECT_EQ( 0, a.determinantLaplaceExpansion());

    EXPECT_EQ( 84, Lib_matrix::determinantLaplaceExpansion(a));
}