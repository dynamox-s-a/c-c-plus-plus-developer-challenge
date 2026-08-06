#ifndef HMI_H_INCLUDED
#define HMI_H_INCLUDED

#include <vector>
#include <optional>
#include "lib_menu.h"

/**
 * @brief Abstract Human Machine Interface class. Concept class of base 
 *        user interaction.
 */
class HMI {
public:
    /**
     * @brief Class Constructor
     */
    HMI() = default;
    /**
     * @brief Method to show the menu in the interface.
     * @param m Object menu to show itens.
     */
    virtual void displayMenu(const Menu& m) = 0;
    /**
     * @brief Method to read a menu item choice by user.
     * @param msg String message to show before wait user answer.
     * @return string of ID option value of item choiced.
     */
    virtual std::string getUserChoice(std::string msg) = 0;
    /**
     * @brief Method to request input from the user of data array values.
     * @return std::vector<double> Vector of double values.
     */
    virtual std::vector<double> getArrayValues() = 0;
    /**
     * @brief Method to request input from the user of matrix values.
     * @param order Order of matrix
     * @return std::vector<std::vector<double>> two-dimensional vector of matrix values.
     */
    virtual std::vector<std::vector<double>> getMatrix(const int order) = 0;
    /**
     * @brief Method to show a grafic matrix with its values.
     * @param matrix A two-dimensional vector of matrix values.
     */
    virtual void showMatrix(std::vector<std::vector<double>> matrix) = 0;
    /**
     * @brief Class destructor */  
    virtual ~HMI() = default;
};



#endif // HMI_H_INCLUDED