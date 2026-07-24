#define _POSIX_C_SOURCE 200809L
#include "print_function.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "logger.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

// Global settings
static bool colors_enabled = true;
static output_head_t current_output_heads = OUTPUT_HEAD_ALL;

// Icon definitions for different log levels
static const log_icon_t log_icons[] = {
    {"🔍", "\033[1;34m"},   // DEBUG - Blue
    {"ℹ️", "\033[1;32m"},    // INFO - Green
    {"✅", "\033[1;32m"},    // SUCCESS - Green
    {"⚠️", "\033[1;33m"},    // WARNING - Yellow
    {"❌", "\033[1;31m"},    // ERROR - Red
    {"🚨", "\033[1;35m"}     // CRITICAL - Magenta
};

// ANSI color codes for Windows compatibility
static const char* color_reset = "\033[0m";
static const char* color_bold = "\033[1m";

// Windows color mapping
#ifdef _WIN32
static int get_windows_color(int log_level) {
    switch (log_level) {
        case LOG_LEVEL_DEBUG:    return FOREGROUND_INTENSITY | FOREGROUND_BLUE;
        case LOG_LEVEL_INFO:     return FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        case LOG_LEVEL_SUCCESS:  return FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        case LOG_LEVEL_WARNING:  return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        case LOG_LEVEL_ERROR:    return FOREGROUND_INTENSITY | FOREGROUND_RED;
        case LOG_LEVEL_CRITICAL: return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
        default:                 return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
#endif

// Icon parser function
icon_purser_t icon_purser(void){
    // implement code here
    icon_purser_t result = ICON_PURSER_SUCCESS;
    return result;
}

// Get log icon for level
log_icon_t get_log_icon(log_level_enum_t level) {
    if (level >= 0 && level < sizeof(log_icons) / sizeof(log_icons[0])) {
        return log_icons[level];
    }
    // Default to info icon
    return log_icons[LOG_LEVEL_INFO];
}

// Enable/disable colors
void enable_colors(bool enable) {
    colors_enabled = enable;
}

bool are_colors_enabled(void) {
    return colors_enabled;
}

// Set output heads
void set_output_heads(output_head_t heads) {
    current_output_heads = heads;
}

output_head_t get_output_heads(void) {
    return current_output_heads;
}

// Get timestamp string
static void get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Print with color and icon
static void print_with_icon_and_color(log_level_enum_t level, const char* prefix, const char* message) {
    if (message == NULL) {
        return;
    }
    
    log_icon_t icon = get_log_icon(level);
    
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (colors_enabled) {
        SetConsoleTextAttribute(hConsole, get_windows_color(level));
        printf("%s %s: %s\n", icon.icon, prefix, message);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    } else {
        printf("%s %s: %s\n", icon.icon, prefix, message);
    }
#else
    if (colors_enabled) {
        printf("%s%s %s: %s%s\n", icon.color_code, icon.icon, prefix, message, color_reset);
    } else {
        printf("%s %s: %s\n", icon.icon, prefix, message);
    }
#endif
}

// Basic print functions (enhanced with icons)
int print_error(const char* message) {
    if (message == NULL) {
        return 1;
    }
    
    print_with_icon_and_color(LOG_LEVEL_ERROR, "ERROR", message);
    log_message_simple(LOG_ERROR, message);
    return 1;
}

int print_info(const char* message) {
    if (message == NULL) {
        return 0;
    }
    
    print_with_icon_and_color(LOG_LEVEL_INFO, "INFO", message);
    log_message_simple(LOG_INFO, message);
    return 0;
}

int print_warning(const char* message) {
    if (message == NULL) {
        return 0;
    }
    
    print_with_icon_and_color(LOG_LEVEL_WARNING, "WARNING", message);
    log_message_simple(LOG_WARNING, message);
    return 0;
}

int print_debug(const char* message) {
    if (message == NULL) {
        return 0;
    }
    
    print_with_icon_and_color(LOG_LEVEL_DEBUG, "DEBUG", message);
    log_message_simple(LOG_DEBUG, message);
    return 0;
}

int print_success(const char* message) {
    if (message == NULL) {
        return 0;
    }
    
    print_with_icon_and_color(LOG_LEVEL_SUCCESS, "SUCCESS", message);
    log_message_simple(LOG_INFO, message);
    return 0;
}

int print_critical(const char* message) {
    if (message == NULL) {
        return 1;
    }
    
    print_with_icon_and_color(LOG_LEVEL_CRITICAL, "CRITICAL", message);
    log_message_simple(LOG_ERROR, message);
    return 1;
}

// Smart logging with context
void log_smart(log_level_enum_t level, const char* message, log_context_t* context) {
    if (message == NULL) {
        return;
    }
    
    log_icon_t icon = get_log_icon(level);
    char timestamp[32];
    const char* level_names[] = {"DEBUG", "INFO", "SUCCESS", "WARNING", "ERROR", "CRITICAL"};
    
    if (context && context->show_timestamp) {
        get_timestamp(timestamp, sizeof(timestamp));
    }
    
    // Console output
    if (current_output_heads & OUTPUT_HEAD_CONSOLE) {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (colors_enabled) {
            SetConsoleTextAttribute(hConsole, get_windows_color(level));
            
            if (context && context->show_timestamp) {
                printf("[%s] ", timestamp);
            }
            
            printf("%s %s", icon.icon, level_names[level]);
            
            if (context && context->show_location && context->module) {
                printf(" [%s:%s:%d]", context->module, 
                       context->function ? context->function : "?", context->line);
            }
            
            printf(": %s\n", message);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else {
            if (context && context->show_timestamp) {
                printf("[%s] ", timestamp);
            }
            printf("%s %s: %s\n", icon.icon, level_names[level], message);
        }
#else
        if (colors_enabled) {
            if (context && context->show_timestamp) {
                printf("%s[%s]%s ", color_bold, timestamp, color_reset);
            }
            
            printf("%s%s %s%s", icon.color_code, icon.icon, level_names[level], color_reset);
            
            if (context && context->show_location && context->module) {
                printf(" %s[%s:%s:%d]%s", color_bold, context->module,
                       context->function ? context->function : "?", context->line, color_reset);
            }
            
            printf(": %s\n", message);
        } else {
            if (context && context->show_timestamp) {
                printf("[%s] ", timestamp);
            }
            printf("%s %s: %s\n", icon.icon, level_names[level], message);
        }
#endif
    }
    
    // File output
    if (current_output_heads & OUTPUT_HEAD_FILE) {
        // Convert to simple log level for file logger
        log_level_t simple_level;
        switch (level) {
            case LOG_LEVEL_DEBUG:    simple_level = LOG_DEBUG; break;
            case LOG_LEVEL_INFO:     simple_level = LOG_INFO; break;
            case LOG_LEVEL_SUCCESS:  simple_level = LOG_INFO; break;
            case LOG_LEVEL_WARNING:  simple_level = LOG_WARNING; break;
            case LOG_LEVEL_ERROR:    simple_level = LOG_ERROR; break;
            case LOG_LEVEL_CRITICAL: simple_level = LOG_ERROR; break;
            default:                 simple_level = LOG_INFO; break;
        }
        
        char formatted_message[512];
        if (context && context->show_location && context->module) {
            snprintf(formatted_message, sizeof(formatted_message), 
                    "[%s:%s:%d] %s", context->module,
                    context->function ? context->function : "?", context->line, message);
        } else {
            snprintf(formatted_message, sizeof(formatted_message), "%s", message);
        }
        
        log_message_simple(simple_level, formatted_message);
    }
    
    // Network output (placeholder for future implementation)
    if (current_output_heads & OUTPUT_HEAD_NETWORK) {
        // TODO: Implement network logging
    }
}

// Simple smart logging without context
void log_smart_simple(log_level_enum_t level, const char* message) {
    log_smart(level, message, NULL);
}

// Context management
log_context_t* create_log_context(const char* module, const char* function, int line) {
    log_context_t* context = (log_context_t*)malloc(sizeof(log_context_t));
    if (context == NULL) {
        return NULL;
    }
    
    context->module = module ? strdup(module) : NULL;
    context->function = function ? strdup(function) : NULL;
    context->line = line;
    context->show_timestamp = true;
    context->show_location = true;
    context->output_heads = current_output_heads;
    
    return context;
}

// Destroy log context
void destroy_log_context(log_context_t* context) {
    if (context == NULL) {
        return;
    }
    
    if (context->module) {
        free((void*)context->module);
    }
    if (context->function) {
        free((void*)context->function);
    }
    
    free(context);
}