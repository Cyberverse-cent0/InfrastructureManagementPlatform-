#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "src/backend/core/http_server.h"

int main() {
    printf("=== Server Logging Test ===\n\n");
    
    // Test 1: Initialize and start logging
    log_server_work_t log_work;
    log_server_work_init(&log_work);
    
    printf("Test 1: Initialize logging\n");
    printf("  Client IP: %s\n", log_work.client_ip);
    printf("  Time start: %ld\n", log_work.time_start);
    printf("  Time end: %ld\n", log_work.time_end);
    printf("  Time taken: %ldms\n", log_work.time_taken_ms);
    printf("  Payload size: %zu\n", log_work.payload_size);
    printf("  Success: %s\n", log_work.is_success ? "Yes" : "No");
    printf("\n");
    
    // Test 2: Start logging with client IP
    printf("Test 2: Start logging with client IP\n");
    log_server_work_start(&log_work, "192.168.1.100");
    printf("  Client IP: %s\n", log_work.client_ip);
    printf("  Time start: %ld\n", log_work.time_start);
    printf("\n");
    
    // Test 3: Simulate some work
    printf("Test 3: Simulate work (sleeping for 1 second)\n");
    sleep(1);
    printf("\n");
    
    // Test 4: End logging
    printf("Test 4: End logging\n");
    log_server_work_end(&log_work, 1024, true);
    printf("  Time end: %ld\n", log_work.time_end);
    printf("  Time taken: %ldms\n", log_work.time_taken_ms);
    printf("  Payload size: %zu\n", log_work.payload_size);
    printf("  Success: %s\n", log_work.is_success ? "Yes" : "No");
    printf("\n");
    
    // Test 5: Print log
    printf("Test 5: Print log\n");
    log_server_work_print(&log_work);
    printf("\n");
    
    // Test 6: Convert to JSON
    printf("Test 6: Convert to JSON\n");
    char *json = log_server_work_to_json(&log_work);
    if (json != NULL) {
        printf("  JSON: %s\n", json);
        free(json);
    }
    printf("\n");
    
    // Test 7: Test failure case
    printf("Test 7: Test failure case\n");
    log_server_work_start(&log_work, "10.0.0.50");
    sleep(1);
    log_server_work_end(&log_work, 0, false);
    log_server_work_print(&log_work);
    printf("\n");
    
    // Test 8: Test NULL handling
    printf("Test 8: Test NULL handling\n");
    log_server_work_print(NULL);
    char *null_json = log_server_work_to_json(NULL);
    if (null_json != NULL) {
        printf("  NULL JSON: %s\n", null_json);
        free(null_json);
    }
    printf("\n");
    
    printf("=== Test Complete ===\n");
    
    return 0;
}