#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include "../utils/logger.h"
#include "../utils/print_function.h"
#include "http_server.h"

// strdup compatibility
static char* my_strdup(const char *s) {
    if (s == NULL) return NULL;
    char *d = (char*)malloc(strlen(s) + 1);
    if (d == NULL) return NULL;
    return strcpy(d, s);
}
#define strdup my_strdup

// The reason for creating a custom HTTP server is because we want to use all interfaces to avoid network bottlenecks
// Improve the code with good API for the program

#define DEFAULT_BACKEND_PORT 7234
#define DEFAULT_USE_MULTIPLE_INTERFACE true

static bool loopback_or_not(const char *interface_name) {
    char path[MAX_PATH_LENGTH];
    unsigned int flags;

    if (interface_name == NULL)
        return false;

    snprintf(path, sizeof(path),
             "/sys/class/net/%s/flags",
             interface_name);

    FILE *file = fopen(path, "r");

    if (file == NULL)
        return false;

    if (fscanf(file, "%x", &flags) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);

    /*
     * IFF_LOOPBACK = 0x8
     */
    return (flags & 0x8) != 0;
}

/*
 * Check whether an interface is wireless.
 *
 * Wireless interfaces expose:
 *
 * /sys/class/net/<interface>/wireless/
 */
static bool wireless_or_not(const char *interface_name) {
    char path[MAX_PATH_LENGTH];

    if (interface_name == NULL)
        return false;

    snprintf(path, sizeof(path),
             "/sys/class/net/%s/wireless",
             interface_name);

    return access(path, F_OK) == 0;
}

/*
 * Check whether an interface is virtual.
 *
 * Physical network devices normally have a device symlink:
 *
 * /sys/class/net/<interface>/device
 *
 * Virtual interfaces generally don't.
 */
static bool virtual_or_not(const char *interface_name) {
    char path[MAX_PATH_LENGTH];

    if (interface_name == NULL)
        return false;

    snprintf(path, sizeof(path),
             "/sys/class/net/%s/device",
             interface_name);

    /*
     * No physical device associated with the interface.
     */
    return access(path, F_OK) != 0;
}

/*
 * Check whether an interface is wired Ethernet.
 */
static bool wired_or_not(const char *interface_name) {
    char path[MAX_PATH_LENGTH];

    if (interface_name == NULL)
        return false;

    /*
     * A physical network interface has a /device entry.
     */
    snprintf(path, sizeof(path),
             "/sys/class/net/%s/device",
             interface_name);

    if (access(path, F_OK) != 0)
        return false;

    /*
     * Wireless interfaces are physical too, so exclude them.
     */
    if (wireless_or_not(interface_name))
        return false;

    /*
     * Loopback is not wired.
     */
    if (loopback_or_not(interface_name))
        return false;

    return true;
}

/*
 * Determine the type of a Linux network interface.
 */
INTERFACE_TYPE get_interface_type(const char *interface_name) {
    char path[MAX_PATH_LENGTH];

    if (interface_name == NULL || interface_name[0] == '\0')
        return INTERFACE_UNKNOWN;

    /*
     * First make sure the interface actually exists.
     */
    snprintf(path, sizeof(path),
             "/sys/class/net/%s",
             interface_name);

    if (access(path, F_OK) != 0)
        return INTERFACE_UNKNOWN;

    /*
     * Order matters.
     */
    if (loopback_or_not(interface_name))
        return LOOPBACK;

    if (wireless_or_not(interface_name  ))
        return WIRELESS;

    if (virtual_or_not(interface_name))
        return VIRTUAL;

    if (wired_or_not(interface_name))
        return WIRED;

    return INTERFACE_UNKNOWN;
}

// Runs in pthread
connection_status get_connection_status(const char *interface_name) {
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sock < 0) {
        return CONNECTION_UNKNOWN;
    }
    
    strncpy(ifr.ifr_name, interface_name, IF_NAMESIZE - 1);
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
    
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        close(sock);
        return CONNECTION_UNKNOWN;
    }
    
    close(sock);
    
    // Check if interface is UP and RUNNING
    if ((ifr.ifr_flags & IFF_UP) && (ifr.ifr_flags & IFF_RUNNING)) {
        return CONNECTED_UP;
    } else if (ifr.ifr_flags & IFF_UP) {
        return CONNECTING_PENDING;
    } else {
        return DISCONNECTED_DOWN;
    }
}

