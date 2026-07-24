#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "src/backend/core/http_server.h"

void print_interface_info(network_interface *iface) {
    printf("Interface: %s\n", iface->name);
    printf("  Type: ");
    switch (iface->type) {
        case WIRED: printf("WIRED\n"); break;
        case WIRELESS: printf("WIRELESS\n"); break;
        case LOOPBACK: printf("LOOPBACK\n"); break;
        case VIRTUAL: printf("VIRTUAL\n"); break;
        default: printf("UNKNOWN\n"); break;
    }
    
    printf("  Status: ");
    switch (iface->connection) {
        case CONNECTED_UP: printf("CONNECTED\n"); break;
        case DISCONNECTED_DOWN: printf("DISCONNECTED\n"); break;
        case CONNECTING_PENDING: printf("CONNECTING\n"); break;
        default: printf("UNKNOWN\n"); break;
    }
    
    printf("  IP Version: ");
    switch (iface->ip_version) {
        case IPV4: printf("IPv4\n"); break;
        case IPV6: printf("IPv6\n"); break;
        default: printf("UNKNOWN\n"); break;
    }
    
    printf("  IP Address: %s\n", iface->ip_address);
    printf("  MAC Address: %s\n", iface->mac_address);
    printf("  MTU: %d\n", iface->mtu);
    printf("  Is Up: %s\n", iface->is_up ? "Yes" : "No");
    printf("  Has IPv4: %s\n", iface->has_ipv4 ? "Yes" : "No");
    printf("  Has IPv6: %s\n", iface->has_ipv6 ? "Yes" : "No");
    printf("  RX Bytes: %lu\n", iface->rx_bytes);
    printf("  TX Bytes: %lu\n", iface->tx_bytes);
    printf("\n");
}

void interface_callback(network_interface *iface, void *user_data) {
    (void)user_data; // Suppress unused parameter warning
    printf("Processing interface: %s\n", iface->name);
    print_interface_info(iface);
}

int main() {
    network_interface *interfaces = NULL;
    int interface_count = 0;
    
    printf("=== Network Interface Discovery Test ===\n\n");
    
    // Discover interfaces
    if (discover_interfaces(&interfaces, &interface_count) != 0) {
        fprintf(stderr, "Failed to discover interfaces\n");
        return 1;
    }
    
    printf("Discovered %d network interfaces:\n\n", interface_count);
    
    // Print interface information
    for (int i = 0; i < interface_count; i++) {
        print_interface_info(&interfaces[i]);
    }
    
    // Test thread pool
    printf("=== Thread Pool Test ===\n\n");
    
    thread_pool_t *pool = create_thread_pool(2, interfaces, interface_count);
    if (pool == NULL) {
        fprintf(stderr, "Failed to create thread pool\n");
        free_interfaces(interfaces, interface_count);
        return 1;
    }
    
    // Add work for each interface
    for (int i = 0; i < interface_count; i++) {
        thread_pool_add_work(pool, &interfaces[i], interface_callback, NULL);
    }
    
    // Wait a bit for work to complete
    sleep(2);
    
    // Cleanup
    destroy_thread_pool(pool);
    free_interfaces(interfaces, interface_count);
    
    printf("=== Test Complete ===\n");
    
    return 0;
}