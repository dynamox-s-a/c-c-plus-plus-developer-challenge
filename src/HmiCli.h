#ifndef HMI_CLI_H_INCLUDED
#define HMI_CLI_H_INCLUDED

#include "matrix.h"
#include "lib_menu.h"
#include "hmi.h"

/**
 * @brief Abstraction of a Cli interface.
 */
class HmiCli : public HMI {
public:
    /**
     * @brief Class Constructor.
     */
    HmiCli(){};
    /**
     * @brief Class Destructor.
     */
    ~HmiCli() = default;
    /**
     * @brief Method to show the system menu in the terminal.
     * @param m Reference to the menu object. 
     */
    void displayMenu(const Menu& m ) override;
    /**
     * @brief Method to show a question ang get the user value.
     * @param msg Message to show to the user.
     * @return string with the user answer.
     */
    std::string displayQuestion(std::string msg);
    /**
     * @brief Method to wait the user input choice.
     * @param msg Message to be showed to the user before wait the user answer.
     * @return String Value of user response chain captured from terminal input.
     */
    std::string getUserChoice(std::string msg);
    /**
     * @brief Method to request from the user a array of values to fill a array.
     * @return Vector of data inserted in the terminal. 
     */
    std::vector<double> getArrayValues() override;
    /**
     * @brief Method to request a matrix data from user. 
     * @return A vector of data in bidimension size.
     */
    std::vector<std::vector<double>> getMatrix(const int order) override;
    /**
     * @brief Method to show a grafic matrix to the user.
     * @param A bidimension vector with data.
     */
    void showMatrix(std::vector<std::vector<double>> matrix) override;
    /**
     * @brief Method to show a info user popup message.
     * @param psMsg string message to show to user.
     * @param piTime_sec Time that message should be showed to the user. 
     */
    void show_message(std::string psMsg, int piTime_sec);
};

#endif // HMI_CLI_H_INCLUDED