int get_wifi_information(WiFi_info* wifi_info) {
    (void)wifi_info; // Suppress unused parameter warning
    // TODO: Implement WiFi information gathering
    // Should use malloc to allocate memory for wifi_info structure
    print_warning("WiFi information gathering not yet implemented");
    return -1;
}

static int get_mac_address(const char *interface_name, char *mac_address) {
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sock < 0) {
        print_error("Failed to create socket for MAC address");
        return -1;
    }
    
    strncpy(ifr.ifr_name, interface_name, IF_NAMESIZE - 1);
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
    
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        close(sock);
        print_error("Failed to get MAC address");
        return -1;
    }
    
    close(sock);
    
    snprintf(mac_address, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
             (unsigned char)ifr.ifr_hwaddr.sa_data[0],
             (unsigned char)ifr.ifr_hwaddr.sa_data[1],
             (unsigned char)ifr.ifr_hwaddr.sa_data[2],
             (unsigned char)ifr.ifr_hwaddr.sa_data[3],
             (unsigned char)ifr.ifr_hwaddr.sa_data[4],
             (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
    
    return 0;
}

static int get_ip_address(const char *interface_name, char *ip_address, sa_family_t family) {
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        print_error("Failed to get interface addresses");
        return -1;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
            
        if (ifa->ifa_addr->sa_family == family && 
            strcmp(interface_name, ifa->ifa_name) == 0) {
            
            if (family == AF_INET) {
                struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                inet_ntop(AF_INET, &addr->sin_addr, ip_address, INET_ADDRSTRLEN);
            } else if (family == AF_INET6) {
                struct sockaddr_in6 *addr = (struct sockaddr_in6 *)ifa->ifa_addr;
                inet_ntop(AF_INET6, &addr->sin6_addr, ip_address, INET6_ADDRSTRLEN);
            }
            
            freeifaddrs(ifaddr);
            return 0;
        }
    }
    
    freeifaddrs(ifaddr);
    return -1;
}

static int get_mtu(const char *interface_name) {
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sock < 0) {
        print_error("Failed to create socket for MTU");
        return -1;
    }
    
    strncpy(ifr.ifr_name, interface_name, IF_NAMESIZE - 1);
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
    
    if (ioctl(sock, SIOCGIFMTU, &ifr) < 0) {
        close(sock);
        return -1;
    }
    
    close(sock);
    return ifr.ifr_mtu;
}

static int get_interface_stats(const char *interface_name, unsigned long *rx_bytes, 
                                unsigned long *tx_bytes, unsigned long *rx_packets, 
                                unsigned long *tx_packets) {
    char path[PATH_MAX];
    FILE *file;
    
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", interface_name);
    file = fopen(path, "r");
    if (file) {
        fscanf(file, "%lu", rx_bytes);
        fclose(file);
    }
    
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", interface_name);
    file = fopen(path, "r");
    if (file) {
        fscanf(file, "%lu", tx_bytes);
        fclose(file);
    }
    
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_packets", interface_name);
    file = fopen(path, "r");
    if (file) {
        fscanf(file, "%lu", rx_packets);
        fclose(file);
    }
    
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_packets", interface_name);
    file = fopen(path, "r");
    if (file) {
        fscanf(file, "%lu", tx_packets);
        fclose(file);
    }
    
    return 0;
}

static bool is_interface_up(const char *interface_name) {
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sock < 0) {
        return false;
    }
    
    strncpy(ifr.ifr_name, interface_name, IF_NAMESIZE - 1);
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
    
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        close(sock);
        return false;
    }
    
    close(sock);
    return (ifr.ifr_flags & IFF_UP) != 0;
}

