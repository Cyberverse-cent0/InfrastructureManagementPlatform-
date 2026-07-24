#ifndef INVENTORY_TABLES_H
#define INVENTORY_TABLES_H

#include "../global_lib.h"

// Core inventory tables
int create_categories_table(database_t *database);
int create_suppliers_table(database_t *database);
int create_locations_table(database_t *database);
int create_units_table(database_t *database);
int create_products_table(database_t *database);

// Stock management
int create_inventory_table(database_t *database);
int create_stock_movements_table(database_t *database);

// Product-supplier relationships
int create_product_suppliers_table(database_t *database);

// Purchasing
int create_purchase_orders_table(database_t *database);
int create_purchase_order_items_table(database_t *database);

// Sales
int create_sales_table(database_t *database);
int create_sale_items_table(database_t *database);

// Customers
int create_customers_table(database_t *database);

// Master function to create all inventory tables
int create_all_inventory_tables(database_t *database);

#endif // INVENTORY_TABLES_H