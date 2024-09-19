#include "logger.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

/*
@brief 
log function
@param[in] string message - receives the message to log in the file.
*/
void logMessage(const std::string& message) 
{
    std::ofstream logFile("logfile.log", std::ios_base::app); // Abre em modo append
    if (logFile.is_open()) 
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        logFile << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S") 
                << " - " << message << std::endl;
        logFile.close();
    } 
    
    else 
    {
        std::cerr << "Não foi possível abrir o arquivo de log!" << std::endl;
    }
}
