/*
units
├── id
├── name
├── abbreviation
├── description
└── created_at
*/
#include "../global_lib.h"

int create_units_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS units ("
        "id BIGSERIAL PRIMARY KEY,"
        "name VARCHAR(50) UNIQUE NOT NULL,"
        "abbreviation VARCHAR(10) UNIQUE NOT NULL,"
        "description TEXT,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create units table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}