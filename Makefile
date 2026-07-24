# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# Platform detection
ifdef OS
    # Windows with MinGW
    RM = del /Q
    MKDIR = mkdir
    EXE_EXT = .exe
    PLATFORM_LIBS = -lws2_32
else
    # Linux/Unix
    RM = rm -f
    MKDIR = mkdir -p
    EXE_EXT =
    PLATFORM_LIBS = -lpthread
endif

# Directories
SRC_DIR = scipts
INSTALLER_DIR = installer
BACKEND_DIR = src/backend/core
DATABASE_DIR = src/backend/database
TABLES_DIR = src/backend/database/tables
BUILD_DIR = build
TEST_DIR = test
FRONTEND_DIR = frontend
ANDROID_APP_DIR = $(FRONTEND_DIR)/android_app
ANDROID_MOBILE_DIR = $(ANDROID_APP_DIR)/artifacts/infrastructure-mobile

# Source files
LOGGER_SRC = $(SRC_DIR)/logger.c
PRINT_FUNC_SRC = $(SRC_DIR)/print_function.c
USER_DB_SRC = $(SRC_DIR)/user_databse.c
GET_THREAD_SRC = $(SRC_DIR)/get_thead.c
SMART_LOGGER_SRC = $(SRC_DIR)/smart_logger.c
ASYNC_LOGGER_SRC = $(SRC_DIR)/async_logger.c
INSTALLER_SRC = $(INSTALLER_DIR)/postgress_setup.c
HTTP_SERVER_SRC = $(BACKEND_DIR)/http_server.c
DATABASE_SRC = $(DATABASE_DIR)/user_databse.c
INVENTORY_TABLES_SRC = $(DATABASE_DIR)/inventory_tables.c
PRODUCTS_SRC = $(TABLES_DIR)/products.c
CATEGORIES_SRC = $(TABLES_DIR)/categories.c
SUPPLIERS_SRC = $(TABLES_DIR)/suppliers.c
LOCATIONS_SRC = $(TABLES_DIR)/locations.c
UNITS_SRC = $(TABLES_DIR)/units.c
INVENTORY_SRC = $(TABLES_DIR)/inventory.c
STOCK_MOVEMENTS_SRC = $(TABLES_DIR)/stock_movements.c
PRODUCT_SUPPLIERS_SRC = $(TABLES_DIR)/product_suppliers.c
PURCHASE_ORDERS_SRC = $(TABLES_DIR)/purchase_orders.c
PURCHASE_ORDER_ITEMS_SRC = $(TABLES_DIR)/purchase_order_items.c
SALES_SRC = $(TABLES_DIR)/sales.c
SALE_ITEMS_SRC = $(TABLES_DIR)/sale_items.c
CUSTOMERS_SRC = $(TABLES_DIR)/customers.c

# Object files
LOGGER_OBJ = $(BUILD_DIR)/logger.o
PRINT_FUNC_OBJ = $(BUILD_DIR)/print_function.o
USER_DB_OBJ = $(BUILD_DIR)/user_databse.o
GET_THREAD_OBJ = $(BUILD_DIR)/get_thead.o
SMART_LOGGER_OBJ = $(BUILD_DIR)/smart_logger.o
ASYNC_LOGGER_OBJ = $(BUILD_DIR)/async_logger.o
INSTALLER_OBJ = $(BUILD_DIR)/postgress_setup.o
HTTP_SERVER_OBJ = $(BUILD_DIR)/http_server.o
DATABASE_OBJ = $(BUILD_DIR)/database.o
INVENTORY_TABLES_OBJ = $(BUILD_DIR)/inventory_tables.o
PRODUCTS_OBJ = $(BUILD_DIR)/products.o
CATEGORIES_OBJ = $(BUILD_DIR)/categories.o
SUPPLIERS_OBJ = $(BUILD_DIR)/suppliers.o
LOCATIONS_OBJ = $(BUILD_DIR)/locations.o
UNITS_OBJ = $(BUILD_DIR)/units.o
INVENTORY_OBJ = $(BUILD_DIR)/inventory.o
STOCK_MOVEMENTS_OBJ = $(BUILD_DIR)/stock_movements.o
PRODUCT_SUPPLIERS_OBJ = $(BUILD_DIR)/product_suppliers.o
PURCHASE_ORDERS_OBJ = $(BUILD_DIR)/purchase_orders.o
PURCHASE_ORDER_ITEMS_OBJ = $(BUILD_DIR)/purchase_order_items.o
SALES_OBJ = $(BUILD_DIR)/sales.o
SALE_ITEMS_OBJ = $(BUILD_DIR)/sale_items.o
CUSTOMERS_OBJ = $(BUILD_DIR)/customers.o

