/*
sales
├── id
├── user_id
├── location_id
├── customer_id
├── status
├── total
├── created_at
└── updated_at
*/
#include "../global_lib.h"

int create_sales_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS sales ("
        "id BIGSERIAL PRIMARY KEY,"
        "user_id BIGINT NOT NULL,"
        "location_id BIGINT NOT NULL,"
        "customer_id BIGINT,"
        "status VARCHAR(20) NOT NULL DEFAULT 'PENDING',"
        "total DECIMAL(15, 2) NOT NULL DEFAULT 0,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,"
        "FOREIGN KEY (location_id) REFERENCES locations(id) ON DELETE CASCADE,"
        "FOREIGN KEY (customer_id) REFERENCES customers(id) ON DELETE SET NULL"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create sales table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}