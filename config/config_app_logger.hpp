#ifndef CONFIG_APP_LOGGER_H_INCLUDED
#define CONFIG_APP_LOGGER_H_INCLUDED

/**
 * @brief Name of log directory
 */
#define LOGGER_DIR_NAME "logs"

/**
 * @brief Path to logger file location.
 */
#define LOGGER_PATH LOGGER_DIR_NAME "/"

/**
 * @brief ID of logger started.
 * @details Used to recover the logger with sodlog::get("name")
 */
#define APP_LOGGER_ID "app_logger"

/**
 * @brief Name of application logger file.
 */
#define APP_LOGGER_FILE_NAME "matrix_operator.log"

/**
 * @brief 
 */
#define APP_LOGGER_FULL_PATH (LOGGER_PATH APP_LOGGER_FILE_NAME)





#endif // CONFIG_APP_LOGGER_H_INCLUDED