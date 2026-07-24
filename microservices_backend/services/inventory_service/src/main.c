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
#include <string.h>
#include "server.h"
#include "config_parser.h"

#define DEFAULT_PORT 8080
#define CONFIG_FILE "../config/service.conf"

static volatile int running = 1;

void signal_handler(int sig) {
    (void)sig;
    running = 0;
}

int main(int argc, char *argv[]) {
    service_config_t config;
    const char *config_file = CONFIG_FILE;
    
    // Initialize with defaults
    config_init_defaults(&config);
    
    // Parse command line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "--config") == 0 && argc > 2) {
            config_file = argv[2];
        } else {
            config.server_port = atoi(argv[1]);
            if (config.server_port <= 0 || config.server_port > 65535) {
                fprintf(stderr, "Invalid port number: %s\n", argv[1]);
                return EXIT_FAILURE;
            }
        }
    }
    
    // Load configuration from file
    if (config_load_from_file(config_file, &config)) {
        printf("Loaded configuration from: %s\n", config_file);
    } else {
        printf("Using default configuration\n");
    }
    
    // Load configuration from environment variables (overrides file)
    config_load_from_env(&config);
    
    // Validate configuration
    if (!config_validate(&config)) {
        fprintf(stderr, "Invalid configuration\n");
        return EXIT_FAILURE;
    }
    
    // Print configuration
    config_print(&config);
    
    printf("Inventory Microservice starting on port %d\n", config.server_port);
    
    // Setup signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // TODO: Register with service discovery if enabled
    if (config.service_discovery_enabled) {
        printf("Service discovery enabled - would register with %s:%d\n",
               config.service_discovery_host, config.service_discovery_port);
        // TODO: Implement service registration
    }
    
    // Initialize and start server
    server_context_t *server = server_init(config.server_port);
    if (server == NULL) {
        fprintf(stderr, "Failed to initialize server\n");
        return EXIT_FAILURE;
    }
    
    printf("Inventory Microservice running on port %d\n", config.server_port);
    printf("Service endpoints:\n");
    printf("  - http://%s:%d/health\n", config.server_host, config.server_port);
    printf("  - http://%s:%d/api/assets\n", config.server_host, config.server_port);
    printf("  - http://%s:%d/api/stats\n", config.server_host, config.server_port);
    printf("Press Ctrl+C to stop\n");
    
    // Main server loop
    while (running) {
        server_run_once(server);
        usleep(100000); // 100ms
    }
    
    printf("\nShutting down Inventory Microservice...\n");
    
    // TODO: Unregister from service discovery
    if (config.service_discovery_enabled) {
        printf("Would unregister from service discovery\n");
    }
    
    server_cleanup(server);
    
    printf("Inventory Microservice stopped\n");
    return EXIT_SUCCESS;
}