#ifndef DETERMINANT_H
#define DETERMINANT_H

#include <vector>
double determinant(const std::vector<std::vector<double>>& matrix, int n);
std::vector<std::vector<double>> getSubMatrix(const std::vector<std::vector<double>>& matrix, int p, int q, int n);

#endif