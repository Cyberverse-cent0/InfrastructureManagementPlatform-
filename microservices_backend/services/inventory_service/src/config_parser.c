/**
 * @file config_parser.c
 * @brief Configuration parser implementation for inventory service
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 */

#include "config_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

void config_init_defaults(service_config_t *config) {
    // Server settings
    config->server_port = 8080;
    strncpy(config->server_host, "0.0.0.0", sizeof(config->server_host) - 1);
    config->max_threads = 10;
    config->backlog = 10;
    
    // Service discovery settings
    config->service_discovery_enabled = true;
    strncpy(config->service_discovery_host, "localhost", sizeof(config->service_discovery_host) - 1);
    config->service_discovery_port = 8000;
    strncpy(config->config_file, "../../config/platform_config.json", sizeof(config->config_file) - 1);
    
    // Database settings
    strncpy(config->database_host, "localhost", sizeof(config->database_host) - 1);
    config->database_port = 5432;
    strncpy(config->database_name, "infrastructure_platform", sizeof(config->database_name) - 1);
    strncpy(config->database_user, "infrastructure_user", sizeof(config->database_user) - 1);
    strncpy(config->database_password, "change_me", sizeof(config->database_password) - 1);
    
    // Service settings
    strncpy(config->service_name, "inventory-microservice", sizeof(config->service_name) - 1);
    strncpy(config->service_version, "1.0.0", sizeof(config->service_version) - 1);
    config->max_assets = 1000;
    
    // API settings
    config->api_timeout = 30;
    config->max_request_size = 10485760;
    
    // CORS settings
    config->cors_enabled = true;
    strncpy(config->cors_origin, "*", sizeof(config->cors_origin) - 1);
    strncpy(config->cors_methods, "GET,POST,PUT,DELETE,OPTIONS", sizeof(config->cors_methods) - 1);
    strncpy(config->cors_headers, "Content-Type,Authorization", sizeof(config->cors_headers) - 1);
    
    // Logging settings
    strncpy(config->log_level, "INFO", sizeof(config->log_level) - 1);
    strncpy(config->log_file, "/var/log/inventory_service.log", sizeof(config->log_file) - 1);
}

bool config_load_from_file(const char *config_file, service_config_t *config) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        fprintf(stderr, "Could not open config file: %s\n", config_file);
        return false;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\r\n")] = '\0';
        
        // Skip empty lines and comments
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }
        
        config_parse_line(line, config);
    }
    
    fclose(file);
    return true;
}

