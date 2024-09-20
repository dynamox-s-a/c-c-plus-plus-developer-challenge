#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

FILE* open_log_file(const char *filename);

void log_result(FILE *log_file, float result, float *list, unsigned int list_size, unsigned int operation_choice);

void close_log_file(FILE *log_file);

#endif // LOG_H_INCLUDED
