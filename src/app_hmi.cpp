
#include "app_hmi.hpp"
#include "app_progs.hpp"
#include "lib_menu.h"
#include <nlohmann/json.hpp>
#include <vector>
#include "HmiCli.h"
#include <cstdlib>
#include <thread>
#include "symbol.h"
#include "string.h"
#include <sstream>
#include "parser.h"
#include "tree_expression.h"
#include "app_progs.hpp"
#include "app_matrix.hpp"
#include <utility>
#include <string>
#include <iostream>

using json = nlohmann::json;

/**
 * @brief Variable to store the menu informations to perform the menu system.
 */
std::vector<Menu> gMenu_list;
/**
 * @brief Variable to control the flow of menu. 
 */

uint8_t guIndex_menu = 0;

uint8_t gucHmi_ctr_machine = 0;

bool gHmi_running;

int giOption_to_process;

static HmiCli gHmi;   // Create a interface 

enum HMI_CTR_STATES {
    HMI_CTR_UPDATE = 0,
    HMI_CTR_WAIT_FOR_INPUT,
    HMI_CTR_INVALID_INPUT,
    HMI_CTR_PROCESS_REQUEST,
    HMI_CTR_FINALIZANDO
};

/// Local functions --------------------------------------------------------

/**
 * @brief Load the menu info from database to system menus.
 * @details Construct the navigation menu list of the system. 
 *          Load the informations of all menus from data base, 
 *          create the menus and organize the navigation in a 
 *          list. 
 * @return true if every think ok, false if there is some errors reading database.
 */
bool menu_load_menus();

/**
 * @brief Function to check if the characters of string is only digits.
 * @param pString String to check
 * @return true if there are only number, otherwise false.
 */
bool is_number(const std::string& pString);
/**
 * @brief Function to select the action to be execute.
 * @param piOption_id int value related to the store database register identification.
 * @return   - true if executing done, otherwise false.
 */
bool select_action(const int piOption_id);

/**
 * @brief Function to manage the input of informations to add a new 
 * expression operation.
 * @return true if the expression was add, otherwise false.
 */
bool add_custom_expression();

/// Implementation ---------------------------------------------------------


void app_hmi_init(){
    
    menu_load_menus();

    guIndex_menu = 0;
    gucHmi_ctr_machine = HMI_CTR_UPDATE;
    gHmi_running = true;
}

void app_hmi_end()
{
    gucHmi_ctr_machine = HMI_CTR_FINALIZANDO;
}

void app_hmi_update()
{
    gucHmi_ctr_machine = HMI_CTR_UPDATE;
}

