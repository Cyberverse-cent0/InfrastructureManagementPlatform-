/**
 * @file config_parser.h
 * @brief Configuration parser for inventory service
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Configuration parser that reads from config files and environment variables
 * for dynamic service configuration support
 */

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Service configuration structure
 */
typedef struct {
    // Server settings
    int server_port;
    char server_host[256];
    int max_threads;
    int backlog;
    
    // Service discovery settings
    bool service_discovery_enabled;
    char service_discovery_host[256];
    int service_discovery_port;
    char config_file[512];
    
    // Database settings
    char database_host[256];
    int database_port;
    char database_name[256];
    char database_user[256];
    char database_password[256];
    
    // Service settings
    char service_name[256];
    char service_version[64];
    int max_assets;
    
    // API settings
    int api_timeout;
    int max_request_size;
    
    // CORS settings
    bool cors_enabled;
    char cors_origin[256];
    char cors_methods[256];
    char cors_headers[256];
    
    // Logging settings
    char log_level[64];
    char log_file[512];
} service_config_t;

/**
 * @brief Initialize configuration with default values
 * @param config Configuration structure to initialize
 */
void config_init_defaults(service_config_t *config);

/**
 * @brief Load configuration from file
 * @param config_file Path to configuration file
 * @param config Configuration structure to populate
 * @return true if successful, false otherwise
 */
bool config_load_from_file(const char *config_file, service_config_t *config);

/**
 * @brief Load configuration from environment variables
 * @param config Configuration structure to populate
 * @return true if successful, false otherwise
 */
bool config_load_from_env(service_config_t *config);

/**
 * @brief Parse configuration file line
 * @param line Configuration file line
 * @param config Configuration structure to populate
 * @return true if successful, false otherwise
 */
bool config_parse_line(const char *line, service_config_t *config);

/**
 * @brief Get environment variable with fallback
 * @param env_var Environment variable name
 * @param fallback Fallback value if not set
 * @param buffer Buffer to store result
 * @param buffer_size Size of buffer
 * @return true if environment variable was set, false otherwise
 */
bool config_get_env(const char *env_var, const char *fallback, char *buffer, size_t buffer_size);

/**
 * @brief Get integer environment variable with fallback
 * @param env_var Environment variable name
 * @param fallback Fallback value if not set
 * @return Integer value
 */
int config_get_env_int(const char *env_var, int fallback);

/**
 * @brief Get boolean environment variable with fallback
 * @param env_var Environment variable name
 * @param fallback Fallback value if not set
 * @return Boolean value
 */
bool config_get_env_bool(const char *env_var, bool fallback);

/**
 * @brief Print current configuration
 * @param config Configuration structure to print
 */
void config_print(const service_config_t *config);

/**
 * @brief Validate configuration
 * @param config Configuration structure to validate
 * @return true if valid, false otherwise
 */
bool config_validate(const service_config_t *config);

#endif // CONFIG_PARSER_H