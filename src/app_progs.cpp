
#include "app_progs.hpp"
#include "drv_data_base.hpp"
#include "app_progs.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "config_app.hpp"
#include "config_app_logger.hpp"
#include "lib_menu.h"

using json = nlohmann::json;

/// global variable ----------------------------

/// @brief Variable to store aplication data.
json gData;

/// local functions ----------------------------

void insert_defaul_data(json &pdata_base);

// 
void insert_defaul_menu(json &pdata_base);

/**
 * @brief Function to looking for the least
 * index operation. 
 * @return The value of least index operation.
 */
int progs_get_last_operation_index();

/**
 * @brief Get number of menus
 * @details Read local data to identify the number of menus in the system
 * @return Number of menus.
 */
int get_number_of_menus();

/**
 * @brief Get the last_register index
 * @return Value of last index.
 */
int progs_get_last_register();

/// Function Implementations ------------------- 

void app_progs_init(void)
{
    auto logger = spdlog::get(APP_LOGGER_ID);   // get logger global register.
    logger->info("app_progs - Progs Init");

    //Create a local data base object new data configuration base
    gData = json::object();

    std::string arq = DATA_PATH;
    //check if data base has some data.
    if(!drv_data_base_exists(arq)){
        
        //std::filesystem:
        drv_data_base_init(arq, gData);
        logger->info("app_progs - Data Base not found! Data Base Init ..");
    }
    
    try{
        // Loacheck if there is some data.
        gData = drv_load_data_base(arq);
        if(gData.empty()){
            logger->info("app_progs - Data Base empty! Insert basic functions.");
            insert_defaul_data(gData);
            drv_save_data_base(DATA_PATH, gData);
        }     
    }
    catch (const json::parse_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        // Todo enviar para o log
    }
}

void insert_defaul_data(json& pdata_base){
 
    // Another data base.

    // Store the basic menu itens.
    insert_defaul_menu(pdata_base);
}

int app_progs_get_number_of_menus()
{
    int num_menus = 0;
    // looking for the greatest menu ID
    for(const auto& item : gData){
        if(int v = item.value("id_m", -1) > num_menus) num_menus = v;
    }
    return (++num_menus);
}

json app_progs_get_itens_of_menu(const int pmenu_inded)
{
    json list = json::array(); 

    for(const auto& item : gData){
        if(item.value("id_m", -1) == pmenu_inded){
            list.push_back(item);
        }
    }
    return list;
}

std::string app_progs_get_menu_item_name(int piId_register){

    try{
        for(const auto& item : gData){
            if(item.value("id", -1) == piId_register){
                return item.at("op").get<std::string>();
            }
        }
    }catch (const json::parse_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        // Todo enviar para o log
    }
    return "";
}

std::string app_progs_get_expression(int piId_register){
    try{
        for(const auto& item : gData){
            if(item.value("id", -1) == piId_register){
                return item.at("expression").get<std::string>();
            }
        }
    }catch (const json::parse_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        // Todo enviar para o log
    }
    return "";
}

int progs_get_last_operation_index(){
    int great_ind = 0;
    try{
        for(const auto& item : gData){
            // looking for the great operator index.
            if(item.value("id_m", -1) == 1){
                if(item.value("ind",-1) > great_ind)
                {
                    // update the great index.
                    great_ind = item.value("ind",-1);
                }
            }
        }
        return great_ind;
    }catch (const json::parse_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        // Todo enviar para o log
    }
    return -1;
}

int progs_get_last_register()
{
    try{
        json ultimo = gData.back();
        return ultimo.value("id",-1);

    }catch (const json::parse_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        // Todo enviar para o log
    }
    return -1;
}

void progs_insert_custom_expression(const std::string& name, const std::string& expression)
{
    int last_operator_index;
    int register_id;
    //check the base of menu to identify the local of menu
    last_operator_index = progs_get_last_operation_index();
    register_id = progs_get_last_register();

    if((last_operator_index != -1) && (register_id != -1)){

        register_id++;
        last_operator_index++;
        gData.push_back({{"id", register_id},{"id_m", 1},{"id_mx", 1},{"ind", last_operator_index},{"op", name},{"expression", expression}});
    }
    drv_save_data_base(DATA_PATH, gData);
}

/**
 * @def "id"   - Database column of the Primary key of a data base.
 * @def "id_m" - Database column of the reference of menu association.
 * @def "id_mx"- Database column of the menu reference of next menu association. 
 *               id_mx = 0 means remain in the same menu, otherwise jump to id_mx menu.
 * @def "ind"  - Database column of the menu item option index. idx=0 means th title of menu.
 * @def "op"   - Database Column of string operation name.
 * @def "exp"  - Database Column of string math expression. 
 */
void insert_defaul_menu(json& pdata_base){

    // Insert the basic Menu Itens.
    // Main menu ind = 0 - Title
    pdata_base.push_back({{"id", 0},{"id_m", 0},{"id_mx", 0},{"ind", 0},{"op", "Matrix Menu"},        {"exp", " "}});
    // Insert itens
    pdata_base.push_back({{"id", 1},{"id_m", 0},{"id_mx", 1},{"ind", 1},{"op", "Operations"},         {"exp", " "}});
    pdata_base.push_back({{"id", 2},{"id_m", 0},{"id_mx", 0},{"ind", 2},{"op", "New Expression"},     {"exp", " "}});
    pdata_base.push_back({{"id", 3},{"id_m", 0},{"id_mx", 0},{"ind", 3},{"op", "Remove Expression"},  {"exp", " "}});
    pdata_base.push_back({{"id", 4},{"id_m", 0},{"id_mx", 0},{"ind", 4},{"op", "Exit"},               {"exp", " "}});
    // Operation Menu
    // Main menu ind = 0 - Title
    pdata_base.push_back({{"id", 5},{"id_m", 1},{"id_mx", 0},{"ind", 0},{"op", "Operations Menu"}, {"exp", " "}});
    // Insert itens
    pdata_base.push_back({{"id", 6},{"id_m", 1},{"id_mx", 0},{"ind", 1},{"op", "Back"},           {"expression", " "}});
    pdata_base.push_back({{"id", 7},{"id_m", 1},{"id_mx", 1},{"ind", 2},{"op", "Addition"},       {"expression", "A + B"}});
    pdata_base.push_back({{"id", 8},{"id_m", 1},{"id_mx", 1},{"ind", 3},{"op", "Subtraction"},    {"expression", "A - B"}});
    pdata_base.push_back({{"id", 9},{"id_m", 1},{"id_mx", 1},{"ind", 4},{"op", "Multiplication"}, {"expression", "A * B"}});
    pdata_base.push_back({{"id",10},{"id_m", 1},{"id_mx", 1},{"ind", 5},{"op", "Determinant"},    {"expression", "det A"}});
}