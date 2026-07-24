/*
inventory
├── id
├── product_id
├── location_id
├── quantity
├── minimum_quantity
├── maximum_quantity
└── updated_at
*/
#include "../global_lib.h"

int create_inventory_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS inventory ("
        "id BIGSERIAL PRIMARY KEY,"
        "product_id BIGINT NOT NULL,"
        "location_id BIGINT NOT NULL,"
        "quantity DECIMAL(15, 2) NOT NULL DEFAULT 0,"
        "minimum_quantity DECIMAL(15, 2) DEFAULT 0,"
        "maximum_quantity DECIMAL(15, 2),"
        "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE,"
        "FOREIGN KEY (location_id) REFERENCES locations(id) ON DELETE CASCADE,"
        "UNIQUE (product_id, location_id)"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create inventory table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}