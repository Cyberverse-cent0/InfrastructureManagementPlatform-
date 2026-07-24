/**
 * @file json_utils.c
 * @brief JSON Utility Functions Implementation
 * @author Infrastructure Platform Team
 * @date 2024-07-24
 * 
 * Utility functions implementation for JSON parsing and generation.
 */

#include "json_utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

bool json_escape_string(const char *input, char *output, size_t output_size) {
    if (input == NULL || output == NULL || output_size == 0) return false;
    
    size_t input_len = strlen(input);
    size_t output_pos = 0;
    
    for (size_t i = 0; i < input_len && output_pos < output_size - 1; i++) {
        char c = input[i];
        
        switch (c) {
            case '"':
                if (output_pos + 2 >= output_size) return false;
                output[output_pos++] = '\\';
                output[output_pos++] = '"';
                break;
            case '\\':
                if (output_pos + 2 >= output_size) return false;
                output[output_pos++] = '\\';
                output[output_pos++] = '\\';
                break;
            case '\n':
                if (output_pos + 2 >= output_size) return false;
                output[output_pos++] = '\\';
                output[output_pos++] = 'n';
                break;
            case '\r':
                if (output_pos + 2 >= output_size) return false;
                output[output_pos++] = '\\';
                output[output_pos++] = 'r';
                break;
            case '\t':
                if (output_pos + 2 >= output_size) return false;
                output[output_pos++] = '\\';
                output[output_pos++] = 't';
                break;
            default:
                output[output_pos++] = c;
                break;
        }
    }
    
    output[output_pos] = '\0';
    return true;
}

bool json_unescape_string(const char *input, char *output, size_t output_size) {
    if (input == NULL || output == NULL || output_size == 0) return false;
    
    size_t input_len = strlen(input);
    size_t output_pos = 0;
    
    for (size_t i = 0; i < input_len && output_pos < output_size - 1; i++) {
        char c = input[i];
        
        if (c == '\\' && i + 1 < input_len) {
            char next = input[i + 1];
            
            switch (next) {
                case '"':
                    output[output_pos++] = '"';
                    i++;
                    break;
                case '\\':
                    output[output_pos++] = '\\';
                    i++;
                    break;
                case 'n':
                    output[output_pos++] = '\n';
                    i++;
                    break;
                case 'r':
                    output[output_pos++] = '\r';
                    i++;
                    break;
                case 't':
                    output[output_pos++] = '\t';
                    i++;
                    break;
                default:
                    output[output_pos++] = c;
                    break;
            }
        } else {
            output[output_pos++] = c;
        }
    }
    
    output[output_pos] = '\0';
    return true;
}

bool json_build_response(char *buffer, size_t buffer_size, bool success, 
                        const char *data, const char *error) {
    if (buffer == NULL || buffer_size == 0) return false;
    
    int written;
    
    if (success) {
        if (data != NULL) {
            written = snprintf(buffer, buffer_size,
                "{\"success\":true,\"data\":%s}",
                data);
        } else {
            written = snprintf(buffer, buffer_size,
                "{\"success\":true}");
        }
    } else {
        if (error != NULL) {
            char escaped_error[512];
            if (!json_escape_string(error, escaped_error, sizeof(escaped_error))) {
                return false;
            }
            
            written = snprintf(buffer, buffer_size,
                "{\"success\":false,\"error\":\"%s\"}",
                escaped_error);
        } else {
            written = snprintf(buffer, buffer_size,
                "{\"success\":false,\"error\":\"Unknown error\"}");
        }
    }
    
    return written > 0 && (size_t)written < buffer_size;
}

bool json_build_paginated_response(char *buffer, size_t buffer_size, 
                                   const char *data, int page, int limit, int total) {
    if (buffer == NULL || buffer_size == 0) return false;
    
    int written = snprintf(buffer, buffer_size,
        "{\"success\":true,\"data\":%s,\"pagination\":{\"page\":%d,\"limit\":%d,\"total\":%d}}",
        data, page, limit, total);
    
    return written > 0 && (size_t)written < buffer_size;
}