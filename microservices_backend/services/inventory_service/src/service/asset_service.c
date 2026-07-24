/**
 * @file asset_service.c
 * @brief Asset Business Logic Layer Implementation
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Service layer implementation for asset business logic and operations.
 */

#include "asset_service.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

asset_service_t* asset_service_init(asset_repository_t *repository) {
    if (repository == NULL) return NULL;
    
    asset_service_t *service = calloc(1, sizeof(asset_service_t));
    if (service == NULL) return NULL;
    
    service->repository = repository;
    printf("Asset service initialized\n");
    return service;
}

void asset_service_cleanup(asset_service_t *service) {
    if (service == NULL) return;
    
    free(service);
    printf("Asset service cleaned up\n");
}

bool asset_service_get_all(asset_service_t *service, int page, int limit, 
                           asset_t *assets, int *count) {
    if (service == NULL || assets == NULL || count == NULL) return false;
    
    // Validate pagination parameters
    if (page < 1) page = 1;
    if (limit < 1 || limit > 100) limit = 10;
    
    // Get all assets from repository
    asset_t all_assets[1000];
    int total_count = 0;
    
    if (!asset_repository_find_all(service->repository, all_assets, 1000, &total_count)) {
        return false;
    }
    
    // Calculate pagination
    int start_index = (page - 1) * limit;
    int end_index = start_index + limit;
    
    if (start_index >= total_count) {
        *count = 0;
        return true;
    }
    
    if (end_index > total_count) {
        end_index = total_count;
    }
    
    // Copy requested page
    int copy_count = end_index - start_index;
    memcpy(assets, &all_assets[start_index], copy_count * sizeof(asset_t));
    *count = copy_count;
    
    printf("Retrieved %d assets (page %d, limit %d)\n", copy_count, page, limit);
    return true;
}

bool asset_service_get_by_id(asset_service_t *service, const char *asset_id, 
                             asset_t *asset) {
    if (service == NULL || asset_id == NULL || asset == NULL) return false;
    
    if (!asset_repository_find_by_id(service->repository, asset_id, asset)) {
        printf("Asset not found: %s\n", asset_id);
        return false;
    }
    
    printf("Asset retrieved: %s\n", asset_id);
    return true;
}

bool asset_service_create(asset_service_t *service, const asset_t *asset) {
    if (service == NULL || asset == NULL) return false;
    
    // Validate asset data
    if (!asset_validate(asset)) {
        printf("Asset validation failed\n");
        return false;
    }
    
    // Check if asset with same ID already exists
    asset_t existing_asset;
    if (strlen(asset->id) > 0 && 
        asset_repository_find_by_id(service->repository, asset->id, &existing_asset)) {
        printf("Asset already exists: %s\n", asset->id);
        return false;
    }
    
    // Create asset
    if (!asset_repository_create(service->repository, asset)) {
        printf("Failed to create asset\n");
        return false;
    }
    
    printf("Asset created successfully\n");
    return true;
}

bool asset_service_update(asset_service_t *service, const asset_t *asset) {
    if (service == NULL || asset == NULL) return false;
    
    // Validate asset data
    if (!asset_validate(asset)) {
        printf("Asset validation failed\n");
        return false;
    }
    
    // Check if asset exists
    asset_t existing_asset;
    if (!asset_repository_find_by_id(service->repository, asset->id, &existing_asset)) {
        printf("Asset not found for update: %s\n", asset->id);
        return false;
    }
    
    // Update asset
    if (!asset_repository_update(service->repository, asset)) {
        printf("Failed to update asset\n");
        return false;
    }
    
    printf("Asset updated successfully\n");
    return true;
}

bool asset_service_delete(asset_service_t *service, const char *asset_id) {
    if (service == NULL || asset_id == NULL) return false;
    
    // Check if asset exists
    asset_t existing_asset;
    if (!asset_repository_find_by_id(service->repository, asset_id, &existing_asset)) {
        printf("Asset not found for deletion: %s\n", asset_id);
        return false;
    }
    
    // Delete asset
    if (!asset_repository_delete(service->repository, asset_id)) {
        printf("Failed to delete asset\n");
        return false;
    }
    
    printf("Asset deleted successfully\n");
    return true;
}

bool asset_service_search(asset_service_t *service, const char *search_field, 
                          const char *search_value, asset_t *assets, 
                          int max_results, int *count) {
    if (service == NULL || search_field == NULL || search_value == NULL) return false;
    
    // Get all assets
    asset_t all_assets[1000];
    int total_count = 0;
    
    if (!asset_repository_find_all(service->repository, all_assets, 1000, &total_count)) {
        return false;
    }
    
    // Filter assets based on search criteria
    int match_count = 0;
    for (int i = 0; i < total_count && match_count < max_results; i++) {
        bool match = false;
        
        if (strcmp(search_field, "name") == 0) {
            match = strstr(all_assets[i].name, search_value) != NULL;
        } else if (strcmp(search_field, "type") == 0) {
            match = strcmp(all_assets[i].type, search_value) == 0;
        } else if (strcmp(search_field, "status") == 0) {
            match = strcmp(all_assets[i].status, search_value) == 0;
        } else if (strcmp(search_field, "location") == 0) {
            match = strstr(all_assets[i].location, search_value) != NULL;
        }
        
        if (match) {
            memcpy(&assets[match_count], &all_assets[i], sizeof(asset_t));
            match_count++;
        }
    }
    
    *count = match_count;
    printf("Search found %d assets\n", match_count);
    return true;
}