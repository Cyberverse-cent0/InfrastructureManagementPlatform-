#include "../global_lib.h"


static bool database_create_table_users(database_t *database, const char *table_name)
{
    if (database == NULL || table_name == NULL) {
        print_error("Invalid parameters in creating table");
        return false;
    }
    const char* query = {
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY,"
        "username VARCHAR(50) UNIQUE NOT NULL,"
        "password_hash VARCHAR(255) NOT NULL,"
        "email VARCHAR(100) UNIQUE NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")"
    }; 
    PGresult *result = PQexec(database->connection, query);
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error("Failed to create table: %s", PQerrorMessage(database->connection));
        PQclear(result);
        return false;
    }
    PQclear(result);
    return true;
}