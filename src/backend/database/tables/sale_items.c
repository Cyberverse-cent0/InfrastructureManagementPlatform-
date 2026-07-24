/*
sale_items
├── id
├── sale_id
├── product_id
├── quantity
├── unit_price
└── total_price
*/
#include "../global_lib.h"

int create_sale_items_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS sale_items ("
        "id BIGSERIAL PRIMARY KEY,"
        "sale_id BIGINT NOT NULL,"
        "product_id BIGINT NOT NULL,"
        "quantity DECIMAL(15, 2) NOT NULL,"
        "unit_price DECIMAL(10, 2) NOT NULL,"
        "total_price DECIMAL(15, 2) NOT NULL,"
        "FOREIGN KEY (sale_id) REFERENCES sales(id) ON DELETE CASCADE,"
        "FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create sale_items table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}