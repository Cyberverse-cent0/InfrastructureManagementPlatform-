/*
product_suppliers
├── product_id
├── supplier_id
├── supplier_sku
├── cost_price
└── lead_time
*/
#include "../global_lib.h"

int create_product_suppliers_table(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    const char *query =
        "CREATE TABLE IF NOT EXISTS product_suppliers ("
        "product_id BIGINT NOT NULL,"
        "supplier_id BIGINT NOT NULL,"
        "supplier_sku VARCHAR(50),"
        "cost_price DECIMAL(10, 2),"
        "lead_time INTEGER," 
        "PRIMARY KEY (product_id, supplier_id),"
        "FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE,"
        "FOREIGN KEY (supplier_id) REFERENCES suppliers(id) ON DELETE CASCADE"
        ");";

    PGresult *result = PQexec(database->connection, query);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        print_error(
            "Failed to create product_suppliers table: %s",
            PQerrorMessage(database->connection)
        );
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}