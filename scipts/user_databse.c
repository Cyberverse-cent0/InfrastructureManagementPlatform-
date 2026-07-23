#define _POSIX_C_SOURCE 200809L
#include "user_databse.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_POSTGRES
#include <libpq-fe.h>
#endif

// Initialize database connection
database_connection_t* database_init(const char* host, int port, const char* database, 
                                     const char* user, const char* password) {
    if (host == NULL || database == NULL || user == NULL) {
        return NULL;
    }
    
    database_connection_t* db = (database_connection_t*)malloc(sizeof(database_connection_t));
    if (db == NULL) {
        return NULL;
    }
    
    db->host = strdup(host);
    db->port = port > 0 ? port : 5432;
    db->database = strdup(database);
    db->user = strdup(user);
    db->password = password ? strdup(password) : NULL;
    db->connection = NULL;
    db->is_connected = 0;
    
    return db;
}

// Connect to database
db_status_t database_connect(database_connection_t* db) {
    if (db == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
#ifdef USE_POSTGRES
    char conn_string[512];
    snprintf(conn_string, sizeof(conn_string), "host=%s port=%d dbname=%s user=%s",
             db->host, db->port, db->database, db->user);
    
    if (db->password != NULL) {
        strcat(conn_string, " password=");
        strcat(conn_string, db->password);
    }
    
    PGconn* conn = PQconnectdb(conn_string);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return DB_CONNECTION_ERROR;
    }
    
    db->connection = conn;
    db->is_connected = 1;
    return DB_SUCCESS;
#else
    // For non-PostgreSQL implementations, return success for now
    // This can be extended to support SQLite or other databases
    printf("Database connection simulated (PostgreSQL not enabled)\n");
    db->is_connected = 1;
    return DB_SUCCESS;
#endif
}

// Disconnect from database
void database_disconnect(database_connection_t* db) {
    if (db == NULL || !db->is_connected) {
        return;
    }
    
#ifdef USE_POSTGRES
    if (db->connection != NULL) {
        PGconn* conn = (PGconn*)db->connection;
        PQfinish(conn);
        db->connection = NULL;
    }
#endif
    
    db->is_connected = 0;
}

// Destroy database connection
void database_destroy(database_connection_t* db) {
    if (db == NULL) {
        return;
    }
    
    database_disconnect(db);
    
    if (db->host != NULL) {
        free(db->host);
    }
    if (db->database != NULL) {
        free(db->database);
    }
    if (db->user != NULL) {
        free(db->user);
    }
    if (db->password != NULL) {
        free(db->password);
    }
    
    free(db);
}

// Execute a query
db_status_t database_execute_query(database_connection_t* db, const char* query) {
    if (db == NULL || query == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
    if (!db->is_connected) {
        return DB_CONNECTION_ERROR;
    }
    
#ifdef USE_POSTGRES
    PGconn* conn = (PGconn*)db->connection;
    PGresult* res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return DB_QUERY_ERROR;
    }
    
    PQclear(res);
    return DB_SUCCESS;
#else
    printf("Executing query: %s\n", query);
    return DB_SUCCESS;
#endif
}

// Execute a query with parameters
db_status_t database_execute_query_params(database_connection_t* db, const char* query, 
                                          const char** params, int param_count) {
    if (db == NULL || query == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
    if (!db->is_connected) {
        return DB_CONNECTION_ERROR;
    }
    
#ifdef USE_POSTGRES
    PGconn* conn = (PGconn*)db->connection;
    
    // Convert params to const char* array for PQexecParams
    const char* param_values[param_count];
    int param_lengths[param_count];
    int param_formats[param_count];
    
    for (int i = 0; i < param_count; i++) {
        param_values[i] = params[i];
        param_lengths[i] = params[i] ? strlen(params[i]) : 0;
        param_formats[i] = 0; // text format
    }
    
    PGresult* res = PQexecParams(conn, query, param_count, NULL, param_values, 
                                param_lengths, param_formats, 0);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Parameterized query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return DB_QUERY_ERROR;
    }
    
    PQclear(res);
    return DB_SUCCESS;
#else
    (void)params; // Suppress unused parameter warning
    (void)param_count; // Suppress unused parameter warning
    printf("Executing parameterized query: %s\n", query);
    return DB_SUCCESS;
#endif
}

