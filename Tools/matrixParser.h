#ifndef MATRIX_PARSER_H
#define MATRIX_PARSER_H

#include <vector>
#include <string>

std::vector<std::vector<double>> parseMatrix(std::string& input);

std::string matrixToString(std::vector<std::vector<double>>& matrix);
bool isSquareAndValid(std::vector<std::vector<double>>& matrix);

#endif