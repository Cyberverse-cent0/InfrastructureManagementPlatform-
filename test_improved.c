#include "scipts/logger.h"
#include "scipts/print_function.h"
#include "scipts/user_databse.h"
#include "scipts/get_thead.h"
#include "scipts/smart_logger.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("=== Testing Improved Smart Logging System ===\n\n");
    
    // Initialize smart logger with default configuration
    installation_logger_t* logger = quick_init_logger();
    if (logger == NULL) {
        printf("Failed to initialize smart logger\n");
        return 1;
    }
    
    printf("1. Testing basic print functions with icons:\n");
    print_info("Testing info message with icon");
    print_warning("Testing warning message with icon");
    print_error("Testing error message with icon");
    print_debug("Testing debug message with icon");
    print_success("Testing success message with icon");
    print_critical("Testing critical message with icon");
    
    printf("\n2. Testing smart logging macros:\n");
    LOG_DEBUG("Debug message using macro");
    LOG_INFO("Info message using macro");
    LOG_SUCCESS("Success message using macro");
    LOG_WARNING("Warning message using macro");
    LOG_ERROR("Error message using macro");
    LOG_CRITICAL("Critical message using macro");
    
    printf("\n3. Testing context-aware logging:\n");
    log_context_t* ctx = create_log_context("MAIN_MODULE", "main", 42);
    log_smart(LOG_LEVEL_INFO, "Message with context", ctx);
    log_smart(LOG_LEVEL_SUCCESS, "Success with context", ctx);
    log_smart(LOG_LEVEL_ERROR, "Error with context", ctx);
    destroy_log_context(ctx);
    
    printf("\n4. Testing automatic location logging:\n");
    LOG_HERE_DEBUG("Debug message with automatic location");
    LOG_HERE_INFO("Info message with automatic location");
    LOG_HERE_SUCCESS("Success message with automatic location");
    LOG_HERE_WARNING("Warning message with automatic location");
    LOG_HERE_ERROR("Error message with automatic location");
    
    printf("\n5. Testing module-specific logging:\n");
    LOG_MODULE_DEBUG("DATABASE", "Database connection attempt");
    LOG_MODULE_INFO("DATABASE", "Database connected successfully");
    LOG_MODULE_SUCCESS("AUTH", "User authenticated");
    LOG_MODULE_WARNING("NETWORK", "High latency detected");
    LOG_MODULE_ERROR("CONFIG", "Configuration file not found");
    
    printf("\n6. Testing output head switching:\n");
    printf("   Switching to console only...\n");
    set_logger_to_console_only();
    LOG_INFO("This should only appear in console");
    
    printf("   Switching to file only...\n");
    set_logger_to_file_only();
    LOG_INFO("This should only appear in file");
    
    printf("   Switching back to all outputs...\n");
    set_logger_to_all();
    LOG_INFO("This should appear in both console and file");
    
    printf("\n7. Testing color toggling:\n");
    printf("   Disabling colors...\n");
    enable_colors(false);
    LOG_INFO("Message without colors");
    
    printf("   Enabling colors...\n");
    enable_colors(true);
    LOG_INFO("Message with colors restored");
    
    printf("\n8. Testing thread functions:\n");
    int thread_count = get_thread_count();
    LOG_MODULE_INFO("SYSTEM", "Thread count detected");
    
    bool supports_threading = check_if_support_threading();
    LOG_MODULE_INFO("SYSTEM", "Threading support verified");
    
    (void)thread_count; // Suppress unused warning
    (void)supports_threading; // Suppress unused warning
    
    printf("\n9. Testing database functions:\n");
    database_connection_t* db = database_init("localhost", 5432, "testdb", "testuser", "testpass");
    if (db != NULL) {
        LOG_MODULE_SUCCESS("DATABASE", "Database initialized successfully");
        database_destroy(db);
    } else {
        LOG_MODULE_ERROR("DATABASE", "Failed to initialize database");
    }
    
    // Clean up
    cleanup_smart_logger(logger);
    
    printf("\n=== All smart logging tests completed successfully! ===\n");
    return 0;
}