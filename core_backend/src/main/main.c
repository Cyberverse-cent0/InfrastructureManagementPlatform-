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
#include <string.h>
#include <stdbool.h>
#include "network/http_server.h"
// #include "database/global_lib.h"  // Disabled until PostgreSQL is available

#define DEFAULT_PORT 7234
#define CONFIG_FILE "../../config/platform_config.json"

static volatile int running = 1;

void signal_handler(int sig) {
    (void)sig;
    running = 0;
    printf("\nShutdown signal received\n");
}

// Simple configuration parser for core backend
typedef struct {
    int port;
    char host[256];
    char database_host[256];
    int database_port;
    char database_name[256];
    char database_user[256];
    char service_discovery_host[256];
    int service_discovery_port;
    bool service_discovery_enabled;
} core_config_t;

void load_core_config(core_config_t *config, const char *config_file) {
    // Initialize defaults
    config->port = DEFAULT_PORT;
    strncpy(config->host, "0.0.0.0", sizeof(config->host) - 1);
    strncpy(config->database_host, "localhost", sizeof(config->database_host) - 1);
    config->database_port = 5432;
    strncpy(config->database_name, "infrastructure_platform", sizeof(config->database_name) - 1);
    strncpy(config->database_user, "infrastructure_user", sizeof(config->database_user) - 1);
    strncpy(config->service_discovery_host, "localhost", sizeof(config->service_discovery_host) - 1);
    config->service_discovery_port = 8000;
    config->service_discovery_enabled = true;
    
    // Load from environment variables
    const char *env_port = getenv("CORE_BACKEND_PORT");
    if (env_port) config->port = atoi(env_port);
    
    const char *env_host = getenv("CORE_BACKEND_HOST");
    if (env_host) strncpy(config->host, env_host, sizeof(config->host) - 1);
    
    const char *env_db_host = getenv("DATABASE_HOST");
    if (env_db_host) strncpy(config->database_host, env_db_host, sizeof(config->database_host) - 1);
    
    const char *env_db_port = getenv("DATABASE_PORT");
    if (env_db_port) config->database_port = atoi(env_db_port);
    
    const char *env_sd_enabled = getenv("SERVICE_DISCOVERY_ENABLED");
    if (env_sd_enabled) {
        config->service_discovery_enabled = (strcmp(env_sd_enabled, "true") == 0);
    }
}

int main(int argc, char *argv[]) {
    core_config_t config;
    const char *config_file = CONFIG_FILE;
    
    // Parse command line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "--config") == 0 && argc > 2) {
            config_file = argv[2];
        } else {
            config.port = atoi(argv[1]);
            if (config.port <= 0 || config.port > 65535) {
                fprintf(stderr, "Invalid port number: %s\n", argv[1]);
                return EXIT_FAILURE;
            }
        }
    }
    
    // Load configuration
    load_core_config(&config, config_file);
    
    printf("Infrastructure Management Platform - Core Backend\n");
    printf("Configuration:\n");
    printf("  Host: %s\n", config.host);
    printf("  Port: %d\n", config.port);
    printf("  Database: %s:%d/%s\n", config.database_host, config.database_port, config.database_name);
    printf("  Service Discovery: %s\n", config.service_discovery_enabled ? "enabled" : "disabled");
    printf("Starting on port %d\n", config.port);
    
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
    
    // TODO: Register with service discovery if enabled
    if (config.service_discovery_enabled) {
        printf("Service discovery enabled - would register with %s:%d\n",
               config.service_discovery_host, config.service_discovery_port);
        // TODO: Implement service registration
    }
    
    // TODO: Initialize HTTP server with routes
    // TODO: Setup API handlers
    // TODO: Start server loop
    
    printf("Core backend running on port %d\n", config.port);
    printf("Press Ctrl+C to stop\n");
    
    // Main server loop
    while (running) {
        // TODO: Process server events
        usleep(100000); // 100ms
    }
    
    printf("Shutting down core backend...\n");
    
    // TODO: Unregister from service discovery
    if (config.service_discovery_enabled) {
        printf("Would unregister from service discovery\n");
    }
    
    // TODO: Cleanup database connection
    // database_disconnect(database);
    // TODO: Cleanup HTTP server
    
    printf("Core backend stopped\n");
    return EXIT_SUCCESS;
}