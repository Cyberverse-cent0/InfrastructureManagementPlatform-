#ifndef SMART_LOGGER_H
#define SMART_LOGGER_H

#include "print_function.h"
#include "logger.h"
#include "async_logger.h"

// Convenience macros for easy logging in main program
#define LOG_DEBUG(msg) log_smart_simple(LOG_LEVEL_DEBUG, msg)
#define LOG_INFO(msg) log_smart_simple(LOG_LEVEL_INFO, msg)
#define LOG_SUCCESS(msg) log_smart_simple(LOG_LEVEL_SUCCESS, msg)
#define LOG_WARNING(msg) log_smart_simple(LOG_LEVEL_WARNING, msg)
#define LOG_ERROR(msg) log_smart_simple(LOG_LEVEL_ERROR, msg)
#define LOG_CRITICAL(msg) log_smart_simple(LOG_LEVEL_CRITICAL, msg)

// Context-aware logging macros
#define LOG_DEBUG_CTX(msg, ctx) log_smart(LOG_LEVEL_DEBUG, msg, ctx)
#define LOG_INFO_CTX(msg, ctx) log_smart(LOG_LEVEL_INFO, msg, ctx)
#define LOG_SUCCESS_CTX(msg, ctx) log_smart(LOG_LEVEL_SUCCESS, msg, ctx)
#define LOG_WARNING_CTX(msg, ctx) log_smart(LOG_LEVEL_WARNING, msg, ctx)
#define LOG_ERROR_CTX(msg, ctx) log_smart(LOG_LEVEL_ERROR, msg, ctx)
#define LOG_CRITICAL_CTX(msg, ctx) log_smart(LOG_LEVEL_CRITICAL, msg, ctx)

// Automatic context creation macros
#define LOG_HERE_DEBUG(msg) do { \
    log_context_t* ctx = create_log_context(__FILE__, __func__, __LINE__); \
    log_smart(LOG_LEVEL_DEBUG, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_HERE_INFO(msg) do { \
    log_context_t* ctx = create_log_context(__FILE__, __func__, __LINE__); \
    log_smart(LOG_LEVEL_INFO, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_HERE_SUCCESS(msg) do { \
    log_context_t* ctx = create_log_context(__FILE__, __func__, __LINE__); \
    log_smart(LOG_LEVEL_SUCCESS, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_HERE_WARNING(msg) do { \
    log_context_t* ctx = create_log_context(__FILE__, __func__, __LINE__); \
    log_smart(LOG_LEVEL_WARNING, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_HERE_ERROR(msg) do { \
    log_context_t* ctx = create_log_context(__FILE__, __func__, __LINE__); \
    log_smart(LOG_LEVEL_ERROR, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_HERE_CRITICAL(msg) do { \
    log_context_t* ctx = create_log_context(__FILE__, __func__, __LINE__); \
    log_smart(LOG_LEVEL_CRITICAL, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

// Module-specific logging
#define LOG_MODULE_DEBUG(module, msg) do { \
    log_context_t* ctx = create_log_context(module, __func__, __LINE__); \
    log_smart(LOG_LEVEL_DEBUG, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_MODULE_INFO(module, msg) do { \
    log_context_t* ctx = create_log_context(module, __func__, __LINE__); \
    log_smart(LOG_LEVEL_INFO, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_MODULE_SUCCESS(module, msg) do { \
    log_context_t* ctx = create_log_context(module, __func__, __LINE__); \
    log_smart(LOG_LEVEL_SUCCESS, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_MODULE_WARNING(module, msg) do { \
    log_context_t* ctx = create_log_context(module, __func__, __LINE__); \
    log_smart(LOG_LEVEL_WARNING, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_MODULE_ERROR(module, msg) do { \
    log_context_t* ctx = create_log_context(module, __func__, __LINE__); \
    log_smart(LOG_LEVEL_ERROR, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

#define LOG_MODULE_CRITICAL(module, msg) do { \
    log_context_t* ctx = create_log_context(module, __func__, __LINE__); \
    log_smart(LOG_LEVEL_CRITICAL, msg, ctx); \
    destroy_log_context(ctx); \
} while(0)

// Async logging macros (non-blocking)
#define ASYNC_LOG_DEBUG(msg) async_log(get_async_logger(), msg, ASYNC_LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, NULL)
#define ASYNC_LOG_INFO(msg) async_log(get_async_logger(), msg, ASYNC_LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, NULL)
#define ASYNC_LOG_SUCCESS(msg) async_log(get_async_logger(), msg, ASYNC_LOG_LEVEL_SUCCESS, __FILE__, __func__, __LINE__, NULL)
#define ASYNC_LOG_WARNING(msg) async_log(get_async_logger(), msg, ASYNC_LOG_LEVEL_WARNING, __FILE__, __func__, __LINE__, NULL)
#define ASYNC_LOG_ERROR(msg) async_log(get_async_logger(), msg, ASYNC_LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, NULL)
#define ASYNC_LOG_CRITICAL(msg) async_log(get_async_logger(), msg, ASYNC_LOG_LEVEL_CRITICAL, __FILE__, __func__, __LINE__, NULL)

// Async print functions (non-blocking)
int print_error_async(const char* message);
int print_info_async(const char* message);
int print_warning_async(const char* message);
int print_debug_async(const char* message);
int print_success_async(const char* message);
int print_critical_async(const char* message);

// Integration: Override standard print functions with async versions
// When async mode is enabled, all print_* functions automatically become non-blocking
void set_print_functions_async(bool enable);

// Initialization and configuration
typedef struct {
    bool enable_colors;
    bool enable_timestamps;
    bool enable_location;
    output_head_t output_heads;
    const char* log_file_path;
    const char* log_directory;
    const char* log_file_name;
    bool enable_async;  // Enable async logging
    size_t async_queue_size;
} logger_config_t;

// Default configuration
logger_config_t get_default_logger_config(void);

// Initialize logger with configuration
installation_logger_t* initialize_smart_logger(logger_config_t* config);

// Cleanup smart logger
void cleanup_smart_logger(installation_logger_t* logger);

// Quick initialization (uses defaults)
installation_logger_t* quick_init_logger(void);

// Async logger management
int initialize_async_logger(async_logger_config_t* config);
void cleanup_async_logger(void);
struct async_logger* get_async_logger(void);

// Configuration helpers
void set_logger_to_console_only(void);
void set_logger_to_file_only(void);
void set_logger_to_all(void);

// Async mode helpers
void enable_async_mode(bool enable);
bool is_async_mode_enabled(void);

#endif // SMART_LOGGER_H