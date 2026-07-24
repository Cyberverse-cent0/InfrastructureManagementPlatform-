#define _POSIX_C_SOURCE 200809L
#include "common_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

char* get_distribution(void) {
    char* result = NULL;
    char buffer[BUFFER_SIZE] = {0};
    FILE* fp = NULL;
    char* pretty_name = NULL;
    char* name = NULL;
    char* version = NULL;
    char* version_id = NULL;
    
    // Try to read /etc/os-release directly (more reliable than system())
    fp = fopen("/etc/os-release", "r");
    if (fp == NULL) {
        // Fallback to /etc/lsb-release
        fp = fopen("/etc/lsb-release", "r");
        if (fp == NULL) {
            // Last resort: try system command
            fp = popen("cat /etc/os-release 2>/dev/null", "r");
            if (fp == NULL) {
                return strdup("Error: Could not determine distribution");
            }
        }
    }
    
    // Parse the file line by line
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Look for PRETTY_NAME (preferred)
        if (strncmp(buffer, "PRETTY_NAME=", 12) == 0) {
            char* value = buffer + 12;
            // Remove quotes if present
            if (*value == '"' || *value == '\'') {
                value++;
                char* end = value + strlen(value) - 1;
                if (*end == '"' || *end == '\'') *end = 0;
            }
            pretty_name = strdup(value);
        }
        // Look for NAME
        else if (strncmp(buffer, "NAME=", 5) == 0) {
            char* value = buffer + 5;
            if (*value == '"' || *value == '\'') {
                value++;
                char* end = value + strlen(value) - 1;
                if (*end == '"' || *end == '\'') *end = 0;
            }
            name = strdup(value);
        }
        // Look for VERSION
        else if (strncmp(buffer, "VERSION=", 8) == 0) {
            char* value = buffer + 8;
            if (*value == '"' || *value == '\'') {
                value++;
                char* end = value + strlen(value) - 1;
                if (*end == '"' || *end == '\'') *end = 0;
            }
            version = strdup(value);
        }
        // Look for VERSION_ID
        else if (strncmp(buffer, "VERSION_ID=", 11) == 0) {
            char* value = buffer + 11;
            if (*value == '"' || *value == '\'') {
                value++;
                char* end = value + strlen(value) - 1;
                if (*end == '"' || *end == '\'') *end = 0;
            }
            version_id = strdup(value);
        }
    }
    
    fclose(fp);
    
    // Build result string with best available information
    if (pretty_name) {
        result = pretty_name;
        free(name);
        free(version);
        free(version_id);
    } else if (name && version) {
        asprintf(&result, "%s %s", name, version);
        free(name);
        free(version);
        free(version_id);
    } else if (name && version_id) {
        asprintf(&result, "%s %s", name, version_id);
        free(name);
        free(version_id);
    } else if (name) {
        result = name;
        free(version);
        free(version_id);
    } else {
        result = strdup("Unknown distribution");
    }
    
    return result;
}