// Create user table
db_status_t create_user_table(database_connection_t* db) {
    if (db == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
    const char* query = "CREATE TABLE IF NOT EXISTS users ("
                        "id SERIAL PRIMARY KEY, "
                        "username VARCHAR(255) UNIQUE NOT NULL, "
                        "email VARCHAR(255) UNIQUE NOT NULL, "
                        "password_hash VARCHAR(255) NOT NULL, "
                        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
    
    return database_execute_query(db, query);
}

// Add user to database
db_status_t add_user(database_connection_t* db, const char* username, const char* email, 
                     const char* password_hash) {
    if (db == NULL || username == NULL || email == NULL || password_hash == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
#ifdef USE_POSTGRES
    const char* query = "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3);";
    const char* params[3] = {username, email, password_hash};
    return database_execute_query_params(db, query, params, 3);
#else
    char query[512];
    snprintf(query, sizeof(query), 
             "INSERT INTO users (username, email, password_hash) VALUES ('%s', '%s', '%s');",
             username, email, password_hash);
    return database_execute_query(db, query);
#endif
}

// Get user from database
db_status_t get_user(database_connection_t* db, const char* username, char** email, 
                     char** password_hash) {
    if (db == NULL || username == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
#ifdef USE_POSTGRES
    const char* query = "SELECT email, password_hash FROM users WHERE username = $1;";
    const char* params[1] = {username};
    
    PGconn* conn = (PGconn*)db->connection;
    PGresult* res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get user query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return DB_QUERY_ERROR;
    }
    
    if (PQntuples(res) == 0) {
        PQclear(res);
        return DB_ERROR; // User not found
    }
    
    *email = strdup(PQgetvalue(res, 0, 0));
    *password_hash = strdup(PQgetvalue(res, 0, 1));
    
    PQclear(res);
    return DB_SUCCESS;
#else
    printf("Getting user: %s\n", username);
    *email = strdup("test@example.com");
    *password_hash = strdup("hashed_password");
    return DB_SUCCESS;
#endif
}

// Update user in database
db_status_t update_user(database_connection_t* db, const char* username, 
                        const char* new_email, const char* new_password_hash) {
    if (db == NULL || username == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
    if (new_email == NULL && new_password_hash == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
#ifdef USE_POSTGRES
    if (new_email != NULL && new_password_hash != NULL) {
        const char* query = "UPDATE users SET email = $1, password_hash = $2, updated_at = CURRENT_TIMESTAMP WHERE username = $3;";
        const char* params[3] = {new_email, new_password_hash, username};
        return database_execute_query_params(db, query, params, 3);
    } else if (new_email != NULL) {
        const char* query = "UPDATE users SET email = $1, updated_at = CURRENT_TIMESTAMP WHERE username = $2;";
        const char* params[2] = {new_email, username};
        return database_execute_query_params(db, query, params, 2);
    } else {
        const char* query = "UPDATE users SET password_hash = $1, updated_at = CURRENT_TIMESTAMP WHERE username = $2;";
        const char* params[2] = {new_password_hash, username};
        return database_execute_query_params(db, query, params, 2);
    }
#else
    printf("Updating user: %s\n", username);
    return DB_SUCCESS;
#endif
}

// Delete user from database
db_status_t delete_user(database_connection_t* db, const char* username) {
    if (db == NULL || username == NULL) {
        return DB_INVALID_ARGUMENT;
    }
    
#ifdef USE_POSTGRES
    const char* query = "DELETE FROM users WHERE username = $1;";
    const char* params[1] = {username};
    return database_execute_query_params(db, query, params, 1);
#else
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM users WHERE username = '%s';", username);
    return database_execute_query(db, query);
#endif
}