bool config_parse_line(const char *line, service_config_t *config) {
    char key[256];
    char value[512];
    
    // Parse KEY=VALUE format
    if (sscanf(line, "%255[^=]=%511[^\n]", key, value) != 2) {
        return false;
    }
    
    // Trim whitespace
    char *key_end = key + strlen(key) - 1;
    while (key_end > key && isspace(*key_end)) {
        *key_end = '\0';
        key_end--;
    }
    
    char *value_start = value;
    while (*value_start && isspace(*value_start)) {
        value_start++;
    }
    
    // Server settings
    if (strcmp(key, "SERVER_PORT") == 0) {
        config->server_port = atoi(value_start);
    } else if (strcmp(key, "SERVER_HOST") == 0) {
        strncpy(config->server_host, value_start, sizeof(config->server_host) - 1);
    } else if (strcmp(key, "MAX_THREADS") == 0) {
        config->max_threads = atoi(value_start);
    } else if (strcmp(key, "BACKLOG") == 0) {
        config->backlog = atoi(value_start);
    }
    // Service discovery settings
    else if (strcmp(key, "SERVICE_DISCOVERY_ENABLED") == 0) {
        config->service_discovery_enabled = (strcasecmp(value_start, "true") == 0);
    } else if (strcmp(key, "SERVICE_DISCOVERY_HOST") == 0) {
        strncpy(config->service_discovery_host, value_start, sizeof(config->service_discovery_host) - 1);
    } else if (strcmp(key, "SERVICE_DISCOVERY_PORT") == 0) {
        config->service_discovery_port = atoi(value_start);
    } else if (strcmp(key, "CONFIG_FILE") == 0) {
        strncpy(config->config_file, value_start, sizeof(config->config_file) - 1);
    }
    // Database settings
    else if (strcmp(key, "DATABASE_HOST") == 0) {
        strncpy(config->database_host, value_start, sizeof(config->database_host) - 1);
    } else if (strcmp(key, "DATABASE_PORT") == 0) {
        config->database_port = atoi(value_start);
    } else if (strcmp(key, "DATABASE_NAME") == 0) {
        strncpy(config->database_name, value_start, sizeof(config->database_name) - 1);
    } else if (strcmp(key, "DATABASE_USER") == 0) {
        strncpy(config->database_user, value_start, sizeof(config->database_user) - 1);
    } else if (strcmp(key, "DATABASE_PASSWORD") == 0) {
        strncpy(config->database_password, value_start, sizeof(config->database_password) - 1);
    }
    // Service settings
    else if (strcmp(key, "SERVICE_NAME") == 0) {
        strncpy(config->service_name, value_start, sizeof(config->service_name) - 1);
    } else if (strcmp(key, "SERVICE_VERSION") == 0) {
        strncpy(config->service_version, value_start, sizeof(config->service_version) - 1);
    } else if (strcmp(key, "MAX_ASSETS") == 0) {
        config->max_assets = atoi(value_start);
    }
    // API settings
    else if (strcmp(key, "API_TIMEOUT") == 0) {
        config->api_timeout = atoi(value_start);
    } else if (strcmp(key, "MAX_REQUEST_SIZE") == 0) {
        config->max_request_size = atoi(value_start);
    }
    // CORS settings
    else if (strcmp(key, "CORS_ENABLED") == 0) {
        config->cors_enabled = (strcasecmp(value_start, "true") == 0);
    } else if (strcmp(key, "CORS_ORIGIN") == 0) {
        strncpy(config->cors_origin, value_start, sizeof(config->cors_origin) - 1);
    } else if (strcmp(key, "CORS_METHODS") == 0) {
        strncpy(config->cors_methods, value_start, sizeof(config->cors_methods) - 1);
    } else if (strcmp(key, "CORS_HEADERS") == 0) {
        strncpy(config->cors_headers, value_start, sizeof(config->cors_headers) - 1);
    }
    // Logging settings
    else if (strcmp(key, "LOG_LEVEL") == 0) {
        strncpy(config->log_level, value_start, sizeof(config->log_level) - 1);
    } else if (strcmp(key, "LOG_FILE") == 0) {
        strncpy(config->log_file, value_start, sizeof(config->log_file) - 1);
    }
    
    return true;
}

bool config_load_from_env(service_config_t *config) {
    // Server settings
    config->server_port = config_get_env_int("SERVER_PORT", config->server_port);
    config_get_env("SERVER_HOST", config->server_host, config->server_host, sizeof(config->server_host));
    config->max_threads = config_get_env_int("MAX_THREADS", config->max_threads);
    config->backlog = config_get_env_int("BACKLOG", config->backlog);
    
    // Service discovery settings
    config->service_discovery_enabled = config_get_env_bool("SERVICE_DISCOVERY_ENABLED", config->service_discovery_enabled);
    config_get_env("SERVICE_DISCOVERY_HOST", config->service_discovery_host, config->service_discovery_host, sizeof(config->service_discovery_host));
    config->service_discovery_port = config_get_env_int("SERVICE_DISCOVERY_PORT", config->service_discovery_port);
    config_get_env("CONFIG_FILE", config->config_file, config->config_file, sizeof(config->config_file));
    
    // Service-specific overrides
    config_get_env("INVENTORY_SERVICE_HOST", config->server_host, config->server_host, sizeof(config->server_host));
    config->server_port = config_get_env_int("INVENTORY_SERVICE_PORT", config->server_port);
    
    // Database settings
    config_get_env("DATABASE_HOST", config->database_host, config->database_host, sizeof(config->database_host));
    config->database_port = config_get_env_int("DATABASE_PORT", config->database_port);
    config_get_env("DATABASE_NAME", config->database_name, config->database_name, sizeof(config->database_name));
    config_get_env("DATABASE_USER", config->database_user, config->database_user, sizeof(config->database_user));
    config_get_env("DATABASE_PASSWORD", config->database_password, config->database_password, sizeof(config->database_password));
    
    // Service settings
    config_get_env("SERVICE_NAME", config->service_name, config->service_name, sizeof(config->service_name));
    config_get_env("SERVICE_VERSION", config->service_version, config->service_version, sizeof(config->service_version));
    config->max_assets = config_get_env_int("MAX_ASSETS", config->max_assets);
    
    // API settings
    config->api_timeout = config_get_env_int("API_TIMEOUT", config->api_timeout);
    config->max_request_size = config_get_env_int("MAX_REQUEST_SIZE", config->max_request_size);
    
    // CORS settings
    config->cors_enabled = config_get_env_bool("CORS_ENABLED", config->cors_enabled);
    config_get_env("CORS_ORIGIN", config->cors_origin, config->cors_origin, sizeof(config->cors_origin));
    config_get_env("CORS_METHODS", config->cors_methods, config->cors_methods, sizeof(config->cors_methods));
    config_get_env("CORS_HEADERS", config->cors_headers, config->cors_headers, sizeof(config->cors_headers));
    
    // Logging settings
    config_get_env("LOG_LEVEL", config->log_level, config->log_level, sizeof(config->log_level));
    config_get_env("LOG_FILE", config->log_file, config->log_file, sizeof(config->log_file));
    
    return true;
}

