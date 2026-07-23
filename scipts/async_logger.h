#ifndef ASYNC_LOGGER_H
#define ASYNC_LOGGER_H

#include <stdbool.h>
#include <pthread.h>
#include <stddef.h>

// Log level definitions (avoiding circular dependency)
typedef enum {
    ASYNC_LOG_LEVEL_DEBUG = 0,
    ASYNC_LOG_LEVEL_INFO = 1,
    ASYNC_LOG_LEVEL_SUCCESS = 2,
    ASYNC_LOG_LEVEL_WARNING = 3,
    ASYNC_LOG_LEVEL_ERROR = 4,
    ASYNC_LOG_LEVEL_CRITICAL = 5
} async_log_level_t;

// Async log message structure
typedef struct {
    char* message;
    size_t message_len;
    async_log_level_t log_level;
    double timestamp;
    const char* file;
    const char* function;
    int line;
    const char* module;
} async_log_message_t;

// Async logger configuration
typedef struct {
    size_t queue_size;
    bool enable_stdout;
    bool enable_file;
    const char* log_file_path;
    bool drop_messages_when_full;
} async_logger_config_t;

// Forward declaration
typedef struct async_logger async_logger_t;

// Async logger state
struct async_logger {
    pthread_t output_thread;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;
    pthread_cond_t queue_not_full;
    
    async_log_message_t* message_queue;
    size_t queue_size;
    size_t queue_capacity;
    
    size_t head;
    size_t tail;
    
    bool running;
    bool initialized;
    
    async_logger_config_t config;
    
    // Statistics
    unsigned long messages_processed;
    unsigned long messages_dropped;
    unsigned long queue_full_events;
};

// Default configuration
async_logger_config_t get_default_async_config(void);

// Initialize async logger
int async_logger_init(async_logger_t* logger, const async_logger_config_t* config);

// Cleanup async logger
void async_logger_destroy(async_logger_t* logger);

// Non-blocking log message
int async_log(async_logger_t* logger, const char* message, async_log_level_t log_level, 
              const char* file, const char* function, int line, const char* module);

// Check if logger is running
bool async_logger_is_running(const async_logger_t* logger);

// Get statistics
void async_logger_get_stats(struct async_logger* logger, 
                            unsigned long* processed, 
                            unsigned long* dropped, 
                            unsigned long* full_events);

// Flush remaining messages (blocking)
int async_logger_flush(async_logger_t* logger);

// Stop logger gracefully
int async_logger_stop(async_logger_t* logger);

// Convenience macros for async logging
#define ASYNC_LOG(logger, msg, level) \
    async_log(logger, msg, level, __FILE__, __func__, __LINE__, NULL)

#endif // ASYNC_LOGGER_H