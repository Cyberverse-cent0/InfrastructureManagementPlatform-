#include "logger.h"
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir _mkdir
#else
#include <unistd.h>
#endif

// Global logger instance for simple logging
static installation_logger_t* global_logger = NULL;

// Initialize logger
installation_logger_t* installation_logger_init(const char* working_dir, const char* log_file_name, const char* log_dir) {
    installation_logger_t* logger = (installation_logger_t*)malloc(sizeof(installation_logger_t));
    if (logger == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for logger\n");
        return NULL;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&logger->log_mutex, NULL) != 0) {
        fprintf(stderr, "Error: Failed to initialize mutex\n");
        free(logger);
        return NULL;
    }
    
    // Set default values
    logger->buffer_size = 4096;
    logger->buffer = (char*)malloc(logger->buffer_size);
    if (logger->buffer == NULL) {
        fprintf(stderr, "Error: Failed to allocate buffer for logger\n");
        pthread_mutex_destroy(&logger->log_mutex);
        free(logger);
        return NULL;
    }
    
    // Set log directory path
    if (log_dir != NULL && working_dir != NULL) {
        size_t dir_len = strlen(working_dir) + strlen(log_dir) + 2;
        if (dir_len < sizeof(logger->log_dir)) {
            snprintf(logger->log_dir, sizeof(logger->log_dir), "%s/%s", working_dir, log_dir);
        } else {
            strncpy(logger->log_dir, log_dir, sizeof(logger->log_dir) - 1);
            logger->log_dir[sizeof(logger->log_dir) - 1] = '\0';
        }
    } else if (log_dir != NULL) {
        strncpy(logger->log_dir, log_dir, sizeof(logger->log_dir) - 1);
        logger->log_dir[sizeof(logger->log_dir) - 1] = '\0';
    } else {
        strncpy(logger->log_dir, "logs", sizeof(logger->log_dir) - 1);
        logger->log_dir[sizeof(logger->log_dir) - 1] = '\0';
    }
    
    // Create log directory if it doesn't exist
#ifdef _WIN32
    _mkdir(logger->log_dir);
#else
    mkdir(logger->log_dir, 0755);
#endif
    
    // Set log file path
    if (log_file_name != NULL) {
        size_t path_len = strlen(logger->log_dir) + strlen(log_file_name) + 2;
        if (path_len < sizeof(logger->log_file_path)) {
            snprintf(logger->log_file_path, sizeof(logger->log_file_path), "%s/%s", logger->log_dir, log_file_name);
        } else {
            strncpy(logger->log_file_path, log_file_name, sizeof(logger->log_file_path) - 1);
            logger->log_file_path[sizeof(logger->log_file_path) - 1] = '\0';
        }
    } else {
        size_t path_len = strlen(logger->log_dir) + 9; // +9 for "/log.txt"
        if (path_len < sizeof(logger->log_file_path)) {
            snprintf(logger->log_file_path, sizeof(logger->log_file_path), "%s/log.txt", logger->log_dir);
        } else {
            strncpy(logger->log_file_path, "log.txt", sizeof(logger->log_file_path) - 1);
            logger->log_file_path[sizeof(logger->log_file_path) - 1] = '\0';
        }
    }
    
    // Open log file
    logger->log_file = fopen(logger->log_file_path, "a");
    if (logger->log_file == NULL) {
        fprintf(stderr, "Error: Could not open log file %s\n", logger->log_file_path);
        pthread_mutex_destroy(&logger->log_mutex);
        free(logger->buffer);
        free(logger);
        return NULL;
    }
    
    logger->is_initialized = 1;
    
    // Log initialization
    time_t time_now = time(NULL);
    fprintf(logger->log_file, "%s Logger initialized\n", asctime(localtime(&time_now)));
    fflush(logger->log_file);
    
    // Set as global logger
    global_logger = logger;
    
    return logger;
}

// Destroy logger
void installation_logger_destroy(installation_logger_t* logger) {
    if (logger == NULL) {
        return;
    }
    
    pthread_mutex_lock(&logger->log_mutex);
    
    if (logger->log_file != NULL) {
        time_t time_now = time(NULL);
        fprintf(logger->log_file, "%s Logger destroyed\n", asctime(localtime(&time_now)));
        fflush(logger->log_file);
        fclose(logger->log_file);
        logger->log_file = NULL;
    }
    
    logger->is_initialized = 0;
    
    pthread_mutex_unlock(&logger->log_mutex);
    pthread_mutex_destroy(&logger->log_mutex);
    
    if (logger->buffer != NULL) {
        free(logger->buffer);
    }
    
    if (global_logger == logger) {
        global_logger = NULL;
    }
    
    free(logger);
}

// Get log level string
static const char* get_log_level_string(log_level_t level) {
    switch (level) {
        case LOG_INFO: return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR: return "ERROR";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

// Log message with logger instance
void log_message(installation_logger_t* logger, log_level_t level, const char* message) {
    if (message == NULL) {
        return;
    }
    
    installation_logger_t* log_target = logger;
    if (log_target == NULL) {
        log_target = global_logger;
    }
    
    if (log_target == NULL || !log_target->is_initialized) {
        // Fallback to stdout if logger not initialized
        printf("[%s] %s\n", get_log_level_string(level), message);
        return;
    }
    
    pthread_mutex_lock(&log_target->log_mutex);
    
    time_t time_now = time(NULL);
    struct tm* time_info = localtime(&time_now);
    
    // Format timestamp
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);
    
    // Write to log file
    fprintf(log_target->log_file, "[%s] [%s] %s\n", timestamp, get_log_level_string(level), message);
    fflush(log_target->log_file);
    
    pthread_mutex_unlock(&log_target->log_mutex);
}

// Simple log message function (uses global logger or stdout)
void log_message_simple(log_level_t level, const char* message) {
    log_message(NULL, level, message);
}
