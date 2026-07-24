/**
 * @file asset_handlers.h
 * @brief Asset HTTP Request Handlers
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * HTTP request handlers for asset-related endpoints.
 */

#ifndef ASSET_HANDLERS_H
#define ASSET_HANDLERS_H

#include <stdbool.h>

/**
 * @brief Handle GET request for all assets
 * 
 * @param client_socket Client socket descriptor
 * @return true on success, false on failure
 */
bool handle_get_assets(int client_socket);

/**
 * @brief Handle GET request for single asset by ID
 * 
 * @param client_socket Client socket descriptor
 * @param asset_id Asset ID string
 * @return true on success, false on failure
 */
bool handle_get_asset_by_id(int client_socket, const char *asset_id);

/**
 * @brief Handle POST request to create new asset
 * 
 * @param client_socket Client socket descriptor
 * @param request_body Request body containing asset data
 * @return true on success, false on failure
 */
bool handle_create_asset(int client_socket, const char *request_body);

/**
 * @brief Handle PUT request to update asset
 * 
 * @param client_socket Client socket descriptor
 * @param asset_id Asset ID string
 * @param request_body Request body containing updated asset data
 * @return true on success, false on failure
 */
bool handle_update_asset(int client_socket, const char *asset_id, const char *request_body);

/**
 * @brief Handle DELETE request to remove asset
 * 
 * @param client_socket Client socket descriptor
 * @param asset_id Asset ID string
 * @return true on success, false on failure
 */
bool handle_delete_asset(int client_socket, const char *asset_id);

#endif // ASSET_HANDLERS_H