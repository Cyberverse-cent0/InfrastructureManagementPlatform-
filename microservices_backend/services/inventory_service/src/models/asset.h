/**
 * @file asset.h
 * @brief Asset Data Model
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Asset data model structure and validation functions.
 */

#ifndef ASSET_MODEL_H
#define ASSET_MODEL_H

#include <stdbool.h>
#include <time.h>

#define MAX_STRING_LENGTH 256
#define MAX_ARRAY_SIZE 50

/**
 * @brief Asset data structure
 */
typedef struct {
    char id[MAX_STRING_LENGTH];
    char name[MAX_STRING_LENGTH];
    char type[MAX_STRING_LENGTH];
    char status[MAX_STRING_LENGTH];
    char priority[MAX_STRING_LENGTH];
    char location[MAX_STRING_LENGTH];
    int cpu;
    int memory;
    int storage;
    char ip_address[MAX_STRING_LENGTH];
    char mac_address[MAX_STRING_LENGTH];
    char operating_system[MAX_STRING_LENGTH];
    time_t created_at;
    time_t updated_at;
    time_t last_checked;
    char tags[MAX_ARRAY_SIZE][MAX_STRING_LENGTH];
    int tag_count;
    char notes[MAX_STRING_LENGTH * 4]; // Allow longer notes
} asset_t;

/**
 * @brief Validate asset data
 * 
 * @param asset Asset to validate
 * @return true if valid, false otherwise
 */
bool asset_validate(const asset_t *asset);

/**
 * @brief Initialize asset with default values
 * 
 * @param asset Asset to initialize
 */
void asset_init(asset_t *asset);

/**
 * @brief Convert asset to JSON string
 * 
 * @param asset Asset to convert
 * @param buffer Buffer to store JSON
 * @param buffer_size Buffer size
 * @return true on success, false on failure
 */
bool asset_to_json(const asset_t *asset, char *buffer, size_t buffer_size);

/**
 * @brief Parse asset from JSON string
 * 
 * @param json JSON string to parse
 * @param asset Asset to populate
 * @return true on success, false on failure
 */
bool asset_from_json(const char *json, asset_t *asset);

#endif // ASSET_MODEL_H