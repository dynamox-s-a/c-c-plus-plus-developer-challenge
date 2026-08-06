
#include "app_matrix.hpp"
#include "app_hmi.hpp"
#include "matrix.h"
#include "lib_matrix.h"
#include "lib_menu.h"
#include "HmiCli.h"
#include <cstdlib>
#include <thread>
#include <iostream>
#include "symbol.h"
#include "string.h"
#include <sstream>
#include "parser.h"
#include "tree_expression.h"
#include "app_progs.hpp"
#include "parser.h"
#include <spdlog/spdlog.h>
#include "config_app_logger.hpp"


/// Local definitions  -------------------------------

/** @brief Define the menu options of matrix application */
enum MatrixOptions {
    EXIT = 0,
    ADDITION = 1,
    SUBTRACTION,
    MULTIPLICATION,
    DETERMINANT,
    MAX_OPTIONS
};

enum MatrixState {
    APP_WAITING_FOR_INPUT = 0,
    APP_INVALID_INPUT,
    APP_PROCESSING,
    APP_DISPLAYING_RESULT,
    APP_FINALIZANDO
};



/// Local Variables -----------------------------------
unsigned char gucCtrMachine = 0; // Global variable to control the state of the application.
unsigned char gucCtrProcessMachine = 0;

static HmiCli gHmi;   // Create a interface 

/// Local Functions -----------------------------------

/**
 * @brief Function to request all data of each element of equation.
 * @param node Pointer to the symbol node tree with all operations and values.
 *             Values means matricies in this app.
 */
void getMatrizData(exprlib::Node* node, const int order);


/// Implementation -----------------------------------------


void app_matrix_perform_operation(const std::string expression)
{
    exprlib::NodePtr node;
    std::ostringstream oss;
    std::string order;

    // create the tree of tokens.
    node = exprlib::Parser::parseInfix(expression);
    
    exprlib::printTree(node.get(),oss);
    std::cout << oss.str() << std::endl;
    
    //Solicita os elementos das matrizes.
    order = gHmi.getUserChoice("Insert the matrix order: ");
    
    getMatrizData(node.get(), std::stoi(order)); 

    gHmi.showMatrix( (exprlib::perform_tree_expression(node.get())).getData());
} 

void app_matrix_perform_determinant(){

    double det_result;
    std::string order;

    //Solicita os elementos das matrizes.
    order = gHmi.getUserChoice("Insert the matrix order: ");

    // Request data matrix.
    Matrix dataMatrix(gHmi.getMatrix(std::stoi(order)));
    
    det_result = Lib_matrix::determinant(dataMatrix);

    gHmi.showMatrix(dataMatrix.getData());
    
    std::cout << "Determinante = " << det_result << std::endl;
}

void getMatrizData(exprlib::Node* node, const int order) {
    if (!node) return;

    std::cout << (node->sym.getName()) << "\n";

    // If value request data elements of matrix
    if(node->sym.getType() == exprlib::Symbol::Type::VALUE)
    {
        std::cout << "\n Insira os dados da Matriz: " << node->sym.getName() << std::endl;
        
        Matrix dataMatrix(gHmi.getMatrix(order));
        node->sym.setValue(dataMatrix); 
        gHmi.showMatrix(dataMatrix.getData());
    }
    // Request data to childrens 
    getMatrizData(node->left.get(),order);
    getMatrizData(node->right.get(),order);
}

bool app_matrix_insert_custom_expression(std::string name, std::string expression)
{
    auto logger = spdlog::get(APP_LOGGER_ID);   // get logger global register.

    if(name.empty() || expression.empty()) return false;

    exprlib::Parser::ValidationResult result;
    //validate expression
    result = exprlib::Parser::validateExpressionTokens(expression);

    if(!result.ok){
        gHmi.show_message(result.error,4);
        logger->error("app_matrix - %s", result.error);
        return false;
    }
    //looking for the greater index number

    progs_insert_custom_expression(name, expression);
    return true;
}

void app_matrix_init() {
    
    int optionSelected = 0;

    app_progs_init();

    app_hmi_init();

    app_hmi_control();
}

/*
void createMatrix() {

    double rows, cols, value;
    std::cout << "Enter number of rows: ";
    std::cin >> rows;
    std::cout << "Enter number of columns: ";
    std::cin >> cols;

    Matrix mat(rows, cols);

    std::cout << "Matrix created with " << mat.getRows() << " rows and " << mat.getCols() << " columns." << std::endl; 

    //inserir os valores da matriz
    for(int i = 0; i < mat.getRows(); i++) {
        for(int j = 0; j < mat.getCols(); j++) {
            std::cout << "Enter value for position (" << i << ", " << j << "): ";
            std::cin >> value;
            mat.setValue(i, j, value);
        }
    }
    
    // Exibir a matriz
    std::cout << "Matrix entered:" << std::endl;
    //printMatrix(mat);

    //calcular o determinante
    try {
        double det = mat.determinant();
        std::cout << "Determinant: " << det << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error calculating determinant: " << e.what() << std::endl;
    }

}
*/