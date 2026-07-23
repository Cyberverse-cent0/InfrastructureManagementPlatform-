#include "smart_logger.h"
#include "async_logger.h"
#include <stdlib.h>
#include <string.h>

// Global async logger instance
static struct async_logger* global_async_logger = NULL;
static bool async_mode_enabled = false;

// Get default logger configuration
logger_config_t get_default_logger_config(void) {
    logger_config_t config;
    config.enable_colors = true;
    config.enable_timestamps = true;
    config.enable_location = true;
    config.output_heads = OUTPUT_HEAD_ALL;
    config.log_file_path = NULL;
    config.log_directory = "logs";
    config.log_file_name = "application.log";
    config.enable_async = false;
    config.async_queue_size = 1024;
    return config;
}

// Initialize logger with configuration
installation_logger_t* initialize_smart_logger(logger_config_t* config) {
    if (config == NULL) {
        return NULL;
    }
    
    // Set up output heads
    set_output_heads(config->output_heads);
    
    // Set up colors
    enable_colors(config->enable_colors);
    
    // Initialize async logger if enabled
    if (config->enable_async) {
        async_logger_config_t async_config;
        async_config.queue_size = config->async_queue_size;
        async_config.enable_stdout = (config->output_heads & OUTPUT_HEAD_CONSOLE) != 0;
        async_config.enable_file = (config->output_heads & OUTPUT_HEAD_FILE) != 0;
        async_config.log_file_path = config->log_file_path;
        async_config.drop_messages_when_full = true;
        
        int result = initialize_async_logger(&async_config);
        if (result == 0) {
            async_mode_enabled = true;
        } else {
            // Log error but continue with sync mode
            fprintf(stderr, "Warning: Failed to initialize async logger (error %d), using sync mode\n", result);
            async_mode_enabled = false;
        }
    }
    
    // Initialize file logger if file output is enabled and not using async
    installation_logger_t* logger = NULL;
    if (!async_mode_enabled && (config->output_heads & OUTPUT_HEAD_FILE)) {
        logger = installation_logger_init(
            config->log_file_path ? "." : NULL,
            config->log_file_name,
            config->log_directory
        );
    }
    
    return logger;
}

// Cleanup smart logger
void cleanup_smart_logger(installation_logger_t* logger) {
    // Cleanup async logger first
    if (async_mode_enabled) {
        cleanup_async_logger();
        async_mode_enabled = false;
    }
    
    // Cleanup file logger
    if (logger != NULL) {
        installation_logger_destroy(logger);
    }
}

// Quick initialization (uses defaults)
installation_logger_t* quick_init_logger(void) {
    logger_config_t config = get_default_logger_config();
    return initialize_smart_logger(&config);
}

// Async logger management
int initialize_async_logger(async_logger_config_t* config) {
    if (global_async_logger != NULL) {
        return -1;  // Already initialized
    }
    
    global_async_logger = (struct async_logger*)malloc(sizeof(struct async_logger));
    if (global_async_logger == NULL) {
        return -2;
    }
    
    int result = async_logger_init(global_async_logger, config);
    if (result != 0) {
        free(global_async_logger);
        global_async_logger = NULL;
        return result;
    }
    
    return 0;
}

void cleanup_async_logger(void) {
    if (global_async_logger != NULL) {
        async_logger_destroy(global_async_logger);
        free(global_async_logger);
        global_async_logger = NULL;
    }
}

struct async_logger* get_async_logger(void) {
    return global_async_logger;
}

// Configuration helpers
void set_logger_to_console_only(void) {
    set_output_heads(OUTPUT_HEAD_CONSOLE);
}

void set_logger_to_file_only(void) {
    set_output_heads(OUTPUT_HEAD_FILE);
}

void set_logger_to_all(void) {
    set_output_heads(OUTPUT_HEAD_ALL);
}

// Async mode helpers
void enable_async_mode(bool enable) {
    async_mode_enabled = enable;
}

bool is_async_mode_enabled(void) {
    return async_mode_enabled;
}

// Integration: Override standard print functions with async versions
void set_print_functions_async(bool enable) {
    enable_async_mode(enable);
}

// Async print functions (non-blocking) - standalone implementations
int print_error_async(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, NULL);
    }
    // Fallback - just return without blocking
    return 1;
}

int print_info_async(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, NULL);
    }
    // Fallback - just return without blocking
    return 0;
}

int print_warning_async(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_WARNING, __FILE__, __func__, __LINE__, NULL);
    }
    // Fallback - just return without blocking
    return 0;
}

int print_debug_async(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, NULL);
    }
    // Fallback - just return without blocking
    return 0;
}

int print_success_async(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_SUCCESS, __FILE__, __func__, __LINE__, NULL);
    }
    // Fallback - just return without blocking
    return 0;
}

int print_critical_async(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_CRITICAL, __FILE__, __func__, __LINE__, NULL);
    }
    // Fallback - just return without blocking
    return 1;
}

// Override standard print functions when async mode is enabled
int print_error(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, NULL);
    }
    // Original blocking implementation
    printf("❌ ERROR: %s\n", message);
    return 1;
}

int print_info(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, NULL);
    }
    // Original blocking implementation
    printf("ℹ️ INFO: %s\n", message);
    return 0;
}

int print_warning(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_WARNING, __FILE__, __func__, __LINE__, NULL);
    }
    // Original blocking implementation
    printf("⚠️ WARNING: %s\n", message);
    return 0;
}

int print_debug(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, NULL);
    }
    // Original blocking implementation
    printf("🔍 DEBUG: %s\n", message);
    return 0;
}

int print_success(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_SUCCESS, __FILE__, __func__, __LINE__, NULL);
    }
    // Original blocking implementation
    printf("✅ SUCCESS: %s\n", message);
    return 0;
}

int print_critical(const char* message) {
    if (async_mode_enabled && global_async_logger != NULL) {
        return async_log(global_async_logger, message, ASYNC_LOG_LEVEL_CRITICAL, __FILE__, __func__, __LINE__, NULL);
    }
    // Original blocking implementation
    printf("🚨 CRITICAL: %s\n", message);
    return 1;
}