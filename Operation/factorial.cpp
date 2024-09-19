#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "factorial.h"
#include "../Tools/logger.h"

using namespace std;

/*
@brief 
function that calculates the factorial recursively
@param[in] int operative - number that will be operated
@return long long int result of the operation
*/
long long int factorial(int operative)
{
    int result = 1;
    if(operative <= 1)
    {
        return result;
    }
    else
    {
        result = operative * factorial(operative - 1);
    }
    
    logMessage("Factorial operated for " + std::to_string(operative) + "\n" + std::to_string(operative) + "! = " + std::to_string(result));
    return result;
}