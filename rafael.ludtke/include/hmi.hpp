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

    class hmi_interface {
    public:
        virtual ~hmi_interface() = default;

        virtual void showMenu() = 0;
        virtual HMI::Operation getOperationChoice() = 0;
        virtual int getInt(const std::string& prompt) = 0;
        virtual double getDouble(const std::string& prompt) = 0;
        virtual double getSingleInput(const std::string& prompt) = 0;
        virtual std::vector<double> getArrayInput() = 0;
        virtual std::vector<std::vector<double>> getMatrixInput(int rows, int cols) = 0;
        virtual std::vector<std::vector<double>> getMatrixInput(int n) = 0; //square
        virtual std::vector<std::vector<double>> getMatrixInput() = 0;
        virtual void showResult(double result) = 0;
        virtual void showResult(const std::vector<std::vector<double>>& matrix) = 0;
        virtual void showMessage(const std::string& msg) = 0;
        virtual void showError(const std::string& msg) = 0;
    };

} // namespace HMI

#endif // HMI_HPP