ip_version_t get_ip_version(const char *interface_name) {
    struct ifaddrs *ifaddr, *ifa;
    bool has_ipv4 = false;
    bool has_ipv6 = false;
    
    if (getifaddrs(&ifaddr) == -1) {
        print_error("Failed to get interface addresses");
        return IP_UNKNOWN;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
            
        if (strcmp(interface_name, ifa->ifa_name) == 0) {
            if (ifa->ifa_addr->sa_family == AF_INET) {
                has_ipv4 = true;
            } else if (ifa->ifa_addr->sa_family == AF_INET6) {
                has_ipv6 = true;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    
    // Return the primary IP version, or indicate if both are present
    if (has_ipv4 && has_ipv6) {
        return IPV4; // Prefer IPv4 as primary if both exist
    } else if (has_ipv4) {
        return IPV4;
    } else if (has_ipv6) {
        return IPV6;
    } else {
        return IP_UNKNOWN;
    }
}

int populate_interface_info(network_interface* iface) {
    if (iface == NULL || iface->name[0] == '\0') {
        return -1;
    }
    
    // Get interface type
    iface->type = get_interface_type(iface->name);
    
    // Check if interface is up
    iface->is_up = is_interface_up(iface->name);
    
    // Get connection status
    iface->connection = get_connection_status(iface->name);
    
    // Get IP version and addresses
    iface->has_ipv4 = (get_ip_address(iface->name, iface->ip_address, AF_INET) == 0);
    iface->has_ipv6 = (get_ip_address(iface->name, iface->ip_address, AF_INET6) == 0);
    
    // Determine primary IP version using the dedicated function
    iface->ip_version = get_ip_version(iface->name);
    
    // Set the primary IP address based on the determined version
    if (iface->ip_version == IPV4) {
        get_ip_address(iface->name, iface->ip_address, AF_INET);
    } else if (iface->ip_version == IPV6) {
        get_ip_address(iface->name, iface->ip_address, AF_INET6);
    } else {
        iface->ip_address[0] = '\0';
    }
    
    // Get MAC address
    if (get_mac_address(iface->name, iface->mac_address) != 0) {
        strcpy(iface->mac_address, "00:00:00:00:00:00");
    }
    
    // Get MTU
    iface->mtu = get_mtu(iface->name);
    
    // Get statistics
    get_interface_stats(iface->name, &iface->rx_bytes, &iface->tx_bytes, 
                        &iface->rx_packets, &iface->tx_packets);
    
    // Initialize other fields
    iface->subnet_mask[0] = '\0';
    iface->gateway[0] = '\0';
    
    return 0;
}

int discover_interfaces(network_interface** interfaces, int* count) {
    struct ifaddrs *ifaddr, *ifa;
    int interface_count = 0;
    network_interface* result = NULL;
    
    if (getifaddrs(&ifaddr) == -1) {
        print_error("Failed to get interface addresses");
        return -1;
    }
    
    // First pass: count interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        
        // Only count each interface once
        bool already_counted = false;
        for (struct ifaddrs *tmp = ifaddr; tmp != ifa; tmp = tmp->ifa_next) {
            if (tmp->ifa_name != NULL && strcmp(tmp->ifa_name, ifa->ifa_name) == 0) {
                already_counted = true;
                break;
            }
        }
        
        if (!already_counted) {
            interface_count++;
        }
    }
    
    // Allocate memory
    result = (network_interface*)malloc(interface_count * sizeof(network_interface));
    if (result == NULL) {
        freeifaddrs(ifaddr);
        print_error("Failed to allocate memory for interfaces");
        return -1;
    }
    
    // Second pass: populate interface data
    int index = 0;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        
        // Check if this interface has already been processed
        bool already_processed = false;
        for (int i = 0; i < index; i++) {
            if (strcmp(result[i].name, ifa->ifa_name) == 0) {
                already_processed = true;
                break;
            }
        }
        
        if (!already_processed) {
            strncpy(result[index].name, ifa->ifa_name, IFNAMSIZ - 1);
            result[index].name[IFNAMSIZ - 1] = '\0';
            
            // Populate interface information
            populate_interface_info(&result[index]);
            
            index++;
        }
    }
    
    freeifaddrs(ifaddr);
    
    *interfaces = result;
    *count = interface_count;
    
    char info_msg[256];
    snprintf(info_msg, sizeof(info_msg), "Discovered %d network interfaces", interface_count);
    print_info(info_msg);
    return 0;
}

void free_interfaces(network_interface* interfaces, int count) {
    (void)count; // Suppress unused parameter warning
    if (interfaces != NULL) {
        free(interfaces);
    }
}

// Thread pool implementation
void* worker_thread(void *arg) {
    thread_pool_t *pool = (thread_pool_t *)arg;
    
    while (true) {
        pthread_mutex_lock(&pool->lock);
        
        while (!pool->shutdown && pool->queue_size == 0) {
            pthread_cond_wait(&pool->condition, &pool->lock);
        }
        
        if (pool->shutdown && pool->queue_size == 0) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }
        
        // Get work item from queue
        work_item_t item = pool->work_queue[pool->queue_head];
        pool->queue_head = (pool->queue_head + 1) % pool->queue_capacity;
        pool->queue_size--;
        
        pthread_mutex_unlock(&pool->lock);
        
        // Execute work item
        if (item.callback != NULL && item.interface != NULL) {
            item.callback(item.interface, item.user_data);
        }
    }
    
    return NULL;
}

