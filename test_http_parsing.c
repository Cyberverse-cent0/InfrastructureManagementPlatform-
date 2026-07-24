#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "src/backend/core/http_server.h"

void test_http_method_parsing() {
    printf("=== HTTP Method Parsing Test ===\n\n");
    
    const char *methods[] = {"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS", "CONNECT", "TRACE", "INVALID"};
    int count = sizeof(methods) / sizeof(methods[0]);
    
    for (int i = 0; i < count; i++) {
        http_method_t method = parse_http_method(methods[i]);
        const char *method_str = http_method_to_string(method);
        printf("  %s -> %s\n", methods[i], method_str);
    }
    printf("\n");
}

void test_header_parsing() {
    printf("=== Header Parsing Test ===\n\n");
    
    http_request_t request;
    memset(&request, 0, sizeof(request));
    
    // Simulate HTTP request with headers
    const char *test_request = 
        "GET /api/test HTTP/1.1\r\n"
        "Host: localhost:7234\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Accept: application/json\r\n"
        "Content-Type: application/json\r\n"
        "Authorization: Bearer token123\r\n"
        "\r\n";
    
    strncpy(request.buffer, test_request, sizeof(request.buffer) - 1);
    request.bytes_received = strlen(test_request);
    
    int result = parse_http_request(&request);
    printf("  Parsing result: %d\n", result);
    printf("  Method: %s\n", request.method);
    printf("  Path: %s\n", request.path);
    printf("  Version: %s\n", request.version);
    printf("  Headers found: %d\n", request.header_count);
    
    for (int i = 0; i < request.header_count; i++) {
        printf("    %s\n", request.headers[i]);
    }
    
    // Test header retrieval
    const char *host = get_header(&request, "Host");
    const char *user_agent = get_header(&request, "User-Agent");
    const char *accept = get_header(&request, "Accept");
    
    printf("\n  Retrieved headers:\n");
    printf("    Host: %s\n", host ? host : "NULL");
    printf("    User-Agent: %s\n", user_agent ? user_agent : "NULL");
    printf("    Accept: %s\n", accept ? accept : "NULL");
    
    // Test case-insensitive header retrieval
    const char *host_lower = get_header(&request, "host");
    printf("    Host (lowercase): %s\n", host_lower ? host_lower : "NULL");
    
    printf("\n");
}

void test_query_string_parsing() {
    printf("=== Query String Parsing Test ===\n\n");
    
    http_request_t request;
    memset(&request, 0, sizeof(request));
    
    const char *test_requests[] = {
        "GET /api/test HTTP/1.1\r\n\r\n",
        "GET /api/test?key=value HTTP/1.1\r\n\r\n",
        "GET /api/test?key1=value1&key2=value2 HTTP/1.1\r\n\r\n",
        "GET /api/test?name=John&age=30&city=NYC HTTP/1.1\r\n\r\n"
    };
    
    for (int i = 0; i < 4; i++) {
        strncpy(request.buffer, test_requests[i], sizeof(request.buffer) - 1);
        request.bytes_received = strlen(test_requests[i]);
        
        parse_http_request(&request);
        
        printf("  Request %d:\n", i + 1);
        printf("    Path: %s\n", request.path);
        printf("    Query string: %s\n", request.query_string);
        printf("\n");
    }
}

void test_response_creation() {
    printf("=== Response Creation Test ===\n\n");
    
    // Test basic response
    http_response_t *response = create_http_response_ex(200);
    printf("  Basic response created: %s\n", response->status_text);
    printf("  Status code: %d\n", response->status_code);
    printf("  Header count: %d\n", response->header_count);
    
    // Test adding headers
    add_header(response, "Content-Type", "application/json");
    add_header(response, "X-Custom-Header", "CustomValue");
    printf("  Headers after adding: %d\n", response->header_count);
    
    // Test setting body
    const char *test_body = "{\"message\":\"Hello, World!\"}";
    set_response_body(response, test_body, strlen(test_body));
    printf("  Body length: %zu\n", response->body_length);
    printf("  Body: %s\n", response->body);
    
    // Test JSON response
    http_response_t *json_response = create_http_response_ex(201);
    set_json_response(json_response, "{\"status\":\"created\"}");
    printf("  JSON response body: %s\n", json_response->body);
    
    free_http_response(response);
    free_http_response(json_response);
    
    printf("\n");
}

void test_status_codes() {
    printf("=== Status Code Test ===\n\n");
    
    int status_codes[] = {100, 200, 201, 204, 301, 302, 304, 400, 401, 403, 404, 405, 409, 413, 415, 429, 500, 501, 502, 503, 504};
    int count = sizeof(status_codes) / sizeof(status_codes[0]);
    
    for (int i = 0; i < count; i++) {
        const char *phrase = get_http_reason_phrase(status_codes[i]);
        printf("  %d: %s\n", status_codes[i], phrase);
    }
    
    printf("\n");
}

void test_enhanced_request_parsing() {
    printf("=== Enhanced Request Parsing Test ===\n\n");
    
    http_request_t request;
    memset(&request, 0, sizeof(request));
    
    // Complex HTTP request
    const char *complex_request = 
        "POST /api/users HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/json\r\n"
        "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9\r\n"
        "User-Agent: MyClient/1.0\r\n"
        "Accept: application/json\r\n"
        "Content-Length: 45\r\n"
        "\r\n"
        "{\"name\":\"John Doe\",\"email\":\"john@example.com\"}";
    
    strncpy(request.buffer, complex_request, sizeof(request.buffer) - 1);
    request.bytes_received = strlen(complex_request);
    
    gettimeofday(&request.start_time, NULL);
    
    int parse_result = parse_http_request(&request);
    printf("  Parse result: %d\n", parse_result);
    printf("  Method: %s\n", request.method);
    printf("  Path: %s\n", request.path);
    printf("  Version: %s\n", request.version);
    printf("  Query string: %s\n", request.query_string);
    printf("  Header count: %d\n", request.header_count);
    
    printf("  Headers:\n");
    for (int i = 0; i < request.header_count; i++) {
        printf("    %s\n", request.headers[i]);
    }
    
    // Test specific header retrieval
    const char *auth = get_header(&request, "Authorization");
    const char *content_type = get_header(&request, "Content-Type");
    const char *content_length = get_header(&request, "Content-Length");
    
    printf("  Retrieved:\n");
    printf("    Authorization: %s\n", auth ? auth : "NULL");
    printf("    Content-Type: %s\n", content_type ? content_type : "NULL");
    printf("    Content-Length: %s\n", content_length ? content_length : "NULL");
    
    printf("\n");
}

int main() {
    printf("=== Enhanced HTTP Server Parsing Test Suite ===\n\n");
    
    test_http_method_parsing();
    test_header_parsing();
    test_query_string_parsing();
    test_response_creation();
    test_status_codes();
    test_enhanced_request_parsing();
    
    printf("=== Test Suite Complete ===\n");
    
    return 0;
}