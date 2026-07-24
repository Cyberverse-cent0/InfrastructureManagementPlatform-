/**
 * @file asset_repository.h
 * @brief Asset Data Access Layer
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Repository interface for asset data persistence operations.
 */

#ifndef ASSET_REPOSITORY_H
#define ASSET_REPOSITORY_H

#include "../models/asset.h"
#include <stdbool.h>

/**
 * @brief Repository context structure
 */
typedef struct asset_repository {
    char connection_string[512];
    void *database_connection;  // Opaque pointer to database connection
} asset_repository_t;

/**
 * @brief Initialize asset repository
 * 
 * @param connection_string Database connection string
 * @return Repository context, or NULL on failure
 */
asset_repository_t* asset_repository_init(const char *connection_string);

/**
 * @brief Cleanup repository resources
 * 
 * @param repo Repository to cleanup
 */
void asset_repository_cleanup(asset_repository_t *repo);

/**
 * @brief Find all assets
 * 
 * @param repo Repository context
 * @param assets Array to store assets
 * @param max_assets Maximum number of assets to retrieve
 * @param count Number of assets retrieved
 * @return true on success, false on failure
 */
bool asset_repository_find_all(asset_repository_t *repo, asset_t *assets, 
                                int max_assets, int *count);

/**
 * @brief Find asset by ID
 * 
 * @param repo Repository context
 * @param asset_id Asset ID to find
 * @param asset Asset to populate
 * @return true if found, false otherwise
 */
bool asset_repository_find_by_id(asset_repository_t *repo, const char *asset_id, 
                                  asset_t *asset);

/**
 * @brief Create new asset
 * 
 * @param repo Repository context
 * @param asset Asset to create
 * @return true on success, false on failure
 */
bool asset_repository_create(asset_repository_t *repo, const asset_t *asset);

/**
 * @brief Update existing asset
 * 
 * @param repo Repository context
 * @param asset Asset to update
 * @return true on success, false on failure
 */
bool asset_repository_update(asset_repository_t *repo, const asset_t *asset);

/**
 * @brief Delete asset by ID
 * 
 * @param repo Repository context
 * @param asset_id Asset ID to delete
 * @return true on success, false on failure
 */
bool asset_repository_delete(asset_repository_t *repo, const char *asset_id);

/**
 * @brief Count total assets
 * 
 * @param repo Repository context
 * @param count Total count
 * @return true on success, false on failure
 */
bool asset_repository_count(asset_repository_t *repo, int *count);

#endif // ASSET_REPOSITORY_H