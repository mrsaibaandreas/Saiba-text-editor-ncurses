#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
typedef enum {
    #ifdef DEBUG_A
    DEBUG,
    #endif
    INFO,
    WARNING,
    ERROR
    
} log_level;

FILE *LOG_FILE;
FILE *REAL;
static inline void INIT_LOG(char *filename) {
    time_t current_time = time(NULL); 
    char time_string[21];
    if (strftime(time_string, sizeof(time_string), "%FT%TZ", gmtime(&current_time)) == 0) {
        perror("strftime failed");
        abort();
    }
    
    char log_file_time_stamped[64];
    snprintf(log_file_time_stamped,  sizeof(log_file_time_stamped), "%s%s", filename, time_string); ;
    LOG_FILE = fopen(log_file_time_stamped, "w+"); 
    REAL = fopen("test", "w+");
    if (LOG_FILE == NULL) {
        perror("Error opening file");
        abort(); 
    }
}
#ifdef DEBUG_A
    #define LOG_D(fmt, ...) log_msg(DEBUG, fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define LOG_D(...)
#endif

#define LOG_I(level, fmt, ...) log_msg(level, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

static inline void log_msg(log_level level, const char *fmt, const char *file, int line, ...) {
    
    switch(level) {
        #ifdef DEBUG_A
        case DEBUG:
            fprintf(LOG_FILE, "[DEBUG] [%s (%i)] ", file, line);
            fprintf(REAL, "[DEBUG] [%s (%i)] ", file, line);
            break;
        #endif
        case INFO:
            fprintf(LOG_FILE, "[INFO] [%s (%i)] ", file, line);
            break;
        case WARNING:
            fprintf(LOG_FILE, "[WARNING] [%s (%i)] ", file, line);
            break;
        case ERROR:
            fprintf(LOG_FILE, "[ERROR] [%s (%i)] ", file, line);
            break;
        default: 
            fprintf(LOG_FILE, "Unknown log level, not able to print\n");
    }
    
    va_list args;
    va_start(args, line);
    vfprintf(LOG_FILE, fmt, args);
    vfprintf(REAL, fmt, args);
    va_end(args);
    fprintf(LOG_FILE, "\n");    
    fprintf(REAL, "\n");    
}
