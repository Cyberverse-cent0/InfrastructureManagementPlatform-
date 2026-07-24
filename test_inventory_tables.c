#include "src/backend/database/inventory_tables.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Inventory Management Platform - Database Table Creation Test\n");
    printf("=============================================================\n\n");

    // Initialize database connection
    const char *connection_string = "host=localhost port=5432 dbname=infrastructure_platform user=postgres password=postgres";
    
    database_t *database = database_init(connection_string);
    if (database == NULL) {
        printf("Failed to connect to database\n");
        return 1;
    }

    printf("Connected to database successfully\n\n");

    // Create all inventory tables
    if (create_all_inventory_tables(database)) {
        printf("\n✓ All inventory tables created successfully\n");
    } else {
        printf("\n✗ Failed to create inventory tables\n");
        database_disconnect(database);
        return 1;
    }

    // Disconnect from database
    database_disconnect(database);
    printf("\nDisconnected from database\n");

    return 0;
}