bool select_action(const int piOption_id)
{
    std::string item_name, expression;
    
    item_name = app_progs_get_menu_item_name(piOption_id);
    
    if(item_name == "Exit")
    {
        app_hmi_end();
        return true;
    }
    else if(item_name == "New Expression"){
        if(add_custom_expression()){
            gHmi.show_message("New expression added.", 2);
            if(menu_load_menus()){
                app_hmi_update();
            } 
        }
        else{
            gHmi.show_message("Fail to add expression!", 2);
        }
    }
    else if(item_name == "Remove Expression"){

    }
    else if(item_name == "Determinant")
    {
        app_matrix_perform_determinant();
    }
    // Other expressions
    else
    {
        //check operation
        std::cout << "Execute operation: " << std::endl;
        expression = app_progs_get_expression(piOption_id);

        // Execute the operation.
        app_matrix_perform_operation(expression);
        
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    app_hmi_update();
    return true; //{true,HMI_CTR_UPDATE};
}

bool add_custom_expression()
{
    std::string name, expression;
    // solicitar um nome
    name = gHmi.displayQuestion("Insert the name of expression: ");

    // solicitar a expressao
    expression = gHmi.displayQuestion("Insert the custom expression: ");
    
    // validar expressao
    if(app_matrix_insert_custom_expression(name, expression))
    {
        //insert new option
        return true;
    }
    return false;
}


bool menu_load_menus()
{
    json jMenu_items = json::array();
    int number_of_menus = app_progs_get_number_of_menus();

    try{
        // get data to create each menu of the system.
        for(int i=0; i < number_of_menus; i++)
        {
            Menu menu_temp;
            // load menus of the system
            jMenu_items = app_progs_get_itens_of_menu(i);

            for(const auto& item : jMenu_items){
                // check if the item is a menu
                if(item.value("ind", -1) == 0){
                    menu_temp.setTitle(item.at("op").get<std::string>());
                } 
                else{
                    MenuItem option_menu(item.at("op").get<std::string>() ,item.value("ind", -1) );
                    option_menu.set_id(item.value("id", -1));
                    option_menu.set_id_m(item.value("id_m", -1));
                    option_menu.set_id_mx(item.value("id_mx", -1));
                    menu_temp.addItem(option_menu);
                    //menu_temp.addItem( MenuItem(item.at("op").get<std::string>() ,item.value("ind", -1) ) );
                }
            }
            // insert in menu in the menu list of the system.
            gMenu_list.push_back(menu_temp);
        }
    } 
    catch (const nlohmann::json::type_error& e) {
        //std::cout << "Tipo inválido (não é string)\n";
        // TODO log
        return false;
    }

    return true;
}

bool is_number(const std::string& pString)
{
    return !pString.empty() && std::all_of(pString.begin(), pString.end(),[](unsigned char c){ return std::isdigit(c); });
}

void app_hmi_control()
{
    std::string optionSelected;
    bool resultado;
    bool opt = false;    
    int iOpt_select;
    do{
        std::string equation;  // store the operation expression to execute.  
        
        switch(gucHmi_ctr_machine){

            case HMI_CTR_UPDATE:
                
                gHmi.displayMenu(gMenu_list[guIndex_menu]);
                gucHmi_ctr_machine = HMI_CTR_WAIT_FOR_INPUT;

                break;
            case HMI_CTR_WAIT_FOR_INPUT:

                optionSelected = gHmi.getUserChoice("Select an option: ");

                // Validate the user input.
                if(is_number(optionSelected))
                {
                    opt = false;    
                    iOpt_select = std::stoi(optionSelected);
                    
                    for(const auto& item : gMenu_list[guIndex_menu].getItems()){
                        
                        if(item.get_ind() == iOpt_select){
                            opt = true;
                            // Check if it's a link to another menu
                            if(item.get_id_m() != item.get_id_mx()){
                                // jump to another menu
                                guIndex_menu = item.get_id_mx();
                                gucHmi_ctr_machine = HMI_CTR_UPDATE;
                            }
                            else
                            {
                                gucHmi_ctr_machine = HMI_CTR_PROCESS_REQUEST;
                                // Store the index of the register in database to restore acton info.
                                giOption_to_process = item.getId();

                            // select the action to execute.
                            }
                            
                            break;  // stop search. Option finded.
                        } 
                    }
                    if(!opt){
                        gucHmi_ctr_machine = HMI_CTR_INVALID_INPUT;
                    }
                }
                else
                {
                    gucHmi_ctr_machine = HMI_CTR_INVALID_INPUT;
                }

                break;    
            case HMI_CTR_PROCESS_REQUEST:
            
                resultado = select_action(giOption_to_process);

                if(resultado)
                {
                    gHmi.show_message("Operation Executed!", 2);
                }
                else{
                    // generate log about problems.
                }
                
                break;
               
            case HMI_CTR_INVALID_INPUT:
                
                gHmi.show_message("Invalid input value.", 4);
                //std::cout << "\nInvalid input. Please try again." << std::endl;
                gucHmi_ctr_machine = HMI_CTR_UPDATE;
                break;

            case HMI_CTR_FINALIZANDO:
                //std::cout << "App Finalizando" << std::endl;
                gHmi.show_message("Finalizing app ...", 2);
                gHmi_running = false;
                break;

        }

    } while (gHmi_running);
}