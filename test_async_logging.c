#include "scipts/smart_logger.h"
#include "scipts/async_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Simulate application work
void do_work(int iterations) {
    for (int i = 0; i < iterations; i++) {
        // Simulate some work
        volatile int x = 0;
        for (int j = 0; j < 1000; j++) {
            x += j;
        }
        (void)x; // Suppress unused warning
    }
}

int main(void) {
    printf("=== Testing Async Logging System ===\n\n");
    
    // Test 1: Sync logging (baseline)
    printf("1. Testing synchronous logging (baseline):\n");
    logger_config_t sync_config = get_default_logger_config();
    sync_config.enable_async = false;
    installation_logger_t* sync_logger = initialize_smart_logger(&sync_config);
    
    clock_t start = clock();
    for (int i = 0; i < 100; i++) {
        LOG_INFO("Synchronous log message");
        do_work(100);
    }
    clock_t end = clock();
    double sync_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    cleanup_smart_logger(sync_logger);
    printf("   Synchronous logging time: %.3f seconds\n\n", sync_time);
    
    // Test 2: Async logging
    printf("2. Testing asynchronous logging:\n");
    logger_config_t async_config = get_default_logger_config();
    async_config.enable_async = true;
    async_config.async_queue_size = 2048;
    installation_logger_t* async_logger = initialize_smart_logger(&async_config);
    
    start = clock();
    for (int i = 0; i < 100; i++) {
        ASYNC_LOG_INFO("Asynchronous log message");
        do_work(100);
    }
    end = clock();
    double async_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Get async logger statistics
    struct async_logger* logger = get_async_logger();
    unsigned long processed, dropped, full_events;
    async_logger_get_stats(logger, &processed, &dropped, &full_events);
    
    printf("   Asynchronous logging time: %.3f seconds\n", async_time);
    printf("   Messages processed: %lu\n", processed);
    printf("   Messages dropped: %lu\n", dropped);
    printf("   Queue full events: %lu\n", full_events);
    printf("   Performance improvement: %.1fx\n\n", sync_time / async_time);
    
    // Test 3: Different log levels
    printf("3. Testing different log levels asynchronously:\n");
    ASYNC_LOG_DEBUG("Debug message");
    ASYNC_LOG_INFO("Info message");
    ASYNC_LOG_SUCCESS("Success message");
    ASYNC_LOG_WARNING("Warning message");
    ASYNC_LOG_ERROR("Error message");
    ASYNC_LOG_CRITICAL("Critical message");
    
    printf("   Sent messages with different log levels\n\n");
    
    // Test 4: Module-based logging
    printf("4. Testing module-based asynchronous logging:\n");
    ASYNC_LOG_INFO("Database connection established");
    ASYNC_LOG_INFO("Network request completed");
    ASYNC_LOG_INFO("User authenticated successfully");
    ASYNC_LOG_INFO("Configuration loaded");
    
    printf("   Sent module-specific messages\n\n");
    
    // Test 5: High-volume logging
    printf("5. Testing high-volume logging (1000 messages):\n");
    start = clock();
    for (int i = 0; i < 1000; i++) {
        ASYNC_LOG_INFO("High-volume test message");
    }
    end = clock();
    double high_vol_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    async_logger_get_stats(logger, &processed, &dropped, &full_events);
    printf("   Time to send 1000 messages: %.3f seconds\n", high_vol_time);
    printf("   Messages processed: %lu\n", processed);
    printf("   Messages dropped: %lu\n", dropped);
    printf("   Messages per second: %.0f\n\n", 1000.0 / high_vol_time);
    
    // Test 6: Mixed sync/async operations
    printf("6. Testing mixed sync/async operations:\n");
    LOG_INFO("Synchronous message during async mode");
    ASYNC_LOG_INFO("Asynchronous message during async mode");
    print_info("Direct print function");
    print_info_async("Async print function");
    printf("   Mixed operations completed\n\n");
    
    // Flush remaining messages
    printf("7. Flushing remaining messages...\n");
    async_logger_flush(logger);
    printf("   Flush completed\n\n");
    
    // Final statistics
    async_logger_get_stats(logger, &processed, &dropped, &full_events);
    printf("=== Final Statistics ===\n");
    printf("Total messages processed: %lu\n", processed);
    printf("Total messages dropped: %lu\n", dropped);
    printf("Total queue full events: %lu\n", full_events);
    
    // Cleanup
    cleanup_smart_logger(async_logger);
    
    printf("\n=== Async Logging Test Completed ===\n");
    return 0;
}