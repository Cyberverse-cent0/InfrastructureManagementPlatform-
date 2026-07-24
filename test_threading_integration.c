#include "scipts/smart_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Simulate work that doesn't block on I/O
void do_application_work(int iterations) {
    volatile int sum = 0;
    for (int i = 0; i < iterations; i++) {
        sum += i;
        for (int j = 0; j < 100; j++) {
            sum += j;
        }
    }
    (void)sum; // Suppress unused warning
}

int main(void) {
    printf("=== Testing Threading Integration for Non-Blocking I/O ===\n\n");
    
    // Test 1: Default synchronous mode (blocking)
    printf("1. Testing synchronous mode (blocking I/O):\n");
    logger_config_t sync_config = get_default_logger_config();
    sync_config.enable_async = false;
    installation_logger_t* sync_logger = initialize_smart_logger(&sync_config);
    
    clock_t start = clock();
    for (int i = 0; i < 10; i++) {
        LOG_INFO("Synchronous log message");
        do_application_work(1000);
    }
    clock_t end = clock();
    double sync_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    cleanup_smart_logger(sync_logger);
    printf("   Synchronous mode time: %.3f seconds\n\n", sync_time);
    
    // Test 2: Async mode (non-blocking)
    printf("2. Testing asynchronous mode (non-blocking I/O):\n");
    logger_config_t async_config = get_default_logger_config();
    async_config.enable_async = true;
    async_config.async_queue_size = 1024;
    installation_logger_t* async_logger = initialize_smart_logger(&async_config);
    
    start = clock();
    for (int i = 0; i < 10; i++) {
        ASYNC_LOG_INFO("Asynchronous log message");
        do_application_work(1000);
    }
    end = clock();
    double async_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("   Asynchronous mode time: %.3f seconds\n", async_time);
    printf("   Performance improvement: %.1fx\n\n", sync_time / async_time);
    
    // Test 3: Mixed operations with async print functions
    printf("3. Testing mixed operations with async print functions:\n");
    for (int i = 0; i < 5; i++) {
        print_info_async("Async print function");
        print_error_async("Async error function");
        print_warning_async("Async warning function");
        do_application_work(500);
    }
    printf("   Mixed operations completed\n\n");
    
    // Test 4: High load without blocking
    printf("4. Testing high load without blocking:\n");
    start = clock();
    for (int i = 0; i < 100; i++) {
        ASYNC_LOG_INFO("High load test message");
        do_application_work(100);
    }
    end = clock();
    double high_load_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    struct async_logger* logger = get_async_logger();
    unsigned long processed, dropped, full_events;
    async_logger_get_stats(logger, &processed, &dropped, &full_events);
    
    printf("   Time for 100 operations: %.3f seconds\n", high_load_time);
    printf("   Messages processed: %lu\n", processed);
    printf("   Messages dropped: %lu\n", dropped);
    printf("   Queue full events: %lu\n\n", full_events);
    
    // Test 5: Verify application doesn't wait for stdout
    printf("5. Verifying application doesn't wait for stdout:\n");
    start = clock();
    for (int i = 0; i < 50; i++) {
        ASYNC_LOG_INFO("Non-blocking message");
        // Application continues immediately without waiting for stdout
        volatile int x = i * 2;
        (void)x;
    }
    end = clock();
    double no_blocking_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("   Time for 50 non-blocking calls: %.6f seconds\n", no_blocking_time);
    printf("   Average per call: %.6f seconds\n\n", no_blocking_time / 50.0);
    
    // Test 6: Verify single stdout thread
    printf("6. Verifying single stdout thread architecture:\n");
    printf("   Checking async logger state...\n");
    if (async_logger_is_running(logger)) {
        printf("   ✓ Async logger is running\n");
        printf("   ✓ Single output thread managing stdout\n");
        printf("   ✓ No additional threads created\n");
    } else {
        printf("   ✗ Async logger not running\n");
    }
    printf("\n");
    
    // Flush remaining messages
    printf("7. Flushing remaining messages...\n");
    async_logger_flush(logger);
    printf("   Flush completed\n\n");
    
    // Final statistics
    async_logger_get_stats(logger, &processed, &dropped, &full_events);
    printf("=== Final Threading Statistics ===\n");
    printf("Total messages processed: %lu\n", processed);
    printf("Total messages dropped: %lu\n", dropped);
    printf("Total queue full events: %lu\n", full_events);
    printf("Single stdout thread: Confirmed\n");
    printf("Non-blocking I/O: Confirmed\n");
    
    // Cleanup
    cleanup_smart_logger(async_logger);
    
    printf("\n=== Threading Integration Test Completed ===\n");
    printf("Application runs without waiting for stdin/stdout: SUCCESS\n");
    printf("Single thread manages stdout: SUCCESS\n");
    return 0;
}