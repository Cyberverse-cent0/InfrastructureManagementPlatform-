/*
stock_movements
├── id
├── product_id
├── location_id
├── user_id
├── movement_type
├── quantity
├── reference_id
├── reason
└── created_at
*/
#include "../global_lib.h"

int create_stock_movements_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS stock_movements ("
        "id BIGSERIAL PRIMARY KEY,"
        "product_id BIGINT NOT NULL,"
        "location_id BIGINT NOT NULL,"
        "user_id BIGINT,"
        "movement_type VARCHAR(20) NOT NULL," 
        "quantity DECIMAL(15, 2) NOT NULL,"
        "reference_id BIGINT,"
        "reason TEXT,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE,"
        "FOREIGN KEY (location_id) REFERENCES locations(id) ON DELETE CASCADE,"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create stock_movements table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}