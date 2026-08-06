#include "HmiCli.h"
#include "matrix.h"
#include <thread>
#include <iostream>
#include "lib_menu.h"

/*

int main() {
    int linha = 10, coluna = 20;
    std::cout << "\x1b[" << linha << ";" << coluna << "H" << "Oi aqui!";
    std::cout.flush(); // importante para aparecer imediatamente
}

#include <iostream>

int main() {
    std::cout << "\x1b[2J";      // limpa a tela
    std::cout << "\x1b[H";       // vai pro home
    std::cout << "\x1b[5;10H" << "Texto na linha 5, coluna 10";
    std::cout.flush();
}


*/    


    void HmiCli::displayMenu(const Menu& m ) {
        
        std::string line(20, '-');
        std::string header_msg;
        
        // construct the menu header
        header_msg += "--" + line + "--\n";
        header_msg += "- " + m.getTitle() + " -\n";
        header_msg += "--" + line + "--\n";

        system("clear"); // Clear the console for better readability. 
        std::cout << header_msg << std::endl;
        //std::cout << "\n"<< m.getTitle() << "\n" << std::endl;
        for (const auto& item : m.getItems()) {
            std::cout << "- " << item.get_ind() << ".  " << item.getName() << std::endl;
        }
        std::cout << "--" << line << "--\n";

    }

    std::string HmiCli::displayQuestion(std::string msg)
    {
        std::string choice;
        std::string line(20, '-');
        std::string header_msg;
        
        // construct the menu header
        header_msg += "--" + line + "--\n";
        header_msg += "- " + msg + " -\n\n";
        //system("clear"); // Clear the console for better readability. 
        std::cout << header_msg << std::endl;
        std::getline(std::cin >> std::ws, choice);
    
        return choice;
    }

    std::string HmiCli::getUserChoice(std::string msg)
    {
        std::string choice;
        std::cout << msg; 
        std::cin >> choice;
        //std::cout << "Choiced Operation: " << choice;
        return choice;
    }

    std::vector<double> HmiCli::getArrayValues() {
        int size;
        std::cout << "Enter number of values: ";
        std::cin >> size;
        std::vector<double> values(size);
        for (int i = 0; i < size; ++i) {
            std::cout << "Enter value " << (i + 1) << ": ";
            std::cin >> values[i];
        }
        return values;
    }

    std::vector<std::vector<double>> HmiCli::getMatrix(const int order){
        std::cout << std::endl;
        std::vector<std::vector<double>> matrix(order, std::vector<double>(order));
        for (int i = 0; i < order; ++i) {
            std::cout << "Enter row " << (i + 1) << ": ";
            for (int j = 0; j < order; ++j) {
                std::cin >> matrix[i][j];
            }
        }
        return matrix;
    }

    void HmiCli::showMatrix(std::vector<std::vector<double>> matrix)
    {
        size_t rows = matrix.size();
        size_t cols = matrix[0].size();

        for (int i = 0; i < rows; ++i) {
            std::cout << "| ";
            for (int j = 0; j < cols; ++j) {
                std::cout << matrix[i][j] << " ";
            }
            std::cout << " |" << std::endl;
        }
    }

    void HmiCli::show_message(std::string psMsg, int piTime_sec )
    {
        std::string line(psMsg.size(), '-');
        std::string popup_msg;
        
        popup_msg += "--" + line + "--\n";
        popup_msg += "- " + psMsg + " -\n";
        popup_msg += "--" + line + "--\n";

        std::cout << popup_msg << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(piTime_sec));
    }

    