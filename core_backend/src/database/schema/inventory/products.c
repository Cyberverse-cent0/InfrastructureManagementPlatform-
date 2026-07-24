/*
products
├── id
├── sku
├── name
├── barcode
├── picture
├── description
├── category_id
├── supplier_id
├── unit_id
├── price
├── cost_price
├── is_active
├── created_at
└── updated_at
*/
#include "../global_lib.h"
#define MAX_PICTURE_SIZE 10485760 // 10MB

int create_products_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS products ("
        "id BIGSERIAL PRIMARY KEY,"
        "sku VARCHAR(50) UNIQUE NOT NULL,"
        "name VARCHAR(255) NOT NULL,"
        "barcode VARCHAR(50),"
        "picture VARCHAR(255),"
        "description TEXT,"
        "category_id BIGINT NOT NULL,"
        "supplier_id BIGINT NOT NULL,"
        "unit_id BIGINT NOT NULL,"
        "price DECIMAL(10, 2) NOT NULL,"
        "cost_price DECIMAL(10, 2) NOT NULL,"
        "is_active BOOLEAN NOT NULL DEFAULT TRUE,"
        "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE CASCADE,"
        "FOREIGN KEY (supplier_id) REFERENCES suppliers(id) ON DELETE CASCADE,"
        "FOREIGN KEY (unit_id) REFERENCES units(id) ON DELETE CASCADE"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create products table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}