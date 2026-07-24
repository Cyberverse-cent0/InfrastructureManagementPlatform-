# Core Backend Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
CONFIG_DIR = config

# Source files
MAIN_SRC = $(SRC_DIR)/main/main.c
NETWORK_SRC = $(SRC_DIR)/network/http_server.c
# DATABASE_SRC = $(SRC_DIR)/database/user_databse.c  # Disabled until PostgreSQL is available
# DATABASE_TABLES_SRC = $(SRC_DIR)/database/inventory_tables.c  # Disabled until PostgreSQL is available
LOGGER_SRC = $(SRC_DIR)/utils/logger.c
PRINT_FUNC_SRC = $(SRC_DIR)/utils/print_function.c

# Object files
MAIN_OBJ = $(BUILD_DIR)/main.o
NETWORK_OBJ = $(BUILD_DIR)/http_server.o
# DATABASE_OBJ = $(BUILD_DIR)/user_databse.o  # Disabled until PostgreSQL is available
# DATABASE_TABLES_OBJ = $(BUILD_DIR)/inventory_tables.o  # Disabled until PostgreSQL is available
UTILS_OBJS = $(BUILD_DIR)/logger.o $(BUILD_DIR)/print_function.o

# Targets
CORE_BACKEND_TARGET = $(BUILD_DIR)/core_backend

# PostgreSQL library
POSTGRES_LIBS = -lpq
POSTGRES_FLAGS = -DUSE_POSTGRES

# Default target
all: directories $(CORE_BACKEND_TARGET)

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)

# Core backend target
core_backend: directories $(CORE_BACKEND_TARGET)

$(CORE_BACKEND_TARGET): $(MAIN_OBJ) $(NETWORK_OBJ) $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

# Object files
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

$(NETWORK_OBJ): $(NETWORK_SRC) $(SRC_DIR)/network/http_server.h
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# $(DATABASE_OBJ): $(DATABASE_SRC) $(SRC_DIR)/database/global_lib.h
# 	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -I$(SRC_DIR) -c $< -o $@

# $(DATABASE_TABLES_OBJ): $(DATABASE_TABLES_SRC) $(SRC_DIR)/database/inventory_tables.h
# 	$(CC) $(CFLAGS) $(POSTGRES_FLAGS) -I$(SRC_DIR) -c $< -o $@

$(BUILD_DIR)/logger.o: $(LOGGER_SRC) $(SRC_DIR)/utils/logger.h
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

$(BUILD_DIR)/print_function.o: $(PRINT_FUNC_SRC) $(SRC_DIR)/utils/print_function.h
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Release build
release: CFLAGS += $(RELEASE_FLAGS)
release: clean all

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Help target
help:
	@echo "Core Backend Makefile"
	@echo "Available targets:"
	@echo "  all           - Build core backend (default)"
	@echo "  core_backend  - Build core backend"
	@echo "  debug         - Build with debug symbols"
	@echo "  release       - Build optimized release"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help message"

.PHONY: all core_backend debug release clean help directories