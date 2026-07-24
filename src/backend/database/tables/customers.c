/*
customers
├── id
├── name
├── email
├── phone
├── address
├── created_at
└── updated_at
*/
#include "../global_lib.h"

int create_customers_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS customers ("
        "id BIGSERIAL PRIMARY KEY,"
        "name VARCHAR(255) NOT NULL,"
        "email VARCHAR(100),"
        "phone VARCHAR(30),"
        "address TEXT,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create customers table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}