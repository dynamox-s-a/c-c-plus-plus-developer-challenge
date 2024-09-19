#include <sstream>
#include <vector>
#include "matrixParser.h"
#include "logger.h"

using namespace std;

/*
@brief 
Function that parses the user's entry to be processed afterwards.
@param[in] string& input  - Reference for a string in the right pattern to be transformed into a valid matrix.
@return const vector<vector<double>>& matrix, a valid matrix to be processed.
*/
vector<vector<double>> parseMatrix(string& input)
{
    vector<vector<double>> matrix;
    stringstream ss(input);
    string row;

    while (getline(ss, row, ';')) {
        vector<double> rowValues;
        stringstream rowStream(row);
        double value;

        while (rowStream >> value) {
            rowValues.push_back(value);
        }

        matrix.push_back(rowValues);
    }

    logMessage(std::string("Parsed Matrix: \n") + matrixToString(matrix));

    return matrix;
}
/*
@brief 
Simple to_string function
@param[in] const vector<vector<double>>& matrix - References for the matrix that will be parsed into a readable string.
@return a readable string for the user to see.
*/
string matrixToString(vector<vector<double>>& matrix) 
{
    stringstream ss;
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            ss << elem << " ";
        }
        ss << "\n";
    }
    return ss.str();
}

/*
@brief 
Function that verifies if the entrance is a valid all numbered squared Matrix.
@param[in] const vector<vector<double>>& matrix - References for the matrix that will be verified.
@return true for a squared valid matrix, false for the oposite.
*/
bool isSquareAndValid(vector<vector<double>>& matrix)
{
    int n = matrix.size();
    for (const auto& row : matrix) 
    {
        if (row.size() != n) 
        {
            logMessage(std::string("Matrix not valid inputed"));
            return false;
        }
    }

    logMessage(std::string("Valid matrix inputed...  \n") + matrixToString(matrix));
    return true;
}

