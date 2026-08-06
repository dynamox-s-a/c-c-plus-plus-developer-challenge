
#include "drv_data_base.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "config_app_logger.hpp"

using json = nlohmann::json;

/// global variable ----------------------------



void drv_data_base_init(const std::string& pdata_base_path, json& data_base)
{
    auto logger = spdlog::get(APP_LOGGER_ID);   // get logger global register.

    logger->info("drv_data_base - data init");
    
    std::filesystem::path p(pdata_base_path);
    // Criate the directory if it isn't exist.
    std::filesystem::create_directories(p.parent_path());

    if (!(std::filesystem::exists(p))){
        
        // Open the stream to create a empty file to write.
        std::ofstream out(pdata_base_path,std::ios::out);
        json j = json::array();     // create a empty json.
        out << std::setw(2) << j << "\n";         
        out.close();
        
        logger->info("drv_data_base - criate data base");
    }
/*
    if (!(std::filesystem::exists(p.filename()) && 
        std::filesystem::is_regular_file(p.filename()))) {
            logger->info("drv_data_base - criate data base");
            std::ofstream out(pdata_base_path, std::ios::out); 
            //out.close();
    }
*/
}




bool drv_data_base_exists(const std::string& pdata_base_path)
{
    std::filesystem::path p(pdata_base_path);
 
    return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
}
// Faz a leitura dos dados para a base local.
json drv_load_data_base(const std::string& path) {
    
    // to create fstream insert only the file name, otherwise error.
    std::ifstream in(path);
    
    if (!in.is_open()) return json::array();     // se não existe, começa vazio
    json j;

    in >> j;                                     // parse do arquivo
    if (!j.is_array()) return json::array();     // garantia: array

    in.close();
    return j;
}

void drv_create_database(const std::string& path)
{
    
}

void drv_save_data_base(const std::string& path, const json& j) {
    
    // Open the stream to write.
    std::ofstream out(path,std::ios::out);
    if(out.is_open()){
        out << std::setw(2) << j << "\n";         
        out.close();
    }

    
}

