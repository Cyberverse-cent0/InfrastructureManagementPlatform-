#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_THREADS 10

typedef struct {
    int socket;
    struct sockaddr_in address;
} ClientConnection;

// Mock data for the microservice
const char *mock_assets = 
    "{"
    "  \"data\": ["
    "    {"
    "      \"id\": \"1\","
    "      \"name\": \"Web Server 01\","
    "      \"type\": \"server\","
    "      \"status\": \"active\","
    "      \"priority\": \"high\","
    "      \"location\": \"US-East-1\","
    "      \"cpu\": 4,"
    "      \"memory\": 16,"
    "      \"storage\": 500,"
    "      \"ipAddress\": \"192.168.1.100\","
    "      \"macAddress\": \"00:1A:2B:3C:4D:5E\","
    "      \"operatingSystem\": \"Ubuntu 22.04\","
    "      \"createdAt\": \"2024-01-15T00:00:00Z\","
    "      \"updatedAt\": \"2024-07-20T00:00:00Z\","
    "      \"lastChecked\": \"2024-07-24T00:00:00Z\","
    "      \"tags\": [\"web\", \"production\"],"
    "      \"notes\": \"Main web server for production traffic\""
    "    },"
    "    {"
    "      \"id\": \"2\","
    "      \"name\": \"Database Server 01\","
    "      \"type\": \"database\","
    "      \"status\": \"active\","
    "      \"priority\": \"critical\","
    "      \"location\": \"US-East-1\","
    "      \"cpu\": 8,"
    "      \"memory\": 32,"
    "      \"storage\": 1000,"
    "      \"ipAddress\": \"192.168.1.101\","
    "      \"macAddress\": \"00:1A:2B:3C:4D:5F\","
    "      \"operatingSystem\": \"CentOS 8\","
    "      \"createdAt\": \"2024-02-01T00:00:00Z\","
    "      \"updatedAt\": \"2024-07-22T00:00:00Z\","
    "      \"lastChecked\": \"2024-07-24T00:00:00Z\","
    "      \"tags\": [\"database\", \"production\", \"critical\"],"
    "      \"notes\": \"Primary PostgreSQL database server\""
    "    }"
    "  ],"
    "  \"pagination\": {"
    "    \"page\": 1,"
    "    \"limit\": 10,"
    "    \"total\": 2"
    "  },"
    "  \"success\": true"
    "}";

const char *mock_stats = 
    "{"
    "  \"totalAssets\": 2,"
    "  \"activeAssets\": 2,"
    "  \"inactiveAssets\": 0,"
    "  \"maintenanceAssets\": 0,"
    "  \"errorAssets\": 0,"
    "  \"totalServers\": 1,"
    "  \"totalNetwork\": 0,"
    "  \"totalStorage\": 0,"
    "  \"totalDatabases\": 1,"
    "  \"avgCpuUsage\": 65.5,"
    "  \"avgMemoryUsage\": 72.3,"
    "  \"uptime\": 99.9,"
    "  \"success\": true"
    "}";

void send_http_response(int client_socket, const char *status_code, const char *content_type, const char *body) {
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
    
    send(client_socket, response, strlen(response), 0);
}

void handle_options_request(int client_socket) {
    const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    send(client_socket, response, strlen(response), 0);
}

void handle_get_request(int client_socket, const char *path) {
    printf("Handling GET request for: %s\n", path);
    
    if (strcmp(path, "/") == 0 || strcmp(path, "/health") == 0) {
        const char *response = "{\"status\":\"ok\",\"service\":\"inventory-microservice\"}";
        send_http_response(client_socket, "200 OK", "application/json", response);
    }
    else if (strcmp(path, "/api/assets") == 0) {
        send_http_response(client_socket, "200 OK", "application/json", mock_assets);
    }
    else if (strcmp(path, "/api/stats") == 0) {
        send_http_response(client_socket, "200 OK", "application/json", mock_stats);
    }
    else if (strncmp(path, "/api/assets/", 12) == 0) {
        // Extract asset ID
        const char *asset_id = path + 12;
        printf("Request for asset ID: %s\n", asset_id);
        
        // For simplicity, return first asset if ID is "1"
        if (strcmp(asset_id, "1") == 0) {
            const char *single_asset = 
                "{"
                "  \"id\": \"1\","
                "  \"name\": \"Web Server 01\","
                "  \"type\": \"server\","
                "  \"status\": \"active\","
                "  \"priority\": \"high\","
                "  \"location\": \"US-East-1\","
                "  \"cpu\": 4,"
                "  \"memory\": 16,"
                "  \"storage\": 500,"
                "  \"ipAddress\": \"192.168.1.100\","
                "  \"macAddress\": \"00:1A:2B:3C:4D:5E\","
                "  \"operatingSystem\": \"Ubuntu 22.04\","
                "  \"createdAt\": \"2024-01-15T00:00:00Z\","
                "  \"updatedAt\": \"2024-07-20T00:00:00Z\","
                "  \"lastChecked\": \"2024-07-24T00:00:00Z\","
                "  \"tags\": [\"web\", \"production\"],"
                "  \"notes\": \"Main web server for production traffic\","
                "  \"success\": true"
                "}";
            send_http_response(client_socket, "200 OK", "application/json", single_asset);
        } else {
            const char *not_found = "{\"error\":\"Asset not found\",\"success\":false}";
            send_http_response(client_socket, "404 Not Found", "application/json", not_found);
        }
    }
    else {
        const char *not_found = "{\"error\":\"Endpoint not found\",\"success\":false}";
        send_http_response(client_socket, "404 Not Found", "application/json", not_found);
    }
}

void *handle_client(void *arg) {
    ClientConnection *connection = (ClientConnection *)arg;
    int client_socket = connection->socket;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    printf("New client connected\n");

    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received request:\n%s\n", buffer);

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
            send_http_response(client_socket, "405 Method Not Allowed", "application/json", not_allowed);
        }
    }

    close(client_socket);
    free(connection);
    printf("Client disconnected\n");
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);
    pthread_t thread_id;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 10) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Inventory Microservice running on port %d\n", PORT);
    printf("Access endpoints:\n");
    printf("  - http://localhost:%d/health\n", PORT);
    printf("  - http://localhost:%d/api/assets\n", PORT);
    printf("  - http://localhost:%d/api/stats\n", PORT);

    // Accept and handle connections
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        printf("Connection from %s:%d\n", 
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));

        // Create client connection structure
        ClientConnection *connection = malloc(sizeof(ClientConnection));
        connection->socket = client_socket;
        connection->address = client_address;

        // Create thread to handle client
        if (pthread_create(&thread_id, NULL, handle_client, connection) != 0) {
            perror("Thread creation failed");
            close(client_socket);
            free(connection);
        } else {
            pthread_detach(thread_id);
        }
    }

    close(server_socket);
    return 0;
}
