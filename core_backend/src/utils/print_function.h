#ifndef PRINT_FUNCTION_H
#define PRINT_FUNCTION_H

#include <stdio.h>
#include <stdbool.h>

// Type definitions
typedef enum icon_purser_e {
    ICON_PURSER_SUCCESS = 0,
    ICON_PURSER_ERROR = -1
} icon_purser_t;

typedef struct icon_location {  
    int x;
    int y;
    char* path;
} icon_location_t;

// Log levels for smarter logging
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_SUCCESS = 2,
    LOG_LEVEL_WARNING = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_CRITICAL = 5
} log_level_enum_t;

// Output heads (destinations)
typedef enum {
    OUTPUT_HEAD_CONSOLE = 1,
    OUTPUT_HEAD_FILE = 2,
    OUTPUT_HEAD_NETWORK = 4,
    OUTPUT_HEAD_ALL = 7
} output_head_t;

// Icon definitions for different log levels
typedef struct {
    const char* icon;
    const char* color_code;
} log_icon_t;

// Smart logging context
typedef struct {
    const char* module;
    const char* function;
    int line;
    bool show_timestamp;
    bool show_location;
    output_head_t output_heads;
} log_context_t;

// Function declarations
icon_purser_t icon_purser(void);
log_icon_t get_log_icon(log_level_enum_t level);

// Basic print functions (enhanced with icons)
int print_error(const char* message);
int print_info(const char* message);
int print_warning(const char* message);
int print_debug(const char* message);
int print_success(const char* message);
int print_critical(const char* message);

// Async print functions (non-blocking)
int print_error_async(const char* message);
int print_info_async(const char* message);
int print_warning_async(const char* message);
int print_debug_async(const char* message);
int print_success_async(const char* message);
int print_critical_async(const char* message);

// Smart logging functions
void log_smart(log_level_enum_t level, const char* message, log_context_t* context);
void log_smart_simple(log_level_enum_t level, const char* message);

// Context management
log_context_t* create_log_context(const char* module, const char* function, int line);
void destroy_log_context(log_context_t* context);

// Output head management
void set_output_heads(output_head_t heads);
output_head_t get_output_heads(void);

// Color management
void enable_colors(bool enable);
bool are_colors_enabled(void);

// Async logger integration (set function pointers for async logging)
void set_async_logging_enabled(bool enabled);
bool is_async_logging_enabled(void);

// Async logger function pointer types
typedef int (*async_log_func_t)(const char* message, int level, const char* file, const char* function, int line, const char* module);

// Set async logging function
void set_async_log_function(async_log_func_t func);

#endif // PRINT_FUNCTION_H
