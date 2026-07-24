/*
suppliers
├── id
├── name
├── email
├── phone
├── address
├── tax_number
├── is_active
├── created_at
└── updated_at
*/
#include "../global_lib.h"

int create_suppliers_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS suppliers ("
        "id BIGSERIAL PRIMARY KEY,"
        "name VARCHAR(255) NOT NULL,"
        "email VARCHAR(100),"
        "phone VARCHAR(30),"
        "address TEXT,"
        "tax_number VARCHAR(50),"
        "is_active BOOLEAN NOT NULL DEFAULT TRUE,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create suppliers table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}