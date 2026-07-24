/*
purchase_order_items
├── id
├── purchase_order_id
├── product_id
├── quantity
├── unit_cost
└── total_cost
*/
#include "../global_lib.h"

int create_purchase_order_items_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS purchase_order_items ("
        "id BIGSERIAL PRIMARY KEY,"
        "purchase_order_id BIGINT NOT NULL,"
        "product_id BIGINT NOT NULL,"
        "quantity DECIMAL(15, 2) NOT NULL,"
        "unit_cost DECIMAL(10, 2) NOT NULL,"
        "total_cost DECIMAL(15, 2) NOT NULL,"
        "FOREIGN KEY (purchase_order_id) REFERENCES purchase_orders(id) ON DELETE CASCADE,"
        "FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create purchase_order_items table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}