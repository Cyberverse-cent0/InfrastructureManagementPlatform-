/**
 * @file main.c
 * @brief Core Backend Main Entry Point
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Main entry point for the core infrastructure management backend.
 * Initializes all components and starts the HTTP server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "network/http_server.h"
// #include "database/global_lib.h"  // Disabled until PostgreSQL is available

#define DEFAULT_PORT 7234
#define CONFIG_FILE "../config/server.conf"

static volatile int running = 1;

void signal_handler(int sig) {
    (void)sig;
    running = 0;
    printf("\nShutdown signal received\n");
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
    
    printf("Infrastructure Management Platform - Core Backend\n");
    printf("Starting on port %d\n", port);
    
    // Setup signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // TODO: Initialize database connection when PostgreSQL is available
    // const char *connection_string = "host=localhost port=5432 dbname=infrastructure_platform user=infrastructure_user";
    // database_t *database = database_init(connection_string);
    // if (database == NULL) {
    //     fprintf(stderr, "Failed to initialize database connection\n");
    //     return EXIT_FAILURE;
    // }
    
    // printf("Database connection established\n");
    
    // TODO: Initialize HTTP server with routes
    // TODO: Setup API handlers
    // TODO: Start server loop
    
    printf("Core backend running on port %d\n", port);
    printf("Press Ctrl+C to stop\n");
    
    // Main server loop
    while (running) {
        // TODO: Process server events
        usleep(100000); // 100ms
    }
    
    printf("Shutting down core backend...\n");
    
    // TODO: Cleanup database connection
    // database_disconnect(database);
    // TODO: Cleanup HTTP server
    
    printf("Core backend stopped\n");
    return EXIT_SUCCESS;
}