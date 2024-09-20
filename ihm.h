#ifndef IHM_H_INCLUDED
#define IHM_H_INCLUDED

float get_user_input_float(void);

unsigned int get_user_input_uint(void);

void print_main_menu(void);

float* get_float_list(int n, char prefix);

float* operation_get_values(unsigned int operation_choice, unsigned int* list_size);

void print_result(float result, float *list, unsigned int list_size, unsigned int operation_choice, int file_created);

#endif // IHM_H_INCLUDED
