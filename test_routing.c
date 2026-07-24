#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "src/backend/core/http_server.h"

// Sample route handlers
void home_handler(http_request_t *req, http_response_t *res) {
    (void)req; // Suppress unused parameter warning
    set_json_response(res, "{\"message\":\"Welcome to the API\"}");
}

void api_handler(http_request_t *req, http_response_t *res) {
    (void)req; // Suppress unused parameter warning
    set_json_response(res, "{\"message\":\"API endpoint\"}");
}

void interfaces_handler(http_request_t *req, http_response_t *res) {
    (void)req; // Suppress unused parameter warning
    set_json_response(res, "{\"interfaces\":[\"eth0\",\"wlan0\",\"lo\"]}");
}

void health_handler(http_request_t *req, http_response_t *res) {
    (void)req; // Suppress unused parameter warning
    res->status_code = 200;
    set_json_response(res, "{\"status\":\"healthy\",\"uptime\":12345}");
}

void test_basic_routing() {
    printf("=== Basic Routing Test ===\n\n");
    
    router_t *router = create_router();
    if (router == NULL) {
        printf("Failed to create router\n");
        return;
    }
    
    // Add routes
    add_route(router, "GET", "/", home_handler);
    add_route(router, "GET", "/api", api_handler);
    add_route(router, "GET", "/api/interfaces", interfaces_handler);
    add_route(router, "GET", "/health", health_handler);
    
    printf("Router created with %d routes\n\n", router->route_count);
    
    // Test route finding
    printf("Testing route finding:\n");
    
    route_handler handler;
    
    handler = find_route(router, "GET", "/");
    printf("  GET / -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/api");
    printf("  GET /api -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/api/interfaces");
    printf("  GET /api/interfaces -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/health");
    printf("  GET /health -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/nonexistent");
    printf("  GET /nonexistent -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "POST", "/");
    printf("  POST / -> %s\n", handler ? "Found" : "Not Found");
    
    printf("\n");
    
    destroy_router(router);
}

void test_wildcard_routing() {
    printf("=== Wildcard Routing Test ===\n\n");
    
    router_t *router = create_router();
    if (router == NULL) {
        printf("Failed to create router\n");
        return;
    }
    
    // Add wildcard routes
    add_route(router, "GET", "/api/*", api_handler);
    add_route(router, "*", "/health", health_handler);
    
    printf("Router created with wildcard routes\n\n");
    
    printf("Testing wildcard matching:\n");
    
    route_handler handler;
    
    handler = find_route(router, "GET", "/api/users");
    printf("  GET /api/users -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/api/anything");
    printf("  GET /api/anything -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/api");
    printf("  GET /api -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "POST", "/health");
    printf("  POST /health -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "GET", "/health");
    printf("  GET /health -> %s\n", handler ? "Found" : "Not Found");
    
    printf("\n");
    
    destroy_router(router);
}

void test_request_handling() {
    printf("=== Request Handling Test ===\n\n");
    
    router_t *router = create_router();
    if (router == NULL) {
        printf("Failed to create router\n");
        return;
    }
    
    add_route(router, "GET", "/", home_handler);
    add_route(router, "GET", "/api/interfaces", interfaces_handler);
    add_route(router, "GET", "/health", health_handler);
    
    // Create a mock request
    http_request_t request;
    memset(&request, 0, sizeof(request));
    strcpy(request.method, "GET");
    strcpy(request.path, "/api/interfaces");
    strcpy(request.version, "HTTP/1.1");
    
    // Create response
    http_response_t *response = create_http_response_ex(200);
    
    printf("Testing request handling for GET /api/interfaces:\n");
    handle_request_with_router(&request, response, router);
    
    printf("  Status code: %d\n", response->status_code);
    printf("  Body: %s\n", response->body ? response->body : "NULL");
    
    free_http_response(response);
    
    // Test 404
    strcpy(request.path, "/nonexistent");
    response = create_http_response_ex(200);
    
    printf("\nTesting request handling for GET /nonexistent:\n");
    handle_request_with_router(&request, response, router);
    
    printf("  Status code: %d\n", response->status_code);
    printf("  Body: %s\n", response->body ? response->body : "NULL");
    
    free_http_response(response);
    destroy_router(router);
    
    printf("\n");
}

void test_case_insensitive_methods() {
    printf("=== Case-Insensitive Method Test ===\n\n");
    
    router_t *router = create_router();
    if (router == NULL) {
        printf("Failed to create router\n");
        return;
    }
    
    add_route(router, "GET", "/test", home_handler);
    add_route(router, "POST", "/test", api_handler);
    
    printf("Testing case-insensitive method matching:\n");
    
    route_handler handler;
    
    handler = find_route(router, "GET", "/test");
    printf("  GET /test -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "get", "/test");
    printf("  get /test -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "Get", "/test");
    printf("  Get /test -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "POST", "/test");
    printf("  POST /test -> %s\n", handler ? "Found" : "Not Found");
    
    handler = find_route(router, "post", "/test");
    printf("  post /test -> %s\n", handler ? "Found" : "Not Found");
    
    printf("\n");
    
    destroy_router(router);
}

int main() {
    printf("=== Routing System Test Suite ===\n\n");
    
    test_basic_routing();
    test_wildcard_routing();
    test_request_handling();
    test_case_insensitive_methods();
    
    printf("=== Test Suite Complete ===\n");
    
    return 0;
}