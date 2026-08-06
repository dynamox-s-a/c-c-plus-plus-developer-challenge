#ifndef DRV_DATA_BASE_H_INCLUDED
#define DRV_DATA_BASE_H_INCLUDED

#include <fstream>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Function to create the base of data base and
 * load the initial data.
 */
void drv_data_base_init(const std::string& pdata_base_pat, json& data_base);

bool drv_data_base_exists(const std::string& pdata_base_path);

json drv_load_data_base(const std::string& path);

void drv_save_data_base(const std::string& path, const json& j);

#endif // DRV_DATA_BASE_H_INCLUDED