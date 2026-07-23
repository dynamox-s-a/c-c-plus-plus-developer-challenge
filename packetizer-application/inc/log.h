#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

#define LOG_VERBOSITY_LEVEL LOG_LEVEL_DEBUG

#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_DEBUG 2

#if (LOG_VERBOSITY_LEVEL >= LOG_LEVEL_DEBUG)
#define LOG_DEBUG(...) do{printf("DEBUG:"); printf(__VA_ARGS__);printf("\n\r");}while(0)
#else
#define LOG_DEBUG(...)
#endif

#if (LOG_VERBOSITY_LEVEL >= LOG_LEVEL_ERROR)
#define LOG_ERROR(...) do{printf("ERROR:"); printf(__VA_ARGS__);printf("\n\r");}while(0)
#else
#define LOG_ERROR(...)
#endif

#if (LOG_VERBOSITY_LEVEL >= LOG_LEVEL_INFO)
#define LOG_INFO(...) do{printf("INFO:"); printf(__VA_ARGS__);printf("\n\r");}while(0)
#else
#define LOG_INFO(...)
#endif

#endif