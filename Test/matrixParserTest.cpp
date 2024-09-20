#include <iostream>
#include "../Tools/matrixParser.h"
#include <vector>
#include <string>

using namespace std;

bool testIsSquareAndValidSquared()
{
    vector<vector<double>> matrix = 
    {
        {1, 2, 3, 4, 5, 6},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1}
    };

    if(isSquareAndValid(matrix))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testIsSquareAndValidNotSquared()
{
    vector<vector<double>> matrix = 
    {
        {1, 2, 3, 4, 5, 6},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0}
    };

    if(isSquareAndValid(matrix))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool testParseMatrix()
{
    string matrixString = "1 2 3;4 5 6;7 8 9";
    vector<vector<double>> matrix = parseMatrix(matrixString);
    vector<vector<double>> equalMatrix = 
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    if(matrix == equalMatrix)
    {
        return true;
    }

    else
    {
        return false;
    }
}

int main()
{
    if(testIsSquareAndValidNotSquared)
    {
        cout << "testIsSquareAndValidNotSquared ok" << endl;
    }
    if(testIsSquareAndValidSquared)
    {
        cout << "testIsSquareAndValidSquared ok" << endl;
    }
    if(testParseMatrix)
    {
        cout << "testParseMatrix ok" << endl;
    }   
}