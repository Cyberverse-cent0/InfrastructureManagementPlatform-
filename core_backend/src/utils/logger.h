#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Log levels
typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} log_level_t;

// Logger structure
typedef struct {
    FILE* log_file;
    char log_file_path[512];
    char log_dir[512];
    pthread_mutex_t log_mutex;
    int is_initialized;
    size_t buffer_size;
    char* buffer;
} installation_logger_t;

// Function declarations
installation_logger_t* installation_logger_init(const char* working_dir, const char* log_file_name, const char* log_dir);
void installation_logger_destroy(installation_logger_t* logger);
void log_message(installation_logger_t* logger, log_level_t level, const char* message);
void log_message_simple(log_level_t level, const char* message);

#endif /* LOGGER_H */