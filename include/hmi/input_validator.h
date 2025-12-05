/**
 * @file input_validator.h
 * @brief Input validation utilities
 */

#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

// Buffer size limits (embedded system constraints)
#define MAX_INPUT_LINE 256      // Maximum characters per input line
#define MAX_VALUES 100           // Maximum array elements

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read an integer from stdin with validation
 * @param prompt Message to display
 * @param min Minimum valid value
 * @param max Maximum valid value
 * @param value Pointer to store result
 * @return 0 on success, -1 on error
 */
int input_read_int(const char* prompt, int min, int max, int* value);

/**
 * @brief Read a double from stdin with validation
 * @param prompt Message to display
 * @param value Pointer to store result
 * @return 0 on success, -1 on error
 */
int input_read_double(const char* prompt, double* value);

/**
 * @brief Read array of doubles from stdin
 * @param prompt Message to display
 * @param values Buffer to store values
 * @param max_count Maximum number of values
 * @param count Pointer to store actual count read
 * @return 0 on success, -1 on error
 */
int input_read_array(const char* prompt, double* values, int max_count, int* count);

/**
 * @brief Clear input buffer (remove leftover characters)
 */
void input_clear_buffer(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_VALIDATOR_H