# Targets
INSTALLER_TARGET = $(BUILD_DIR)/installer$(EXE_EXT)
LOGGER_TARGET = $(BUILD_DIR)/logger_test$(EXE_EXT)
PRINT_FUNC_TARGET = $(BUILD_DIR)/print_test$(EXE_EXT)
HTTP_SERVER_TARGET = $(BUILD_DIR)/http_server$(EXE_EXT)
NETWORK_TEST_TARGET = $(BUILD_DIR)/test_network$(EXE_EXT)

# PostgreSQL library (required for inventory system)
POSTGRES_LIBS = -lpq
POSTGRES_FLAGS = -DUSE_POSTGRES

# Default target
all: directories $(INSTALLER_TARGET)

# Inventory tables target
inventory_tables: directories $(BUILD_DIR)/inventory_tables$(EXE_EXT)

$(BUILD_DIR)/inventory_tables$(EXE_EXT): $(INVENTORY_TABLES_OBJ) $(PRODUCTS_OBJ) $(CATEGORIES_OBJ) $(SUPPLIERS_OBJ) $(LOCATIONS_OBJ) $(UNITS_OBJ) $(INVENTORY_OBJ) $(STOCK_MOVEMENTS_OBJ) $(PRODUCT_SUPPLIERS_OBJ) $(PURCHASE_ORDERS_OBJ) $(PURCHASE_ORDER_ITEMS_OBJ) $(SALES_OBJ) $(SALE_ITEMS_OBJ) $(CUSTOMERS_OBJ) $(DATABASE_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -o $@ $^ $(PLATFORM_LIBS) $(POSTGRES_LIBS)

# Test inventory tables
test_inventory: directories $(BUILD_DIR)/test_inventory$(EXE_EXT)

$(BUILD_DIR)/test_inventory$(EXE_EXT): test_inventory_tables.c $(INVENTORY_TABLES_OBJ) $(PRODUCTS_OBJ) $(CATEGORIES_OBJ) $(SUPPLIERS_OBJ) $(LOCATIONS_OBJ) $(UNITS_OBJ) $(INVENTORY_OBJ) $(STOCK_MOVEMENTS_OBJ) $(PRODUCT_SUPPLIERS_OBJ) $(PURCHASE_ORDERS_OBJ) $(PURCHASE_ORDER_ITEMS_OBJ) $(SALES_OBJ) $(SALE_ITEMS_OBJ) $(CUSTOMERS_OBJ) $(DATABASE_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -o $@ $< $(INVENTORY_TABLES_OBJ) $(PRODUCTS_OBJ) $(CATEGORIES_OBJ) $(SUPPLIERS_OBJ) $(LOCATIONS_OBJ) $(UNITS_OBJ) $(INVENTORY_OBJ) $(STOCK_MOVEMENTS_OBJ) $(PRODUCT_SUPPLIERS_OBJ) $(PURCHASE_ORDERS_OBJ) $(PURCHASE_ORDER_ITEMS_OBJ) $(SALES_OBJ) $(SALE_ITEMS_OBJ) $(CUSTOMERS_OBJ) $(DATABASE_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(PLATFORM_LIBS) $(POSTGRES_LIBS)

# Create build directories
directories:
	$(MKDIR) $(BUILD_DIR)

# Installer target
installer: directories $(INSTALLER_TARGET)

$(INSTALLER_TARGET): $(INSTALLER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(GET_THREAD_OBJ)
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -o $@ $^ $(PLATFORM_LIBS) $(POSTGRES_LIBS)

# Test target with smart logger
test_smart: directories $(BUILD_DIR)/test_smart

$(BUILD_DIR)/test_smart: test_improved.c $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(USER_DB_OBJ) $(GET_THREAD_OBJ) $(SMART_LOGGER_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(USER_DB_OBJ) $(GET_THREAD_OBJ) $(SMART_LOGGER_OBJ) $(PLATFORM_LIBS)

# Test target for async logging
test_async: directories $(BUILD_DIR)/test_async

$(BUILD_DIR)/test_async: test_async_logging.c $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(USER_DB_OBJ) $(GET_THREAD_OBJ) $(SMART_LOGGER_OBJ) $(ASYNC_LOGGER_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(USER_DB_OBJ) $(GET_THREAD_OBJ) $(SMART_LOGGER_OBJ) $(ASYNC_LOGGER_OBJ) $(PLATFORM_LIBS)

# Test target for threading integration
test_threading: directories $(BUILD_DIR)/test_threading

$(BUILD_DIR)/test_threading: test_threading_integration.c $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(USER_DB_OBJ) $(GET_THREAD_OBJ) $(SMART_LOGGER_OBJ) $(ASYNC_LOGGER_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(USER_DB_OBJ) $(GET_THREAD_OBJ) $(SMART_LOGGER_OBJ) $(ASYNC_LOGGER_OBJ) $(PLATFORM_LIBS)

# HTTP server target
http_server: directories $(HTTP_SERVER_TARGET)

$(HTTP_SERVER_TARGET): $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(PLATFORM_LIBS)

# Network interface test
test_network: directories $(NETWORK_TEST_TARGET)

$(NETWORK_TEST_TARGET): test_network_interface.c $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(PLATFORM_LIBS)

# Server logging test
test_logging: directories $(BUILD_DIR)/test_logging

$(BUILD_DIR)/test_logging: test_server_logging.c $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(PLATFORM_LIBS)

# HTTP parsing test
test_http: directories $(BUILD_DIR)/test_http

$(BUILD_DIR)/test_http: test_http_parsing.c $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(PLATFORM_LIBS)

# Routing test
test_routing: directories $(BUILD_DIR)/test_routing

$(BUILD_DIR)/test_routing: test_routing.c $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(HTTP_SERVER_OBJ) $(LOGGER_OBJ) $(PRINT_FUNC_OBJ) $(PLATFORM_LIBS)

# Object files
$(INSTALLER_OBJ): $(INSTALLER_SRC) $(INSTALLER_DIR)/postgress_setup.h $(SRC_DIR)/get_thead.h
	$(CC) $(CFLAGS) -c $< -o $@

$(LOGGER_OBJ): $(LOGGER_SRC) $(SRC_DIR)/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PRINT_FUNC_OBJ): $(PRINT_FUNC_SRC) $(SRC_DIR)/print_function.h $(SRC_DIR)/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(USER_DB_OBJ): $(USER_DB_SRC) $(SRC_DIR)/user_databse.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(GET_THREAD_OBJ): $(GET_THREAD_SRC) $(SRC_DIR)/get_thead.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SMART_LOGGER_OBJ): $(SMART_LOGGER_SRC) $(SRC_DIR)/smart_logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASYNC_LOGGER_OBJ): $(ASYNC_LOGGER_SRC) $(SRC_DIR)/async_logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(HTTP_SERVER_OBJ): $(HTTP_SERVER_SRC) $(BACKEND_DIR)/http_server.h $(LOGGER_OBJ) $(PRINT_FUNC_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# Database object files
$(DATABASE_OBJ): $(DATABASE_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(INVENTORY_TABLES_OBJ): $(INVENTORY_TABLES_SRC) $(DATABASE_DIR)/inventory_tables.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(PRODUCTS_OBJ): $(PRODUCTS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(CATEGORIES_OBJ): $(CATEGORIES_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(SUPPLIERS_OBJ): $(SUPPLIERS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(LOCATIONS_OBJ): $(LOCATIONS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(UNITS_OBJ): $(UNITS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(INVENTORY_OBJ): $(INVENTORY_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(STOCK_MOVEMENTS_OBJ): $(STOCK_MOVEMENTS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(PRODUCT_SUPPLIERS_OBJ): $(PRODUCT_SUPPLIERS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(PURCHASE_ORDERS_OBJ): $(PURCHASE_ORDERS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(PURCHASE_ORDER_ITEMS_OBJ): $(PURCHASE_ORDER_ITEMS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(SALES_OBJ): $(SALES_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(SALE_ITEMS_OBJ): $(SALE_ITEMS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

$(CUSTOMERS_OBJ): $(CUSTOMERS_SRC) $(DATABASE_DIR)/global_lib.h
	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -c $< -o $@

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Release build
release: CFLAGS += $(RELEASE_FLAGS)
release: clean all

# Test targets
test_compile: directories
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test_user_db $(USER_DB_OBJ) $(LOGGER_OBJ) $(POSTGRES_LIBS)

test_run: test_compile
	$(BUILD_DIR)/test_user_db

# Clean build artifacts
clean:
	$(RM) $(BUILD_DIR)/*.o
	$(RM) $(BUILD_DIR)/*$(EXE_EXT)
	$(RM) $(BUILD_DIR)/*.log
	@echo "Cleaning C build artifacts..."
	@echo "Cleaning frontend artifacts..."
	cd $(FRONTEND_DIR) && rm -rf .next node_modules
	@echo "Cleaning Android app artifacts..."
	cd $(ANDROID_APP_DIR) && rm -rf node_modules .local
	cd $(ANDROID_MOBILE_DIR) && rm -rf node_modules .expo dist
	@echo "Cleaning core backend artifacts..."
	cd core_backend && $(MAKE) clean || true
	@echo "Cleaning microservices artifacts..."
	cd microservices_backend/services/inventory_service && $(MAKE) clean || true

# Dependency management (example for package managers)
depedecy_manager:
	@echo "Dependency management:"
	@echo "Linux (Arch): sudo pacman -S postgresql"
	@echo "Linux (Ubuntu): sudo apt-get install libpq-dev"
	@echo "Windows: Download PostgreSQL installer from postgresql.org"

# Install dependencies
install_deps:
	@echo "Installing system dependencies..."
ifdef OS
	@echo "Please install PostgreSQL, Node.js, and pnpm manually on Windows"
else
	@if command -v pacman >/dev/null 2>&1; then \
		echo "Detected Arch Linux"; \
		sudo pacman -S --needed postgresql nodejs pnpm gcc make; \
	elif command -v apt-get >/dev/null 2>&1; then \
		echo "Detected Debian/Ubuntu"; \
		sudo apt-get update; \
		sudo apt-get install -y postgresql libpq-dev nodejs npm gcc make; \
		npm install -g pnpm; \
	else \
		echo "Unknown package manager. Please install PostgreSQL, Node.js, pnpm, gcc, and make manually."; \
	fi
	@echo "System dependencies installation complete"
	@echo "Please run 'make android_install' to install Android app dependencies"
endif

# Frontend targets
frontend: frontend_web frontend_android
	@echo "Frontend compilation complete"

frontend_web:
	@echo "Building Next.js web frontend..."
	cd $(FRONTEND_DIR) && npm install && npm run build

frontend_run_web:
	@echo "Running Next.js web frontend..."
	cd $(FRONTEND_DIR) && npm run dev

# Android app targets
frontend_android: android_install android_build
	@echo "Android app compilation complete"

android_install:
	@echo "Installing Android app dependencies..."
	cd $(ANDROID_APP_DIR) && pnpm install

android_build:
	@echo "Building Android app..."
	cd $(ANDROID_MOBILE_DIR) && pnpm run build

android_run:
	@echo "Running Android app in development mode..."
	cd $(ANDROID_MOBILE_DIR) && pnpm run dev

android_serve:
	@echo "Serving Android app..."
	cd $(ANDROID_MOBILE_DIR) && pnpm run serve

android_typecheck:
	@echo "Type checking Android app..."
	cd $(ANDROID_APP_DIR) && pnpm run typecheck

android_clean:
	@echo "Cleaning Android app build artifacts..."
	cd $(ANDROID_APP_DIR) && rm -rf node_modules .local
	cd $(ANDROID_MOBILE_DIR) && rm -rf node_modules .expo dist

# Backend targets
backend: core_backend microservices
	@echo "Backend compilation complete"

core_backend:
	@echo "Building core backend..."
	cd core_backend && $(MAKE)

core_backend_run:
	@echo "Running core backend..."
	cd core_backend && $(MAKE) run

microservices:
	@echo "Building microservices..."
	cd microservices_backend/services/inventory_service && $(MAKE)

microservices_run:
	@echo "Running microservices..."
	cd microservices_backend/services/inventory_service && $(MAKE) run

# Help target
help:
	@echo "Available targets:"
	@echo "  all                - Build the installer (default)"
	@echo "  installer          - Build the installer"
	@echo "  http_server        - Build the HTTP server"
	@echo "  inventory_tables   - Build inventory tables system"
	@echo "  test_inventory     - Build and test inventory tables"
	@echo "  test_network       - Build and test network interface discovery"
	@echo "  test_logging       - Build and test server logging functionality"
	@echo "  test_http          - Build and test HTTP parsing functionality"
	@echo "  test_routing       - Build and test routing system"
	@echo "  debug              - Build with debug symbols"
	@echo "  release            - Build optimized release"
	@echo "  test_compile       - Compile test programs"
	@echo "  test_run           - Compile and run tests"
	@echo "  clean              - Remove build artifacts"
	@echo "  install_deps       - Install system dependencies"
	@echo "  frontend           - Build all frontend components (web + android)"
	@echo "  frontend_web       - Build Next.js web frontend"
	@echo "  frontend_run_web   - Run Next.js web frontend in development mode"
	@echo "  frontend_android   - Build Android app"
	@echo "  android_install    - Install Android app dependencies"
	@echo "  android_build      - Build Android app"
	@echo "  android_run        - Run Android app in development mode"
	@echo "  android_serve      - Serve Android app"
	@echo "  android_typecheck  - Type check Android app"
	@echo "  android_clean      - Clean Android app build artifacts"
	@echo "  backend            - Build all backend components (core + microservices)"
	@echo "  core_backend       - Build core backend"
	@echo "  core_backend_run   - Run core backend"
	@echo "  microservices      - Build microservices"
	@echo "  microservices_run  - Run microservices"
	@echo "  help               - Show this help message"

.PHONY: all installer http_server inventory_tables test_inventory test_network test_logging test_http test_routing debug release test_compile test_run clean depedecy_manager install_deps frontend frontend_web frontend_run_web frontend_android android_install android_build android_run android_serve android_typecheck android_clean backend core_backend core_backend_run microservices microservices_run help directories
	