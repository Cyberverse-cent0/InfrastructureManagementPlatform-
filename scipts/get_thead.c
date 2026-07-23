#include "get_thead.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#endif

// Get the number of available threads/cores
int get_thread_count() {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#else
    long logical_cpu = sysconf(_SC_NPROCESSORS_ONLN);
    if (logical_cpu <= 0) {
        return 1; // Default to 1 if we can't determine the count
    }
    return (int)logical_cpu;
#endif
}

// Check if the system supports threading
bool check_if_support_threading() {
#ifdef _WIN32
    // Windows always supports threading
    return true;
#else
    long logical_cpu = sysconf(_SC_NPROCESSORS_ONLN);
    long configured_cpu = sysconf(_SC_NPROCESSORS_CONF);
    
    if (logical_cpu > 0 && configured_cpu > 0) {
        return true;
    }
    // Even if we can't get CPU info, most modern systems support threading
    return true;
#endif
}