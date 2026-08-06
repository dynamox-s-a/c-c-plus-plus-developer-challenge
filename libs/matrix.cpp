
#include "matrix.h"
#include <stdexcept>

Matrix::Matrix(size_t r, size_t c) : data(r, std::vector<double>(c, 0.0)) {
    rows = r;
    cols = c;
}

Matrix::Matrix(const std::vector<std::vector<double>>& d) : data(d) {
    rows = d.size();
    cols = rows > 0 ? d[0].size() : 0;
}

size_t Matrix::getRows() const {
    return rows;
}

size_t Matrix::getCols() const {
    return cols;
}

double& Matrix::at(size_t i, size_t j) {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("Index out of range");
    }
    return data[i][j];
}

const double& Matrix::at(size_t i, size_t j) const {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("Index out of range");
    }
    return data[i][j];
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Incompatible matrix dimensions");
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Incompatible matrix dimensions");
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw std::invalid_argument("Incompatible matrix dimensions for multiplication");
    }
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < other.cols; ++j) {
            for (size_t k = 0; k < cols; ++k) {
                result.data[i][j] += data[i][k] * other.data[k][j];
            }
        }
    }
    return result;
}

// Function to print the matrix in a readable format.
std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            os << m.data[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

void Matrix::setValue(size_t row, size_t col, double value) {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Index out of range");
    }
    data[row][col] = value;
}

double Matrix::getValue(size_t row, size_t col) const {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Index out of range");
    }
    return data[row][col];
}

std::vector<std::vector<double>> Matrix::getData() const {
    return data;
}