bool config_get_env(const char *env_var, const char *fallback, char *buffer, size_t buffer_size) {
    const char *env_value = getenv(env_var);
    if (env_value) {
        strncpy(buffer, env_value, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
        return true;
    }
    strncpy(buffer, fallback, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
    return false;
}

int config_get_env_int(const char *env_var, int fallback) {
    const char *env_value = getenv(env_var);
    if (env_value) {
        return atoi(env_value);
    }
    return fallback;
}

bool config_get_env_bool(const char *env_var, bool fallback) {
    const char *env_value = getenv(env_var);
    if (env_value) {
        return (strcasecmp(env_value, "true") == 0 || strcasecmp(env_value, "1") == 0);
    }
    return fallback;
}

void config_print(const service_config_t *config) {
    printf("=== Service Configuration ===\n");
    printf("Server Settings:\n");
    printf("  Host: %s\n", config->server_host);
    printf("  Port: %d\n", config->server_port);
    printf("  Max Threads: %d\n", config->max_threads);
    printf("  Backlog: %d\n", config->backlog);
    
    printf("\nService Discovery Settings:\n");
    printf("  Enabled: %s\n", config->service_discovery_enabled ? "true" : "false");
    printf("  Discovery Host: %s\n", config->service_discovery_host);
    printf("  Discovery Port: %d\n", config->service_discovery_port);
    printf("  Config File: %s\n", config->config_file);
    
    printf("\nDatabase Settings:\n");
    printf("  Host: %s\n", config->database_host);
    printf("  Port: %d\n", config->database_port);
    printf("  Name: %s\n", config->database_name);
    printf("  User: %s\n", config->database_user);
    
    printf("\nService Settings:\n");
    printf("  Name: %s\n", config->service_name);
    printf("  Version: %s\n", config->service_version);
    printf("  Max Assets: %d\n", config->max_assets);
    
    printf("\nAPI Settings:\n");
    printf("  Timeout: %d\n", config->api_timeout);
    printf("  Max Request Size: %d\n", config->max_request_size);
    
    printf("\nCORS Settings:\n");
    printf("  Enabled: %s\n", config->cors_enabled ? "true" : "false");
    printf("  Origin: %s\n", config->cors_origin);
    printf("  Methods: %s\n", config->cors_methods);
    printf("  Headers: %s\n", config->cors_headers);
    
    printf("\nLogging Settings:\n");
    printf("  Level: %s\n", config->log_level);
    printf("  File: %s\n", config->log_file);
    printf("============================\n");
}

bool config_validate(const service_config_t *config) {
    bool valid = true;
    
    if (config->server_port <= 0 || config->server_port > 65535) {
        fprintf(stderr, "Invalid server port: %d\n", config->server_port);
        valid = false;
    }
    
    if (config->max_threads <= 0) {
        fprintf(stderr, "Invalid max threads: %d\n", config->max_threads);
        valid = false;
    }
    
    if (config->database_port <= 0 || config->database_port > 65535) {
        fprintf(stderr, "Invalid database port: %d\n", config->database_port);
        valid = false;
    }
    
    if (strlen(config->service_name) == 0) {
        fprintf(stderr, "Service name cannot be empty\n");
        valid = false;
    }
    
    return valid;
}