thread_pool_t* create_thread_pool(int thread_count, network_interface *interfaces, int interface_count) {
    thread_pool_t *pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        print_error("Failed to allocate memory for thread pool");
        return NULL;
    }
    
    pool->thread_count = thread_count;
    pool->interfaces = interfaces;
    pool->interface_count = interface_count;
    pool->shutdown = false;
    pool->queue_size = 0;
    pool->queue_capacity = 256; // Default queue capacity
    pool->queue_head = 0;
    pool->queue_tail = 0;
    
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->condition, NULL);
    
    pool->work_queue = (work_item_t *)malloc(pool->queue_capacity * sizeof(work_item_t));
    if (pool->work_queue == NULL) {
        print_error("Failed to allocate memory for work queue");
        free(pool);
        return NULL;
    }
    
    pool->threads = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    if (pool->threads == NULL) {
        print_error("Failed to allocate memory for threads");
        free(pool->work_queue);
        free(pool);
        return NULL;
    }
    
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Failed to create worker thread %d", i);
            print_error(error_msg);
            
            // Clean up already created threads
            for (int j = 0; j < i; j++) {
                pthread_cancel(pool->threads[j]);
            }
            free(pool->threads);
            free(pool->work_queue);
            free(pool);
            return NULL;
        }
    }
    
    char info_msg[256];
    snprintf(info_msg, sizeof(info_msg), "Created thread pool with %d workers", thread_count);
    print_info(info_msg);
    return pool;
}

void destroy_thread_pool(thread_pool_t *pool) {
    if (pool == NULL) {
        return;
    }
    
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->condition);
    pthread_mutex_unlock(&pool->lock);
    
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->condition);
    
    free(pool->work_queue);
    free(pool->threads);
    free(pool);
    
    print_info("Thread pool destroyed");
}

int thread_pool_add_work(thread_pool_t *pool, network_interface *iface, 
                         void (*callback)(network_interface *iface, void *user_data), 
                         void *user_data) {
    if (pool == NULL) {
        return -1;
    }
    
    pthread_mutex_lock(&pool->lock);
    
    // Check if queue is full
    if (pool->queue_size >= pool->queue_capacity) {
        pthread_mutex_unlock(&pool->lock);
        print_error("Work queue is full");
        return -1;
    }
    
    // Add work item to queue
    work_item_t item;
    item.interface = iface;
    item.callback = callback;
    item.user_data = user_data;
    
    pool->work_queue[pool->queue_tail] = item;
    pool->queue_tail = (pool->queue_tail + 1) % pool->queue_capacity;
    pool->queue_size++;
    
    pthread_cond_signal(&pool->condition);
    pthread_mutex_unlock(&pool->lock);
    
    return 0;
}

// HTTP server implementation
char* get_http_reason_phrase(int status_code) {
    switch (status_code) {
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 409: return "Conflict";
        case 413: return "Payload Too Large";
        case 415: return "Unsupported Media Type";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        default: return "Unknown";
    }
}

// Enhanced HTTP parsing
http_method_t parse_http_method(const char *method_str) {
    if (strcmp(method_str, "GET") == 0) return HTTP_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_DELETE;
    if (strcmp(method_str, "PATCH") == 0) return HTTP_PATCH;
    if (strcmp(method_str, "HEAD") == 0) return HTTP_HEAD;
    if (strcmp(method_str, "OPTIONS") == 0) return HTTP_OPTIONS;
    if (strcmp(method_str, "CONNECT") == 0) return HTTP_CONNECT;
    if (strcmp(method_str, "TRACE") == 0) return HTTP_TRACE;
    return HTTP_UNKNOWN;
}

const char* http_method_to_string(http_method_t method) {
    switch (method) {
        case HTTP_GET: return "GET";
        case HTTP_POST: return "POST";
        case HTTP_PUT: return "PUT";
        case HTTP_DELETE: return "DELETE";
        case HTTP_PATCH: return "PATCH";
        case HTTP_HEAD: return "HEAD";
        case HTTP_OPTIONS: return "OPTIONS";
        case HTTP_CONNECT: return "CONNECT";
        case HTTP_TRACE: return "TRACE";
        default: return "UNKNOWN";
    }
}

