#include <time.h>
#include <stdlib.h>
#include "network.h" // inclde network api


#define time_difrence 50 // 5 seconds
typedef struct {
    char *ntp_server;
    int port;
    int timeout;
} ntp_config_t;

static int sync_time_with_ntp_server(const char *ntp_server) {
    // TODO: Implement NTP time synchronization
    // This would use network.h to connect to NTP server and sync time
    return 0;
}

