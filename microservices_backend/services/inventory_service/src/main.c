/**
 * @file main.c
 * @brief Inventory Service Entry Point
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Main entry point for the inventory microservice.
 * Initializes the HTTP server and handles service lifecycle.
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "server.h"

#define DEFAULT_PORT 8080
#define CONFIG_FILE "../config/service.conf"

static volatile int running = 1;

void signal_handler(int sig) {
    (void)sig;
    running = 0;
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    // Parse command line arguments
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    
    printf("Inventory Microservice starting on port %d\n", port);
    
    // Setup signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize and start server
    server_context_t *server = server_init(port);
    if (server == NULL) {
        fprintf(stderr, "Failed to initialize server\n");
        return EXIT_FAILURE;
    }
    
    printf("Inventory Microservice running on port %d\n", port);
    printf("Press Ctrl+C to stop\n");
    
    // Main server loop
    while (running) {
        server_run_once(server);
        usleep(100000); // 100ms
    }
    
    printf("\nShutting down Inventory Microservice...\n");
    server_cleanup(server);
    
    printf("Inventory Microservice stopped\n");
    return EXIT_SUCCESS;
}