#include "../Operation/determinant.h"
#include "iostream"
#include <vector>

using namespace std;

bool testDeterminant3x3()
{
    vector<vector<double>> matrix = {
        {1, 2, 3},
        {6, 8, 11},
        {3, 0, 7}
    };

    if(determinant(matrix, matrix.size()) == -34)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testDeterminant2x2()
{
    vector<vector<double>> matrix = 
    {
        {1, 2},
        {6, 8},
    };

    if(determinant(matrix, matrix.size()) == -4)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testDeterminant6x6()
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

    if(determinant(matrix, matrix.size()) == 1)
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
    if(testDeterminant3x3())
    {
        cout << "testDeterminant3x3" << "ok" << endl;
    }
    else
    {
        cout << "testDeterminant3x3" << "nok";
    }

    if(testDeterminant2x2())
    {
        cout << "testDeterminant2x2" << "ok" << endl;
    }
    else
    {
        cout << "testDeterminant2x2" << "nok";
    }

    if(testDeterminant6x6())
    {
        cout << "testDeterminant6x6" << "ok" << endl;
    }
    else
    {
        cout << "testDeterminant6x6" << "nok";
    }

    return 0;
}