int parse_http_request(http_request_t *request) {
    if (request == NULL) {
        return -1;
    }
    
    // Parse request line: METHOD PATH VERSION
    if (sscanf(request->buffer, "%511s %511s %15s", request->method, request->path, request->version) != 3) {
        return -1;
    }
    
    // Parse query string from path
    char *query_start = strchr(request->path, '?');
    if (query_start != NULL) {
        *query_start = '\0';  // Split path and query
        strncpy(request->query_string, query_start + 1, sizeof(request->query_string) - 1);
        request->query_string[sizeof(request->query_string) - 1] = '\0';
    } else {
        request->query_string[0] = '\0';
    }
    
    // Parse headers
    return parse_http_headers(request);
}

int parse_http_headers(http_request_t *request) {
    char *line_start = request->buffer;
    char *line_end;
    
    // Skip request line (already parsed)
    line_end = strchr(line_start, '\r');
    if (line_end == NULL) line_end = strchr(line_start, '\n');
    if (line_end == NULL) return -1;
    
    line_start = line_end + 1;
    if (*line_start == '\n') line_start++;  // Handle \r\n vs \n
    
    request->header_count = 0;
    
    // Parse each header line
    while (request->header_count < 32 && *line_start != '\0' && *line_start != '\r' && *line_start != '\n') {
        line_end = strchr(line_start, '\r');
        if (line_end == NULL) line_end = strchr(line_start, '\n');
        if (line_end == NULL) break;
        
        int line_length = line_end - line_start;
        if (line_length > 0 && line_length < 255) {
            // Copy header line
            strncpy(request->headers[request->header_count], line_start, line_length);
            request->headers[request->header_count][line_length] = '\0';
            
            // Trim trailing whitespace
            char *header_end = request->headers[request->header_count] + line_length - 1;
            while (header_end > request->headers[request->header_count] && 
                   (*header_end == ' ' || *header_end == '\t')) {
                *header_end = '\0';
                header_end--;
            }
            
            request->header_count++;
        }
        
        line_start = line_end + 1;
        if (*line_start == '\n') line_start++;  // Handle \r\n vs \n
    }
    
    return request->header_count;
}

const char* get_header(const http_request_t *request, const char *name) {
    if (request == NULL || name == NULL) {
        return NULL;
    }
    
    size_t name_len = strlen(name);
    for (int i = 0; i < request->header_count; i++) {
        if (strncasecmp(request->headers[i], name, name_len) == 0) {
            // Check for colon separator
            if (request->headers[i][name_len] == ':') {
                // Return header value (skip colon and whitespace)
                const char *value = request->headers[i] + name_len + 1;
                while (*value == ' ' || *value == '\t') value++;
                return value;
            }
        }
    }
    
    return NULL;
}

void add_header(http_response_t *response, const char *name, const char *value) {
    if (response == NULL || name == NULL || value == NULL) {
        return;
    }
    
    if (response->header_count >= 32) {
        return;  // Header limit reached
    }
    
    snprintf(response->headers[response->header_count], 256, "%s: %s", name, value);
    response->header_count++;
}

http_response_t* create_http_response_ex(int status_code) {
    http_response_t *response = (http_response_t *)malloc(sizeof(http_response_t));
    if (response == NULL) {
        return NULL;
    }
    
    memset(response, 0, sizeof(http_response_t));
    response->status_code = status_code;
    const char *reason_phrase = get_http_reason_phrase(status_code);
    response->status_text = (char *)malloc(strlen(reason_phrase) + 1);
    if (response->status_text != NULL) {
        strcpy(response->status_text, reason_phrase);
    }
    response->keep_alive = false;
    response->header_count = 0;
    
    // Add default headers
    add_header(response, "Server", "InfrastructureManagementPlatform/1.0");
    
    // Add current date header
    time_t now = time(NULL);
    char date_str[128];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    add_header(response, "Date", date_str);
    
    return response;
}

void set_response_body(http_response_t *response, const char *body, size_t length) {
    if (response == NULL || body == NULL) {
        return;
    }
    
    if (response->body != NULL) {
        free(response->body);
    }
    
    response->body = (char *)malloc(length + 1);
    if (response->body != NULL) {
        memcpy(response->body, body, length);
        response->body[length] = '\0';
        response->body_length = length;
        
        // Update Content-Length header
        char content_length[32];
        snprintf(content_length, sizeof(content_length), "%zu", length);
        add_header(response, "Content-Length", content_length);
    }
}

