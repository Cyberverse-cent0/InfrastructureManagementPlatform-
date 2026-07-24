/*
purchase_orders
├── id
├── supplier_id
├── user_id
├── location_id
├── status
├── total_cost
├── created_at
└── updated_at
*/
#include "../global_lib.h"

int create_purchase_orders_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS purchase_orders ("
        "id BIGSERIAL PRIMARY KEY,"
        "supplier_id BIGINT NOT NULL,"
        "user_id BIGINT NOT NULL,"
        "location_id BIGINT NOT NULL,"
        "status VARCHAR(20) NOT NULL DEFAULT 'PENDING',"
        "total_cost DECIMAL(15, 2) NOT NULL DEFAULT 0,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (supplier_id) REFERENCES suppliers(id) ON DELETE CASCADE,"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,"
        "FOREIGN KEY (location_id) REFERENCES locations(id) ON DELETE CASCADE"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create purchase_orders table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}