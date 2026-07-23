#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "async_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>

// ============================================================================
// Configuration and Defaults
// ============================================================================

// Get default configuration for async logger
async_logger_config_t get_default_async_config(void) {
    async_logger_config_t config;
    config.queue_size = 1024;  // Default queue size
    config.enable_stdout = true;
    config.enable_file = false;
    config.log_file_path = NULL;
    config.drop_messages_when_full = true;
    return config;
}

// ============================================================================
// Utility Functions
// ============================================================================

// Get current timestamp in seconds
static double get_timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Safe message duplication with error checking
static char* safe_strdup(const char* src) {
    if (src == NULL) {
        return NULL;
    }
    size_t len = strlen(src);
    char* dest = (char*)malloc(len + 1);
    if (dest == NULL) {
        return NULL;
    }
    memcpy(dest, src, len + 1);
    return dest;
}

// ============================================================================
// Message Formatting
// ============================================================================

// Format log message for output with icons and colors
static void format_log_message(char* buffer, size_t buffer_size, 
                               const async_log_message_t* msg) {
    const char* level_names[] = {"DEBUG", "INFO", "SUCCESS", "WARNING", "ERROR", "CRITICAL"};
    const char* icons[] = {"🔍", "ℹ️", "✅", "⚠️", "❌", "🚨"};
    const char* colors[] = {
        "\033[1;34m",  // DEBUG - Blue
        "\033[1;32m",  // INFO - Green
        "\033[1;32m",  // SUCCESS - Green
        "\033[1;33m",  // WARNING - Yellow
        "\033[1;31m",  // ERROR - Red
        "\033[1;35m"   // CRITICAL - Magenta
    };
    const char* color_reset = "\033[0m";
    
    // Validate log level
    int level_index = (int)msg->log_level;
    if (level_index < 0 || level_index >= 6) {
        level_index = 1; // Default to INFO
    }
    
    // Format with context information
    if (msg->module) {
        snprintf(buffer, buffer_size, 
                "%s%s %s [%s]%s: %s",
                colors[level_index], icons[level_index], 
                level_names[level_index], msg->module, color_reset,
                msg->message);
    } else if (msg->file && msg->function) {
        snprintf(buffer, buffer_size,
                "%s%s %s [%s:%s:%d]%s: %s",
                colors[level_index], icons[level_index],
                level_names[level_index], msg->file, msg->function, msg->line, color_reset,
                msg->message);
    } else {
        snprintf(buffer, buffer_size,
                "%s%s %s%s: %s",
                colors[level_index], icons[level_index],
                level_names[level_index], color_reset,
                msg->message);
    }
}

// ============================================================================
// Output Thread
// ============================================================================

// Output thread function - processes messages from queue
static void* output_thread_func(void* arg) {
    struct async_logger* logger = (struct async_logger*)arg;
    char output_buffer[512];
    
    if (logger == NULL) {
        return NULL;
    }
    
    while (logger->running || logger->head != logger->tail) {
        pthread_mutex_lock(&logger->queue_mutex);
        
        // Wait for messages or stop signal
        while (logger->head == logger->tail && logger->running) {
            pthread_cond_wait(&logger->queue_cond, &logger->queue_mutex);
        }
        
        // Check if we should stop
        if (!logger->running && logger->head == logger->tail) {
            pthread_mutex_unlock(&logger->queue_mutex);
            break;
        }
        
        // Process message
        if (logger->head != logger->tail) {
            async_log_message_t* msg = &logger->message_queue[logger->head];
            
            // Format and output message
            format_log_message(output_buffer, sizeof(output_buffer), msg);
            
            if (logger->config.enable_stdout) {
                printf("%s\n", output_buffer);
                fflush(stdout);
            }
            
            if (logger->config.enable_file && logger->config.log_file_path) {
                FILE* log_file = fopen(logger->config.log_file_path, "a");
                if (log_file) {
                    fprintf(log_file, "[%.3f] %s\n", msg->timestamp, msg->message);
                    fclose(log_file);
                }
            }
            
            // Free message memory
            if (msg->message) {
                free(msg->message);
                msg->message = NULL;
            }
            
            // Move head forward
            logger->head = (logger->head + 1) % logger->queue_capacity;
            logger->messages_processed++;
            
            // Signal that queue is not full
            pthread_cond_signal(&logger->queue_not_full);
        }
        
        pthread_mutex_unlock(&logger->queue_mutex);
    }
    
    return NULL;
}

