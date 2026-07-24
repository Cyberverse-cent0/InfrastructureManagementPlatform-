/**
 * @file asset.c
 * @brief Asset Data Model Implementation
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Asset data model implementation with validation and JSON conversion.
 */

#include "asset.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void asset_init(asset_t *asset) {
    if (asset == NULL) return;
    
    memset(asset, 0, sizeof(asset_t));
    
    // Set default values
    strcpy(asset->type, "unknown");
    strcpy(asset->status, "inactive");
    strcpy(asset->priority, "medium");
    asset->created_at = time(NULL);
    asset->updated_at = time(NULL);
    asset->last_checked = time(NULL);
    asset->tag_count = 0;
}

bool asset_validate(const asset_t *asset) {
    if (asset == NULL) return false;
    
    // Validate required fields
    if (strlen(asset->name) == 0) return false;
    if (strlen(asset->type) == 0) return false;
    if (strlen(asset->status) == 0) return false;
    
    // Validate numeric fields
    if (asset->cpu < 0) return false;
    if (asset->memory < 0) return false;
    if (asset->storage < 0) return false;
    
    // Validate status values
    const char *valid_statuses[] = {"active", "inactive", "maintenance", "error"};
    bool status_valid = false;
    for (int i = 0; i < 4; i++) {
        if (strcmp(asset->status, valid_statuses[i]) == 0) {
            status_valid = true;
            break;
        }
    }
    if (!status_valid) return false;
    
    return true;
}

bool asset_to_json(const asset_t *asset, char *buffer, size_t buffer_size) {
    if (asset == NULL || buffer == NULL) return false;
    
    // Build tags array
    char tags_array[MAX_STRING_LENGTH * 2] = "";
    if (asset->tag_count > 0) {
        strcat(tags_array, "[");
        for (int i = 0; i < asset->tag_count; i++) {
            if (i > 0) strcat(tags_array, ",");
            char tag_json[MAX_STRING_LENGTH];
            snprintf(tag_json, sizeof(tag_json), "\"%.50s\"", asset->tags[i]);
            strcat(tags_array, tag_json);
        }
        strcat(tags_array, "]");
    } else {
        strcpy(tags_array, "[]");
    }
    
    // Convert timestamps to strings
    char created_str[32], updated_str[32], checked_str[32];
    strftime(created_str, sizeof(created_str), "%Y-%m-%dT%H:%M:%SZ", 
             gmtime(&asset->created_at));
    strftime(updated_str, sizeof(updated_str), "%Y-%m-%dT%H:%M:%SZ", 
             gmtime(&asset->updated_at));
    strftime(checked_str, sizeof(checked_str), "%Y-%m-%dT%H:%M:%SZ", 
             gmtime(&asset->last_checked));
    
    // Build JSON
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"id\":\"%s\","
        "\"name\":\"%s\","
        "\"type\":\"%s\","
        "\"status\":\"%s\","
        "\"priority\":\"%s\","
        "\"location\":\"%s\","
        "\"cpu\":%d,"
        "\"memory\":%d,"
        "\"storage\":%d,"
        "\"ipAddress\":\"%s\","
        "\"macAddress\":\"%s\","
        "\"operatingSystem\":\"%s\","
        "\"createdAt\":\"%s\","
        "\"updatedAt\":\"%s\","
        "\"lastChecked\":\"%s\","
        "\"tags\":%s,"
        "\"notes\":\"%s\""
        "}",
        asset->id, asset->name, asset->type, asset->status, asset->priority,
        asset->location, asset->cpu, asset->memory, asset->storage,
        asset->ip_address, asset->mac_address, asset->operating_system,
        created_str, updated_str, checked_str, tags_array, asset->notes
    );
    
    return written > 0 && (size_t)written < buffer_size;
}

bool asset_from_json(const char *json, asset_t *asset) {
    // TODO: Implement JSON parsing
    // This would typically use a JSON library like cJSON or jansson
    (void)json;
    (void)asset;
    return false;
}