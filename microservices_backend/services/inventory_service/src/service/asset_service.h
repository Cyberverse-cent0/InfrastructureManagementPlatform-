/**
 * @file asset_service.h
 * @brief Asset Business Logic Layer
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Service layer for asset business logic and operations.
 */

#ifndef ASSET_SERVICE_H
#define ASSET_SERVICE_H

#include "../models/asset.h"
#include "../repository/asset_repository.h"
#include <stdbool.h>

/**
 * @brief Service context structure
 */
typedef struct {
    asset_repository_t *repository;
} asset_service_t;

/**
 * @brief Initialize asset service
 * 
 * @param repository Repository to use
 * @return Service context, or NULL on failure
 */
asset_service_t* asset_service_init(asset_repository_t *repository);

/**
 * @brief Cleanup service resources
 * 
 * @param service Service to cleanup
 */
void asset_service_cleanup(asset_service_t *service);

/**
 * @brief Get all assets with pagination
 * 
 * @param service Service context
 * @param page Page number (1-based)
 * @param limit Items per page
 * @param assets Array to store assets
 * @param count Number of assets retrieved
 * @return true on success, false on failure
 */
bool asset_service_get_all(asset_service_t *service, int page, int limit, 
                           asset_t *assets, int *count);

/**
 * @brief Get asset by ID
 * 
 * @param service Service context
 * @param asset_id Asset ID
 * @param asset Asset to populate
 * @return true if found, false otherwise
 */
bool asset_service_get_by_id(asset_service_t *service, const char *asset_id, 
                             asset_t *asset);

/**
 * @brief Create new asset with validation
 * 
 * @param service Service context
 * @param asset Asset to create
 * @return true on success, false on failure
 */
bool asset_service_create(asset_service_t *service, const asset_t *asset);

/**
 * @brief Update existing asset with validation
 * 
 * @param service Service context
 * @param asset Asset to update
 * @return true on success, false on failure
 */
bool asset_service_update(asset_service_t *service, const asset_t *asset);

/**
 * @brief Delete asset by ID
 * 
 * @param service Service context
 * @param asset_id Asset ID to delete
 * @return true on success, false on failure
 */
bool asset_service_delete(asset_service_t *service, const char *asset_id);

/**
 * @brief Search assets by criteria
 * 
 * @param service Service context
 * @param search_field Field to search
 * @param search_value Value to search for
 * @param assets Array to store results
 * @param max_results Maximum results to return
 * @param count Number of results found
 * @return true on success, false on failure
 */
bool asset_service_search(asset_service_t *service, const char *search_field, 
                          const char *search_value, asset_t *assets, 
                          int max_results, int *count);

#endif // ASSET_SERVICE_H