// ============================================================================
// Logger Lifecycle
// ============================================================================

// Initialize async logger with enhanced error checking
int async_logger_init(struct async_logger* logger, const async_logger_config_t* config) {
    if (logger == NULL) {
        return -1;  // Invalid logger pointer
    }
    
    // Initialize structure to zero
    memset(logger, 0, sizeof(struct async_logger));
    
    // Use default config if none provided
    if (config == NULL) {
        logger->config = get_default_async_config();
    } else {
        logger->config = *config;
    }
    
    // Validate queue size
    size_t capacity = logger->config.queue_size;
    if (capacity == 0) {
        capacity = 1024;  // Default minimum
    }
    if (capacity < 16) {
        capacity = 16;  // Minimum viable queue size
    }
    
    // Allocate message queue
    logger->message_queue = (async_log_message_t*)calloc(capacity, sizeof(async_log_message_t));
    if (logger->message_queue == NULL) {
        return -2;  // Memory allocation failed
    }
    
    logger->queue_capacity = capacity;
    logger->queue_size = 0;
    logger->head = 0;
    logger->tail = 0;
    logger->running = true;
    logger->initialized = true;
    
    // Initialize mutex and condition variables
    if (pthread_mutex_init(&logger->queue_mutex, NULL) != 0) {
        free(logger->message_queue);
        logger->message_queue = NULL;
        return -3;  // Mutex initialization failed
    }
    
    if (pthread_cond_init(&logger->queue_cond, NULL) != 0) {
        pthread_mutex_destroy(&logger->queue_mutex);
        free(logger->message_queue);
        logger->message_queue = NULL;
        return -4;  // Condition variable initialization failed
    }
    
    if (pthread_cond_init(&logger->queue_not_full, NULL) != 0) {
        pthread_cond_destroy(&logger->queue_cond);
        pthread_mutex_destroy(&logger->queue_mutex);
        free(logger->message_queue);
        logger->message_queue = NULL;
        return -5;  // Condition variable initialization failed
    }
    
    // Start output thread
    if (pthread_create(&logger->output_thread, NULL, output_thread_func, logger) != 0) {
        pthread_cond_destroy(&logger->queue_not_full);
        pthread_cond_destroy(&logger->queue_cond);
        pthread_mutex_destroy(&logger->queue_mutex);
        free(logger->message_queue);
        logger->message_queue = NULL;
        return -6;  // Thread creation failed
    }
    
    return 0;  // Success
}

// Cleanup async logger with proper resource cleanup
void async_logger_destroy(struct async_logger* logger) {
    if (logger == NULL || !logger->initialized) {
        return;
    }
    
    // Stop the logger first
    async_logger_stop(logger);
    
    // Free any remaining messages in queue
    pthread_mutex_lock(&logger->queue_mutex);
    while (logger->head != logger->tail) {
        async_log_message_t* msg = &logger->message_queue[logger->head];
        if (msg->message) {
            free(msg->message);
        }
        logger->head = (logger->head + 1) % logger->queue_capacity;
    }
    pthread_mutex_unlock(&logger->queue_mutex);
    
    // Cleanup synchronization primitives
    pthread_cond_destroy(&logger->queue_not_full);
    pthread_cond_destroy(&logger->queue_cond);
    pthread_mutex_destroy(&logger->queue_mutex);
    
    // Free queue memory
    free(logger->message_queue);
    
    logger->initialized = false;
}

// ============================================================================
// Message Enqueue
// ============================================================================

