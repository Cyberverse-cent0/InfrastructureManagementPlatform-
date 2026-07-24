/**
 * @file stats_handlers.h
 * @brief Statistics HTTP Request Handlers
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * HTTP request handlers for statistics-related endpoints.
 */

#ifndef STATS_HANDLERS_H
#define STATS_HANDLERS_H

#include <stdbool.h>

/**
 * @brief Handle GET request for statistics
 * 
 * @param client_socket Client socket descriptor
 * @return true on success, false on failure
 */
bool handle_get_stats(int client_socket);

/**
 * @brief Handle GET request for specific statistics type
 * 
 * @param client_socket Client socket descriptor
 * @param stats_type Type of statistics requested
 * @return true on success, false on failure
 */
bool handle_get_stats_by_type(int client_socket, const char *stats_type);

#endif // STATS_HANDLERS_H