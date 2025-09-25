#ifndef HMI_HPP
#define HMI_HPP

#include <vector>
#include <string>

namespace HMI {

    enum class Operation {
        Exit        = 0, // Exit must be 0 for dinamic menu generation.
        SumSingle   = 1,
        SumArray    = 2,
        Divide      = 3,
        Determinant = 4,
        Transpose   = 5,
        CreateNew   = 6,
        LAST_OPERATION
    };

    inline const char* toString(Operation op) {
        switch (op) {
            case Operation::SumSingle:   return "Sum (Single value)";
            case Operation::SumArray:    return "Sum (Array)";
            case Operation::Divide:      return "Divide";
            case Operation::Determinant: return "Determinant";
            case Operation::Transpose:   return "Transpose";
            case Operation::CreateNew:   return "Create new operation";
            case Operation::Exit:        return "Exit";
            default:                     return "Unknown";
        }
    }

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

} // namespace HMI

#endif // HMI_HPP
