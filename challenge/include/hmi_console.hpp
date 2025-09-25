#ifndef HMI_CONSOLE_HPP
#define HMI_CONSOLE_HPP

#include <vector>
#include <string>
#include "hmi.hpp"

namespace HMI {

    class hmi_console : public hmi_interface {

        public:
           hmi_console();
           ~hmi_console();

            void showMenu();
            HMI::Operation getOperationChoice();
            int getInt(const std::string& prompt);
            double getDouble(const std::string& prompt);
            double getSingleInput(const std::string& prompt = "Valor: ");
            std::vector<double> getArrayInput();
            std::vector<std::vector<double>> getMatrixInput(int rows, int cols);
            std::vector<std::vector<double>> getMatrixInput(int n); //square
            std::vector<std::vector<double>> getMatrixInput();
            void showResult(double result);
            void showResult(const std::vector<std::vector<double>>& matrix);
            void showMessage(const std::string& msg);
            void showError(const std::string& msg);
    };
}
#endif // HMI_CONSOLE_HPP
