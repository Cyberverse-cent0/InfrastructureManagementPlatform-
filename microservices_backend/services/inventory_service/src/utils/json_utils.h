/**
 * @file json_utils.h
 * @brief JSON Utility Functions
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Utility functions for JSON parsing and generation.
 */

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Escape JSON string
 * 
 * @param input Input string to escape
 * @param output Buffer to store escaped string
 * @param output_size Output buffer size
 * @return true on success, false on failure
 */
bool json_escape_string(const char *input, char *output, size_t output_size);

/**
 * @brief Unescape JSON string
 * 
 * @param input Input JSON string to unescape
 * @param output Buffer to store unescaped string
 * @param output_size Output buffer size
 * @return true on success, false on failure
 */
bool json_unescape_string(const char *input, char *output, size_t output_size);

/**
 * @brief Build JSON response with success status
 * 
 * @param buffer Buffer to store JSON
 * @param buffer_size Buffer size
 * @param success Success status
 * @param data Data payload (can be NULL)
 * @param error Error message (can be NULL)
 * @return true on success, false on failure
 */
bool json_build_response(char *buffer, size_t buffer_size, bool success, 
                        const char *data, const char *error);

/**
 * @brief Build paginated JSON response
 * 
 * @param buffer Buffer to store JSON
 * @param buffer_size Buffer size
 * @param data Array data JSON
 * @param page Current page number
 * @param limit Items per page
 * @param total Total items
 * @return true on success, false on failure
 */
bool json_build_paginated_response(char *buffer, size_t buffer_size, 
                                   const char *data, int page, int limit, int total);

#endif // JSON_UTILS_H