// Non-blocking log message enqueue
int async_log(struct async_logger* logger, const char* message, async_log_level_t log_level,
              const char* file, const char* function, int line, const char* module) {
    if (logger == NULL || !logger->initialized || message == NULL) {
        return -1;  // Invalid parameters
    }
    
    pthread_mutex_lock(&logger->queue_mutex);
    
    // Check if queue is full
    size_t next_tail = (logger->tail + 1) % logger->queue_capacity;
    if (next_tail == logger->head) {
        logger->queue_full_events++;
        
        if (logger->config.drop_messages_when_full) {
            logger->messages_dropped++;
            pthread_mutex_unlock(&logger->queue_mutex);
            return -2;  // Queue full, message dropped
        } else {
            // Wait for space in queue
            while (next_tail == logger->head && logger->running) {
                pthread_cond_wait(&logger->queue_not_full, &logger->queue_mutex);
                next_tail = (logger->tail + 1) % logger->queue_capacity;
            }
            
            if (!logger->running) {
                pthread_mutex_unlock(&logger->queue_mutex);
                return -3;  // Logger stopped
            }
        }
    }
    
    // Add message to queue
    async_log_message_t* msg = &logger->message_queue[logger->tail];
    msg->message = safe_strdup(message);
    
    // Check if duplication failed
    if (msg->message == NULL) {
        logger->messages_dropped++;
        pthread_mutex_unlock(&logger->queue_mutex);
        return -4;  // Memory allocation failed
    }
    
    msg->message_len = strlen(message);
    msg->log_level = log_level;
    msg->timestamp = get_timestamp();
    msg->file = file;
    msg->function = function;
    msg->line = line;
    msg->module = module;
    
    logger->tail = next_tail;
    logger->queue_size++;
    
    // Signal output thread
    pthread_cond_signal(&logger->queue_cond);
    
    pthread_mutex_unlock(&logger->queue_mutex);
    
    return 0;  // Success
}

// ============================================================================
// Status and Statistics
// ============================================================================

// Check if logger is running
bool async_logger_is_running(const struct async_logger* logger) {
    if (logger == NULL || !logger->initialized) {
        return false;
    }
    
    return logger->running;
}

// Get statistics - must be non-const to allow mutex lock
void async_logger_get_stats(struct async_logger* logger,
                            unsigned long* processed,
                            unsigned long* dropped,
                            unsigned long* full_events) {
    if (logger == NULL) {
        return;
    }
    
    pthread_mutex_lock(&logger->queue_mutex);
    
    if (processed) *processed = logger->messages_processed;
    if (dropped) *dropped = logger->messages_dropped;
    if (full_events) *full_events = logger->queue_full_events;
    
    pthread_mutex_unlock(&logger->queue_mutex);
}

// Flush remaining messages (blocking)
int async_logger_flush(struct async_logger* logger) {
    if (logger == NULL || !logger->initialized) {
        return -1;
    }
    
    pthread_mutex_lock(&logger->queue_mutex);
    
    // Wait for queue to empty
    while (logger->head != logger->tail && logger->running) {
        pthread_cond_signal(&logger->queue_cond);
        pthread_mutex_unlock(&logger->queue_mutex);
        struct timespec ts = {0, 1000000};  // 1ms delay
        nanosleep(&ts, NULL);
        pthread_mutex_lock(&logger->queue_mutex);
    }
    
    pthread_mutex_unlock(&logger->queue_mutex);
    
    return 0;
}

// Stop logger gracefully
int async_logger_stop(struct async_logger* logger) {
    if (logger == NULL || !logger->initialized) {
        return -1;
    }
    
    pthread_mutex_lock(&logger->queue_mutex);
    logger->running = false;
    pthread_cond_signal(&logger->queue_cond);
    pthread_cond_broadcast(&logger->queue_not_full);
    pthread_mutex_unlock(&logger->queue_mutex);
    
    // Wait for thread to finish
    pthread_join(logger->output_thread, NULL);
    
    return 0;
}