void set_json_response(http_response_t *response, const char *json_data) {
    if (response == NULL || json_data == NULL) {
        return;
    }
    
    add_header(response, "Content-Type", "application/json");
    set_response_body(response, json_data, strlen(json_data));
}

http_response_t* create_http_response(int status_code, const char *content_type) {
    http_response_t *response = create_http_response_ex(status_code);
    if (response == NULL) {
        return NULL;
    }
    
    if (content_type != NULL) {
        add_header(response, "Content-Type", content_type);
        // For backward compatibility, also set content_type field
        response->content_type = (char *)malloc(strlen(content_type) + 1);
        if (response->content_type != NULL) {
            strcpy(response->content_type, content_type);
        }
    }
    
    return response;
}

void free_http_response(http_response_t *response) {
    if (response != NULL) {
        if (response->body != NULL) {
            free(response->body);
        }
        if (response->status_text != NULL) {
            free(response->status_text);
        }
        if (response->content_type != NULL) {
            free(response->content_type);
        }
        free(response);
    }
}

void send_http_response(int client_socket, http_response_t *response) {
    char header[4096];
    int header_length = 0;
    
    // Status line
    header_length += snprintf(header + header_length, sizeof(header) - header_length,
                            "HTTP/1.1 %d %s\r\n",
                            response->status_code,
                            response->status_text);
    
    // Headers
    for (int i = 0; i < response->header_count; i++) {
        header_length += snprintf(header + header_length, sizeof(header) - header_length,
                                "%s\r\n", response->headers[i]);
    }
    
    // Add legacy content_type if not already in headers
    if (response->content_type != NULL) {
        bool has_content_type = false;
        for (int i = 0; i < response->header_count; i++) {
            if (strncasecmp(response->headers[i], "Content-Type:", 13) == 0) {
                has_content_type = true;
                break;
            }
        }
        if (!has_content_type) {
            header_length += snprintf(header + header_length, sizeof(header) - header_length,
                                    "Content-Type: %s\r\n", response->content_type);
        }
    }
    
    // Connection header
    const char *connection = response->keep_alive ? "keep-alive" : "close";
    header_length += snprintf(header + header_length, sizeof(header) - header_length,
                            "Connection: %s\r\n", connection);
    
    // End of headers
    header_length += snprintf(header + header_length, sizeof(header) - header_length, "\r\n");
    
    // Send headers
    send(client_socket, header, header_length, 0);
    
    // Send body
    if (response->body != NULL && response->body_length > 0) {
        send(client_socket, response->body, response->body_length, 0);
    }
}

void handle_http_request(http_request_t *request, int server_socket) {
    // Parse the HTTP request using enhanced parser
    if (parse_http_request(request) != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to parse HTTP request");
        print_error(error_msg);
        
        http_response_t *error_response = create_http_response(400, "text/plain");
        set_response_body(error_response, "Bad Request", 11);
        send_http_response(server_socket, error_response);
        free_http_response(error_response);
        return;
    }
    
    char info_msg[1024];
    snprintf(info_msg, sizeof(info_msg), "Received %s request for %s", request->method, request->path);
    print_info(info_msg);
    
    // Log query string if present
    if (strlen(request->query_string) > 0) {
        char query_msg[512];
        snprintf(query_msg, sizeof(query_msg), "Query string: %s", request->query_string);
        print_info(query_msg);
    }
    
    // Log headers
    for (int i = 0; i < request->header_count; i++) {
        char header_msg[512];
        snprintf(header_msg, sizeof(header_msg), "Header: %s", request->headers[i]);
        print_info(header_msg);
    }
    
    http_response_t *response = create_http_response(200, "application/json");
    
    // Generate JSON response with interface information
    const char *json_template = "{\"status\":\"success\",\"message\":\"Infrastructure Management Platform API\"}";
    set_response_body(response, json_template, strlen(json_template));
    
    send_http_response(server_socket, response);
    free_http_response(response);
    
    char success_msg[1024];
    snprintf(success_msg, sizeof(success_msg), "Successfully processed %s request for %s", request->method, request->path);
    print_info(success_msg);
}

