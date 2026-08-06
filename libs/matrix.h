#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>

    class Matrix {
    private:
        std::vector<std::vector<double>> data;
        size_t rows, cols;

    public:
        Matrix(){}
        Matrix(size_t r, size_t c);
        Matrix(const std::vector<std::vector<double>>& d);

        // copy constructor
        Matrix(const Matrix&) = default; 

        ~Matrix() = default;

        size_t getRows() const;
        size_t getCols() const;

        double& at(size_t i, size_t j);
        const double& at(size_t i, size_t j) const;

        Matrix operator+(const Matrix& other) const;
        Matrix operator*(const Matrix& other) const;
        Matrix operator-(const Matrix& other) const;

        friend std::ostream& operator<<(std::ostream& os, const Matrix& m);
   
        void setValue(size_t row, size_t col, double value);
        double getValue(size_t row, size_t col) const;
        /**
         * @brief Function to get data vetor of matrix.
         * @return std::vector<std::vector<double>> data vector.
         */
        std::vector<std::vector<double>> getData() const;

        // Check if the matrix is square.
        bool isSquare() const {
            return rows == cols;
        }         
    };

#endif // MATRIX_H