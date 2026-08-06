#include "app_logger.hpp"
#include "config_app_logger.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>


/// Function implementation -----------------------------

void app_logger_init(){

    std::filesystem::create_directories("logs");

    auto app_logger = spdlog::rotating_logger_mt(
        APP_LOGGER_ID,
        APP_LOGGER_FULL_PATH,
        5 * 1024 * 1024, // 5 MB
        3               // 3 arquivos
    );

    auto cpp_std = []() {
        switch (__cplusplus) {
            case 199711L: return "C++98/03";
            case 201103L: return "C++11";
            case 201402L: return "C++14";
            case 201703L: return "C++17";
            case 202002L: return "C++20";
            case 202302L: return "C++23";
            default:      return "Desconhecida";
        }
    }();

    // Padrão {fmt}
    app_logger->info("App logger initialization. C++: {} (macro __cplusplus={})", cpp_std, __cplusplus);
}


