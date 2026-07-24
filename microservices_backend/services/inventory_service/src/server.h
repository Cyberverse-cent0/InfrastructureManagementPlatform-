/**
 * @file server.h
 * @brief HTTP Server Interface for Inventory Service
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * HTTP server interface and structures for the inventory microservice.
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

/**
 * @brief Server context structure
 * 
 * Contains all server state and configuration.
 */
typedef struct server_context server_context_t;

/**
 * @brief Initialize the HTTP server
 * 
 * @param port Port number to listen on
 * @return Server context pointer, or NULL on failure
 */
server_context_t* server_init(int port);

/**
 * @brief Run a single server iteration
 * 
 * @param server Server context
 * @return 0 on success, -1 on error
 */
int server_run_once(server_context_t *server);

/**
 * @brief Cleanup server resources
 * 
 * @param server Server context to cleanup
 */
void server_cleanup(server_context_t *server);

/**
 * @brief Check if server is running
 * 
 * @param server Server context
 * @return true if running, false otherwise
 */
bool server_is_running(const server_context_t *server);

/**
 * @brief Send HTTP response
 * 
 * @param socket Socket to send response to
 * @param status_code HTTP status code
 * @param content_type Content type header
 * @param body Response body
 */
void send_http_response(int socket, const char *status_code, 
                        const char *content_type, const char *body);

#endif // SERVER_H