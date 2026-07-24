/*
locations
├── id
├── name
├── type
├── address
├── is_active
└── created_at
*/
#include "../global_lib.h"

int create_locations_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS locations ("
        "id BIGSERIAL PRIMARY KEY,"
        "name VARCHAR(100) NOT NULL,"
        "type VARCHAR(50) NOT NULL," 
        "address TEXT,"
        "is_active BOOLEAN NOT NULL DEFAULT TRUE,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create locations table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}