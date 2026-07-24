#include "inventory_tables.h"

int create_all_inventory_tables(database_t *database) {
    if (database == NULL || database->connection == NULL) {
        print_error("Invalid database connection");
        return false;
    }

    print_info("Creating inventory tables...");

    // Create core tables in order (respecting foreign key dependencies)
    if (!create_categories_table(database)) {
        print_error("Failed to create categories table");
        return false;
    }
    print_info("✓ Categories table created");

    if (!create_suppliers_table(database)) {
        print_error("Failed to create suppliers table");
        return false;
    }
    print_info("✓ Suppliers table created");

    if (!create_locations_table(database)) {
        print_error("Failed to create locations table");
        return false;
    }
    print_info("✓ Locations table created");

    if (!create_units_table(database)) {
        print_error("Failed to create units table");
        return false;
    }
    print_info("✓ Units table created");

    if (!create_customers_table(database)) {
        print_error("Failed to create customers table");
        return false;
    }
    print_info("✓ Customers table created");

    if (!create_products_table(database)) {
        print_error("Failed to create products table");
        return false;
    }
    print_info("✓ Products table created");

    if (!create_inventory_table(database)) {
        print_error("Failed to create inventory table");
        return false;
    }
    print_info("✓ Inventory table created");

    if (!create_stock_movements_table(database)) {
        print_error("Failed to create stock_movements table");
        return false;
    }
    print_info("✓ Stock movements table created");

    if (!create_product_suppliers_table(database)) {
        print_error("Failed to create product_suppliers table");
        return false;
    }
    print_info("✓ Product suppliers table created");

    if (!create_purchase_orders_table(database)) {
        print_error("Failed to create purchase_orders table");
        return false;
    }
    print_info("✓ Purchase orders table created");

    if (!create_purchase_order_items_table(database)) {
        print_error("Failed to create purchase_order_items table");
        return false;
    }
    print_info("✓ Purchase order items table created");

    if (!create_sales_table(database)) {
        print_error("Failed to create sales table");
        return false;
    }
    print_info("✓ Sales table created");

    if (!create_sale_items_table(database)) {
        print_error("Failed to create sale_items table");
        return false;
    }
    print_info("✓ Sale items table created");

    print_success("All inventory tables created successfully");
    return true;
}