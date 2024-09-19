/*
* Main execution flow file
* Here the menus HMI is show and the math operation functions are called
*/
#include <iostream>
#include <vector>
#include "../Tools/matrixParser.h"
#include "../Tools/logger.h"
#include "determinant.h"
#include "factorial.h"
#include "operation.h"

using namespace std;

/*
@brief
Void function with no param that calls for determinant() in order to show the result of the operation.
It verifies for a valid squared matrix.
*/
void calculateDeterminant()
{
    string matrixString;
    cout << "Enter with the squared matrix" << endl;
    cin.ignore();
    getline(cin, matrixString);

    vector<vector<double>> matrix = parseMatrix(matrixString);
    if(isSquareAndValid(matrix))
    {
        cout << "Matrix: " << endl << matrixToString(matrix) << endl;
        cout << "This matrix determinant is " << determinant(matrix, matrix.size()) << endl;
    }
    else
    {
        cout << "Input is not correct, a valid squared matrix is necessary... try again" << endl;
    }
}

/*
@brief 
Void function with no param that calls for factorial() in order to show the result of the operation.
It verifies for a valid integer number bellow 20 (long long limits exceeds for 21!).
*/
void calculateFactorial()
{
    int operative;
    bool validInput = false;
    cout << "Enter with a whole number: ";
    
    while(!validInput)
    {
        cin >> operative;
        if(cin.fail() || operative < 0 || operative > 20)
        {
            cout << "Invalid input, it needs to be a positive integer number bellow 20. Try again:" << endl;
            cin.clear();
            cin.ignore();
        }
        else
        {
            validInput = true;
        }
    }

    cout << operative << "! = " <<  factorial(operative) << endl;
}

/*
@brief 
main menu for the HMI
*/
void showMainMenu()
{
    int option = 1;

    do
    {   logMessage("Main menu printed");
        cout << "\n=== Main Menu ===" << endl;
        cout << "1. Calculate determinant" << endl;
        cout << "2. Calculate the factorial of a number" << endl;
        cout << "0. Exit" << endl;
        cout << "Pick an operation to proceed: ";

        cin.clear();
        cin >> option;

        switch(option)
        {
            case 0:
                cout << "Exiting..." << endl;
                break;
            case 1:
                calculateDeterminant();
                break;
            case 2:
                calculateFactorial();
                break;
            default:
                cout << "Press enter to go back to the main menu: " << endl;
                cin.clear(); // Limpa o erro
                cin.ignore(); // Descartar a entrada inválida
                break;
        }
        
    } while (option != 0);
}