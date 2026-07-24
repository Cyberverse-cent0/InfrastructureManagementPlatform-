#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define _GNU_SOURCE
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>

#define MAX_PATH_LENGTH 4096
#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif

typedef enum {
    IPV4,
    IPV6,
    IP_UNKNOWN
} ip_version_t;

typedef enum {
    WIRED,
    WIRELESS,
    LOOPBACK,
    VIRTUAL,
    INTERFACE_UNKNOWN
} INTERFACE_TYPE;

typedef enum {
    CONNECTED_UP,
    DISCONNECTED_DOWN,
    CONNECTING_PENDING,
    CONNECTION_UNKNOWN
} connection_status;

typedef struct {
    char* name;
    ip_version_t ip_version;
    INTERFACE_TYPE interface_type;
    connection_status connection_status;
    char* ip_address;
    char* mac_address;
    char* subnet_mask;
    char* gateway;
    char* dns_servers;
} network_interface_t;

typedef struct {
    char* ssid;
    char* bssid;
    char* security;
    int signal_strength;
    int channel;
    int frequency;
} WiFi_info;


typedef struct {
    char name[IF_NAMESIZE];
    char ip_address[INET6_ADDRSTRLEN];
    char mac_address[18];
    char subnet_mask[INET6_ADDRSTRLEN];
    char gateway[INET6_ADDRSTRLEN];
    
    INTERFACE_TYPE type;
    connection_status connection;
    ip_version_t ip_version;

    bool is_up;
    bool has_ipv4;
    bool has_ipv6;
    
    int mtu;
    unsigned long rx_bytes;
    unsigned long tx_bytes;
    unsigned long rx_packets;
    unsigned long tx_packets;

} network_interface;

// Forward declarations
typedef struct thread_pool_t thread_pool_t;

// Work item structure (must be defined before thread_pool_t)
typedef struct {
    network_interface *interface;
    void (*callback)(network_interface *iface, void *user_data);
    void *user_data;
    
} work_item_t;

// Thread pool and worker system
struct thread_pool_t {
    pthread_t *threads;
    int thread_count;
    pthread_mutex_t lock;
    pthread_cond_t condition;
    bool shutdown;
    
    network_interface *interfaces;
    int interface_count;
    
    work_item_t *work_queue;
    int queue_size;
    int queue_capacity;
    int queue_head;
    int queue_tail;
    
};


typedef struct {
    char client_ip[INET6_ADDRSTRLEN];

    time_t time_start;
    time_t time_end;

    long time_taken_ms;

    size_t payload_size;

    bool is_success;

} log_server_work_t;

// Server logging functions
void log_server_work_init(log_server_work_t *log_work);
void log_server_work_start(log_server_work_t *log_work, const char *client_ip);
void log_server_work_end(log_server_work_t *log_work, size_t payload_size, bool success);
void log_server_work_print(const log_server_work_t *log_work);
char* log_server_work_to_json(const log_server_work_t *log_work);

// Function declarations
int get_wifi_information(WiFi_info* wifi_info);
INTERFACE_TYPE get_interface_type(const char *interface_name);
connection_status get_connection_status(const char *interface_name);
ip_version_t get_ip_version(const char *interface_name);
int main_network_server(void);
int discover_interfaces(network_interface** interfaces, int* count);
void free_interfaces(network_interface* interfaces, int count);
int populate_interface_info(network_interface* iface);

// Thread pool functions
thread_pool_t* create_thread_pool(int thread_count, network_interface *interfaces, int interface_count);
void destroy_thread_pool(thread_pool_t *pool);
int thread_pool_add_work(thread_pool_t *pool, network_interface *iface, 
                         void (*callback)(network_interface *iface, void *user_data), 
                         void *user_data);
void* worker_thread(void *arg);

// HTTP server functions
typedef struct {
    int socket_fd;
    struct sockaddr_in client_addr;
    char buffer[8192];
    int bytes_received;
    
    // Enhanced HTTP parsing
    char method[16];
    char path[512];
    char version[16];
    char query_string[256];
    
    // Headers
    char headers[32][256];  // Up to 32 headers
    int header_count;
    
    // Body
    char *body;
    size_t body_length;
    
    // Connection info
    char client_ip[INET6_ADDRSTRLEN];
    int client_port;
    
    // Timing
    struct timeval start_time;
    
} http_request_t;

typedef struct {
    int status_code;
    char *status_text;
    
    // Headers
    char headers[32][256];  // Up to 32 headers
    int header_count;
    
    // Body
    char *body;
    size_t body_length;
    
    // Connection handling
    bool keep_alive;
    
    // Legacy compatibility
    char *content_type;
    
} http_response_t;

// HTTP methods
typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_PATCH,
    HTTP_HEAD,
    HTTP_OPTIONS,
    HTTP_CONNECT,
    HTTP_TRACE,
    HTTP_UNKNOWN
} http_method_t;

// Header parsing
int parse_http_request(http_request_t *request);
int parse_http_headers(http_request_t *request);
const char* get_header(const http_request_t *request, const char *name);
void add_header(http_response_t *response, const char *name, const char *value);

// HTTP method helpers
http_method_t parse_http_method(const char *method_str);
const char* http_method_to_string(http_method_t method);

// Enhanced response handling
http_response_t* create_http_response_ex(int status_code);
void set_response_body(http_response_t *response, const char *body, size_t length);
void set_json_response(http_response_t *response, const char *json_data);

// Original functions (maintained for compatibility)
void handle_http_request(http_request_t *request, int server_socket);
http_response_t* create_http_response(int status_code, const char *content_type);
void send_http_response(int client_socket, http_response_t *response);
void free_http_response(http_response_t *response);
char* get_http_reason_phrase(int status_code);

// Routing system
typedef struct route route_t;
typedef void (*route_handler)(http_request_t *req, http_response_t *res);

struct route {
    char *method;
    char *pattern;
    route_handler handler;
    route_t *next;
};

typedef struct {
    route_t *routes;
    int route_count;
    
} router_t;

// Router functions
router_t* create_router(void);
void destroy_router(router_t *router);
int add_route(router_t *router, const char *method, const char *pattern, route_handler handler);
route_handler find_route(router_t *router, const char *method, const char *path);
void handle_request_with_router(http_request_t *req, http_response_t *res, router_t *router);

// Routing system
typedef struct route route_t;
typedef void (*route_handler)(http_request_t *req, http_response_t *res);

struct route {
    char *method;
    char *pattern;
    route_handler handler;
    route_t *next;
};

typedef struct {
    route_t *routes;
    int route_count;
    
} router_t;

// Router functions
router_t* create_router(void);
void destroy_router(router_t *router);
int add_route(router_t *router, const char *method, const char *pattern, route_handler handler);
route_handler find_route(router_t *router, const char *method, const char *path);
void handle_request_with_router(http_request_t *req, http_response_t *res, router_t *router);

#endif // HTTP_SERVER_H
