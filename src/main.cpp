#include <iostream>
#include "app_matrix.hpp"
#include "app_logger.hpp"
#include <spdlog/spdlog.h>


using namespace std;

/**
 * @brief Main function of the project
 * @details Start the modules used and application.
 */
int main() {
    // Initialization of the system. 
    app_logger_init();
    // Initialization of application
    app_matrix_init();

    return 0;
}
