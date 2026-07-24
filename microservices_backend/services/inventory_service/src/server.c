/**
 * @file server.c
 * @brief HTTP Server Implementation for Inventory Service
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * HTTP server implementation for the inventory microservice.
 * Handles HTTP requests and routes them to appropriate handlers.
 */

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define MAX_THREADS 10
#define BACKLOG 10

/**
 * @brief Server context structure
 */
struct server_context {
    int server_socket;
    int port;
    bool running;
    pthread_t thread_pool[MAX_THREADS];
    int active_threads;
};

typedef struct {
    int socket;
    struct sockaddr_in address;
    server_context_t *server;
} client_connection_t;

// Forward declarations
void *handle_client(void *arg);
void handle_get_request(int socket, const char *path);
void handle_options_request(int socket);

server_context_t* server_init(int port) {
    server_context_t *server = calloc(1, sizeof(server_context_t));
    if (server == NULL) {
        perror("Failed to allocate server context");
        return NULL;
    }
    
    server->port = port;
    server->running = true;
    
    // Create socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket == -1) {
        perror("Socket creation failed");
        free(server);
        return NULL;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(opt)) < 0) {
        perror("Set socket options failed");
        close(server->server_socket);
        free(server);
        return NULL;
    }
    
    // Configure server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    
    // Bind socket
    if (bind(server->server_socket, (struct sockaddr *)&server_address, 
             sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server->server_socket);
        free(server);
        return NULL;
    }
    
    // Listen for connections
    if (listen(server->server_socket, BACKLOG) < 0) {
        perror("Listen failed");
        close(server->server_socket);
        free(server);
        return NULL;
    }
    
    printf("Server initialized on port %d\n", port);
    return server;
}

int server_run_once(server_context_t *server) {
    if (!server || !server->running) {
        return -1;
    }
    
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    
    int client_socket = accept(server->server_socket, 
                               (struct sockaddr *)&client_address, 
                               &client_len);
    if (client_socket < 0) {
        if (errno == EINTR) {
            return 0; // Interrupted by signal
        }
        perror("Accept failed");
        return -1;
    }
    
    // Create client connection context
    client_connection_t *connection = calloc(1, sizeof(client_connection_t));
    if (connection == NULL) {
        perror("Failed to allocate client connection");
        close(client_socket);
        return -1;
    }
    
    connection->socket = client_socket;
    connection->address = client_address;
    connection->server = server;
    
    // Find available thread or handle directly
    if (server->active_threads < MAX_THREADS) {
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, connection) == 0) {
            pthread_detach(thread_id);
            server->active_threads++;
            return 0;
        }
    }
    
    // Handle directly if no threads available
    handle_client(connection);
    free(connection);
    
    return 0;
}

void server_cleanup(server_context_t *server) {
    if (!server) return;
    
    server->running = false;
    
    if (server->server_socket >= 0) {
        close(server->server_socket);
    }
    
    free(server);
}

bool server_is_running(const server_context_t *server) {
    return server ? server->running : false;
}

void *handle_client(void *arg) {
    client_connection_t *connection = (client_connection_t *)arg;
    int client_socket = connection->socket;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    printf("New client connected from %s\n", 
           inet_ntoa(connection->address.sin_addr));
    
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        
        // Parse HTTP request
        char method[16], path[256];
        sscanf(buffer, "%s %s", method, path);
        
        printf("Method: %s, Path: %s\n", method, path);
        
        if (strcmp(method, "GET") == 0) {
            handle_get_request(client_socket, path);
        } else if (strcmp(method, "OPTIONS") == 0) {
            handle_options_request(client_socket);
        } else {
            const char *not_allowed = "{\"error\":\"Method not allowed\",\"success\":false}";
            send_http_response(client_socket, "405 Method Not Allowed", 
                            "application/json", not_allowed);
        }
    }
    
    close(client_socket);
    if (connection->server) {
        connection->server->active_threads--;
    }
    free(connection);
    
    printf("Client disconnected\n");
    return NULL;
}

void send_http_response(int socket, const char *status_code, 
                        const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    int body_length = strlen(body);
    
    snprintf(response, sizeof(response),
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "\r\n"
        "%s",
        status_code, content_type, body_length, body);
    
    send(socket, response, strlen(response), 0);
}

void handle_options_request(int socket) {
    const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    send(socket, response, strlen(response), 0);
}

void handle_get_request(int socket, const char *path) {
    printf("Handling GET request for: %s\n", path);
    
    if (strcmp(path, "/") == 0 || strcmp(path, "/health") == 0) {
        const char *response = "{\"status\":\"ok\",\"service\":\"inventory-microservice\"}";
        send_http_response(socket, "200 OK", "application/json", response);
    }
    else if (strcmp(path, "/api/assets") == 0) {
        // This would call the asset handler in production
        const char *response = "{\"error\":\"Asset handler not yet implemented\",\"success\":false}";
        send_http_response(socket, "501 Not Implemented", "application/json", response);
    }
    else if (strcmp(path, "/api/stats") == 0) {
        // This would call the stats handler in production
        const char *response = "{\"error\":\"Stats handler not yet implemented\",\"success\":false}";
        send_http_response(socket, "501 Not Implemented", "application/json", response);
    }
    else {
        const char *not_found = "{\"error\":\"Endpoint not found\",\"success\":false}";
        send_http_response(socket, "404 Not Found", "application/json", not_found);
    }
}