void* http_connection_handler(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);
    
    // Get client IP address
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    if (getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len) < 0) {
        close(client_socket);
        return NULL;
    }
    
    http_request_t request;
    memset(&request, 0, sizeof(request));
    request.socket_fd = client_socket;
    request.client_addr = client_addr;
    
    inet_ntop(AF_INET, &client_addr.sin_addr, request.client_ip, INET6_ADDRSTRLEN);
    request.client_port = ntohs(client_addr.sin_port);
    
    // Initialize timing
    gettimeofday(&request.start_time, NULL);
    
    // Initialize logging
    log_server_work_t log_work;
    log_server_work_start(&log_work, request.client_ip);
    
    request.bytes_received = recv(client_socket, request.buffer, sizeof(request.buffer) - 1, 0);
    
    bool success = false;
    if (request.bytes_received > 0) {
        request.buffer[request.bytes_received] = '\0';
        handle_http_request(&request, client_socket);
        success = true;
    }
    
    // End logging and print
    log_server_work_end(&log_work, request.bytes_received, success);
    log_server_work_print(&log_work);
    
    close(client_socket);
    return NULL;
}

int main_network_server(void) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        print_error("Failed to create server socket");
        return -1;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        print_error("Failed to set socket options");
        close(server_socket);
        return -1;
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DEFAULT_BACKEND_PORT);
    
    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        print_error("Failed to bind socket");
        close(server_socket);
        return -1;
    }
    
    // Listen for connections
    if (listen(server_socket, 10) < 0) {
        print_error("Failed to listen on socket");
        close(server_socket);
        return -1;
    }
    
    char info_msg[256];
    snprintf(info_msg, sizeof(info_msg), "HTTP server started on port %d", DEFAULT_BACKEND_PORT);
    print_info(info_msg);
    
    // Discover network interfaces
    network_interface *interfaces = NULL;
    int interface_count = 0;
    
    if (discover_interfaces(&interfaces, &interface_count) == 0) {
        snprintf(info_msg, sizeof(info_msg), "Managing %d network interfaces", interface_count);
        print_info(info_msg);
        
        // Create thread pool for interface management
        thread_pool_t *pool = create_thread_pool(4, interfaces, interface_count);
        if (pool != NULL) {
            // Main server loop
            while (true) {
                client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                if (client_socket < 0) {
                    print_error("Failed to accept client connection");
                    continue;
                }
                
                // Handle connection in a separate thread
                pthread_t thread;
                int *client_sock_ptr = (int *)malloc(sizeof(int));
                *client_sock_ptr = client_socket;
                
                if (pthread_create(&thread, NULL, http_connection_handler, client_sock_ptr) != 0) {
                    print_error("Failed to create connection handler thread");
                    close(client_socket);
                    free(client_sock_ptr);
                } else {
                    pthread_detach(thread);
                }
            }
            
            destroy_thread_pool(pool);
        }
        
        free_interfaces(interfaces, interface_count);
    }
    
    close(server_socket);
    return 0;
}

// Server logging implementation
void log_server_work_init(log_server_work_t *log_work) {
    if (log_work == NULL) {
        return;
    }
    
    memset(log_work, 0, sizeof(log_server_work_t));
    log_work->time_start = 0;
    log_work->time_end = 0;
    log_work->time_taken_ms = 0;
    log_work->payload_size = 0;
    log_work->is_success = false;
}

void log_server_work_start(log_server_work_t *log_work, const char *client_ip) {
    if (log_work == NULL) {
        return;
    }
    
    log_server_work_init(log_work);
    
    if (client_ip != NULL) {
        strncpy(log_work->client_ip, client_ip, INET6_ADDRSTRLEN - 1);
        log_work->client_ip[INET6_ADDRSTRLEN - 1] = '\0';
    }
    
    log_work->time_start = time(NULL);
}

void log_server_work_end(log_server_work_t *log_work, size_t payload_size, bool success) {
    if (log_work == NULL) {
        return;
    }
    
    log_work->time_end = time(NULL);
    log_work->payload_size = payload_size;
    log_work->is_success = success;
    
    // Calculate time taken in milliseconds
    if (log_work->time_end > log_work->time_start) {
        log_work->time_taken_ms = (log_work->time_end - log_work->time_start) * 1000;
    } else {
        log_work->time_taken_ms = 0;
    }
}

