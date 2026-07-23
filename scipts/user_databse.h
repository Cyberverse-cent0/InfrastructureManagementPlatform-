#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <stdio.h>
#include <stdlib.h>

// PostgreSQL includes (if using libpq)
#ifdef USE_POSTGRES
#include <libpq-fe.h>
#endif

// Database status codes
typedef enum {
    DB_SUCCESS = 0,
    DB_ERROR = -1,
    DB_CONNECTION_ERROR = -2,
    DB_QUERY_ERROR = -3,
    DB_AUTH_ERROR = -4,
    DB_INVALID_ARGUMENT = -5
} db_status_t;

// Database connection structure
typedef struct {
    void* connection;  // Generic connection pointer
    char* host;
    int port;
    char* database;
    char* user;
    char* password;
    int is_connected;
} database_connection_t;

// Function declarations
database_connection_t* database_init(const char* host, int port, const char* database, 
                                   const char* user, const char* password);
db_status_t database_connect(database_connection_t* db);
void database_disconnect(database_connection_t* db);
void database_destroy(database_connection_t* db);
db_status_t database_execute_query(database_connection_t* db, const char* query);
db_status_t database_execute_query_params(database_connection_t* db, const char* query, 
                                          const char** params, int param_count);

// User management functions
db_status_t create_user_table(database_connection_t* db);
db_status_t add_user(database_connection_t* db, const char* username, const char* email, 
                     const char* password_hash);
db_status_t get_user(database_connection_t* db, const char* username, char** email, 
                     char** password_hash);
db_status_t update_user(database_connection_t* db, const char* username, 
                        const char* new_email, const char* new_password_hash);
db_status_t delete_user(database_connection_t* db, const char* username);

#endif // USER_DATABASE_H