#include "log.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#define LOG_FILE "operations.log"

namespace Log {
    void write(const std::string& message) {
        std::ofstream file(LOG_FILE, std::ios::app);
        if (!file.is_open()) return;

        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);

        file << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " << message << "\n";
        file.close();
        }
}