void log_server_work_print(const log_server_work_t *log_work) {
    if (log_work == NULL) {
        return;
    }
    
    char log_msg[1024];
    snprintf(log_msg, sizeof(log_msg), 
             "Server Work Log - Client: %s, Time: %ldms, Payload: %zu bytes, Success: %s",
             log_work->client_ip,
             log_work->time_taken_ms,
             log_work->payload_size,
             log_work->is_success ? "Yes" : "No");
    
    if (log_work->is_success) {
        print_info(log_msg);
    } else {
        print_error(log_msg);
    }
}

char* log_server_work_to_json(const log_server_work_t *log_work) {
    if (log_work == NULL) {
        char *error_json = (char *)malloc(28);
        if (error_json == NULL) return NULL;
        strcpy(error_json, "{\"error\":\"null log_work\"}");
        return error_json;
    }
    
    char *json = (char *)malloc(1024);
    if (json == NULL) {
        return NULL;
    }
    
    snprintf(json, 1024,
             "{\"client_ip\":\"%s\",\"time_start\":%ld,\"time_end\":%ld,"
             "\"time_taken_ms\":%ld,\"payload_size\":%zu,\"is_success\":%s}",
             log_work->client_ip,
             log_work->time_start,
             log_work->time_end,
             log_work->time_taken_ms,
             log_work->payload_size,
             log_work->is_success ? "true" : "false");
    
    return json;
}

// Routing system implementation
router_t* create_router(void) {
    router_t *router = (router_t *)malloc(sizeof(router_t));
    if (router == NULL) {
        return NULL;
    }
    
    router->routes = NULL;
    router->route_count = 0;
    
    return router;
}

void destroy_router(router_t *router) {
    if (router == NULL) {
        return;
    }
    
    route_t *current = router->routes;
    while (current != NULL) {
        route_t *next = current->next;
        if (current->method != NULL) {
            free(current->method);
        }
        if (current->pattern != NULL) {
            free(current->pattern);
        }
        free(current);
        current = next;
    }
    
    free(router);
}

int add_route(router_t *router, const char *method, const char *pattern, route_handler handler) {
    if (router == NULL || method == NULL || pattern == NULL || handler == NULL) {
        return -1;
    }
    
    route_t *new_route = (route_t *)malloc(sizeof(route_t));
    if (new_route == NULL) {
        return -1;
    }
    
    new_route->method = strdup(method);
    new_route->pattern = strdup(pattern);
    new_route->handler = handler;
    new_route->next = NULL;
    
    if (new_route->method == NULL || new_route->pattern == NULL) {
        if (new_route->method != NULL) free(new_route->method);
        if (new_route->pattern != NULL) free(new_route->pattern);
        free(new_route);
        return -1;
    }
    
    // Add to beginning of list
    new_route->next = router->routes;
    router->routes = new_route;
    router->route_count++;
    
    char info_msg[256];
    snprintf(info_msg, sizeof(info_msg), "Added route: %s %s", method, pattern);
    print_info(info_msg);
    
    return 0;
}

// Simple pattern matching (supports wildcards)
static bool pattern_matches(const char *pattern, const char *path) {
    // Exact match
    if (strcmp(pattern, path) == 0) {
        return true;
    }
    
    // Wildcard match
    if (strcmp(pattern, "*") == 0) {
        return true;
    }
    
    // Simple wildcard: /api/* matches /api/anything
    size_t pattern_len = strlen(pattern);
    if (pattern_len > 1 && pattern[pattern_len - 1] == '*') {
        if (strncmp(pattern, path, pattern_len - 1) == 0) {
            return true;
        }
    }
    
    return false;
}

route_handler find_route(router_t *router, const char *method, const char *path) {
    if (router == NULL || method == NULL || path == NULL) {
        return NULL;
    }
    
    route_t *current = router->routes;
    while (current != NULL) {
        if (strcasecmp(current->method, method) == 0 && pattern_matches(current->pattern, path)) {
            return current->handler;
        }
        current = current->next;
    }
    
    return NULL;
}

void handle_request_with_router(http_request_t *req, http_response_t *res, router_t *router) {
    if (router == NULL) {
        // Default 404 response
        res->status_code = 404;
        set_json_response(res, "{\"error\":\"Not Found\",\"message\":\"No route found\"}");
        return;
    }
    
    route_handler handler = find_route(router, req->method, req->path);
    if (handler != NULL) {
        handler(req, res);
    } else {
        // 404 Not Found
        res->status_code = 404;
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "{\"error\":\"Not Found\",\"message\":\"No route found for %s\"}", req->path);
        set_json_response(res, error_msg);
    }
}