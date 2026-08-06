#ifndef APP_HMI_H_INCLUDED
#define APP_HMI_H_INCLUDED


/**
 * @brief Function to initialize de menu data.
 * @details Read the datbase, identify all menus and menu itens and
 * construct the array of menus. 
 */
void app_hmi_init();

/**
 * @brief Function to control the hmi interface.
 * @details This application control the menu flow and the
 * execution of menu choices selected by user. 
 */
void app_hmi_control();

#endif // APP_HMI_H_INCLUDED