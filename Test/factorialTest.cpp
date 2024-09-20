#include <iostream>
#include "../Operation/factorial.h"

using namespace std;

bool testFactorialSmallNumber()
{
    int number = 4;
    if(factorial(4) == 24)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testFactorialBigNumber()
{
    int number = 10;
    if(factorial(10) == 3628800)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testFactorialZero()
{
    int number = 0;
    if(factorial(0) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testFactorialOne()
{
    int number = 1;
    if(factorial(1) == 1)
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
    if(testFactorialBigNumber())
    {
        cout << "testFactorialBigNumber" << "ok" << endl;
    }
    else
    {
        cout << "testFactorialBigNumber" << "nok";
    }

    if(testFactorialOne())
    {
        cout << "testFactorialOne" << "ok" << endl;
    }
    else
    {
        cout << "testFactorialOne" << "nok";
    }

    if(testFactorialSmallNumber())
    {
        cout << "testFactorialSmallNumber" << "ok" << endl;
    }
    else
    {
        cout << "testFactorialSmallNumber" << "nok";
    }

    if(testFactorialZero())
    {
        cout << "testFactorialZero" << "ok" << endl;
    }
    else
    {
        cout << "testFactorialZero" << "nok";
    }
}