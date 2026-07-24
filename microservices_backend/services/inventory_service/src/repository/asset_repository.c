/**
 * @file asset_repository.c
 * @brief Asset Data Access Layer Implementation
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Repository implementation for asset data persistence operations.
 * Currently uses in-memory storage, to be replaced with database integration.
 */

#include "asset_repository.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_ASSETS 1000

/**
 * @brief In-memory asset storage
 */
typedef struct {
    asset_t assets[MAX_ASSETS];
    int count;
} asset_storage_t;

static asset_storage_t g_storage = {0};

asset_repository_t* asset_repository_init(const char *connection_string) {
    (void)connection_string; // Will be used for database connection
    
    asset_repository_t *repo = calloc(1, sizeof(asset_repository_t));
    if (repo == NULL) {
        return NULL;
    }
    
    // Initialize storage
    memset(&g_storage, 0, sizeof(g_storage));
    
    printf("Asset repository initialized\n");
    return repo;
}

void asset_repository_cleanup(asset_repository_t *repo) {
    if (repo == NULL) return;
    
    // Clear storage
    memset(&g_storage, 0, sizeof(g_storage));
    
    free(repo);
    printf("Asset repository cleaned up\n");
}

bool asset_repository_find_all(asset_repository_t *repo, asset_t *assets, 
                                int max_assets, int *count) {
    if (repo == NULL || assets == NULL || count == NULL) return false;
    
    int copy_count = (g_storage.count < max_assets) ? g_storage.count : max_assets;
    memcpy(assets, g_storage.assets, copy_count * sizeof(asset_t));
    *count = copy_count;
    
    return true;
}

bool asset_repository_find_by_id(asset_repository_t *repo, const char *asset_id, 
                                  asset_t *asset) {
    if (repo == NULL || asset_id == NULL || asset == NULL) return false;
    
    for (int i = 0; i < g_storage.count; i++) {
        if (strcmp(g_storage.assets[i].id, asset_id) == 0) {
            memcpy(asset, &g_storage.assets[i], sizeof(asset_t));
            return true;
        }
    }
    
    return false;
}

bool asset_repository_create(asset_repository_t *repo, const asset_t *asset) {
    if (repo == NULL || asset == NULL) return false;
    
    if (g_storage.count >= MAX_ASSETS) {
        printf("Asset storage full\n");
        return false;
    }
    
    // Generate ID if not provided
    asset_t new_asset = *asset;
    if (strlen(new_asset.id) == 0) {
        snprintf(new_asset.id, sizeof(new_asset.id), "asset_%d", g_storage.count + 1);
    }
    
    // Set timestamps
    new_asset.created_at = time(NULL);
    new_asset.updated_at = time(NULL);
    
    memcpy(&g_storage.assets[g_storage.count], &new_asset, sizeof(asset_t));
    g_storage.count++;
    
    printf("Asset created: %s\n", new_asset.id);
    return true;
}

bool asset_repository_update(asset_repository_t *repo, const asset_t *asset) {
    if (repo == NULL || asset == NULL) return false;
    
    for (int i = 0; i < g_storage.count; i++) {
        if (strcmp(g_storage.assets[i].id, asset->id) == 0) {
            asset_t updated_asset = *asset;
            updated_asset.updated_at = time(NULL);
            memcpy(&g_storage.assets[i], &updated_asset, sizeof(asset_t));
            printf("Asset updated: %s\n", asset->id);
            return true;
        }
    }
    
    return false;
}

bool asset_repository_delete(asset_repository_t *repo, const char *asset_id) {
    if (repo == NULL || asset_id == NULL) return false;
    
    for (int i = 0; i < g_storage.count; i++) {
        if (strcmp(g_storage.assets[i].id, asset_id) == 0) {
            // Shift remaining assets
            for (int j = i; j < g_storage.count - 1; j++) {
                memcpy(&g_storage.assets[j], &g_storage.assets[j + 1], sizeof(asset_t));
            }
            g_storage.count--;
            printf("Asset deleted: %s\n", asset_id);
            return true;
        }
    }
    
    return false;
}

bool asset_repository_count(asset_repository_t *repo, int *count) {
    if (repo == NULL || count == NULL) return false;
    
    *count = g_storage.count;
    return true;
}