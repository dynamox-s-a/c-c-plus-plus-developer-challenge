#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "factorial.h"

using namespace std;

/*
@brief 
function that calculates the factorial recursively
@param[in] int operative - number that will be operated
@return long long int result of the operation
*/
long long int factorial(int operative)
{

    if(operative <= 1)
    {
        return 1;
    }
    else
    {
        return operative * factorial(operative - 1);
    }

}