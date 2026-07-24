# Inventory Service Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# Directories
SRC_DIR = src
BUILD_DIR = build
CONFIG_DIR = config

# Source files
MAIN_SRC = $(SRC_DIR)/main.c
SERVER_SRC = $(SRC_DIR)/server.c
ASSET_HANDLERS_SRC = $(SRC_DIR)/handlers/asset_handlers.c
STATS_HANDLERS_SRC = $(SRC_DIR)/handlers/stats_handlers.c
ASSET_MODEL_SRC = $(SRC_DIR)/models/asset.c
ASSET_REPOSITORY_SRC = $(SRC_DIR)/repository/asset_repository.c
ASSET_SERVICE_SRC = $(SRC_DIR)/service/asset_service.c
JSON_UTILS_SRC = $(SRC_DIR)/utils/json_utils.c

# Object files
MAIN_OBJ = $(BUILD_DIR)/main.o
SERVER_OBJ = $(BUILD_DIR)/server.o
ASSET_HANDLERS_OBJ = $(BUILD_DIR)/asset_handlers.o
STATS_HANDLERS_OBJ = $(BUILD_DIR)/stats_handlers.o
ASSET_MODEL_OBJ = $(BUILD_DIR)/asset.o
ASSET_REPOSITORY_OBJ = $(BUILD_DIR)/asset_repository.o
ASSET_SERVICE_OBJ = $(BUILD_DIR)/asset_service.o
JSON_UTILS_OBJ = $(BUILD_DIR)/json_utils.o

# Targets
INVENTORY_SERVICE_TARGET = $(BUILD_DIR)/inventory_service

# Default target
all: directories $(INVENTORY_SERVICE_TARGET)

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)

# Inventory service target
inventory_service: directories $(INVENTORY_SERVICE_TARGET)

$(INVENTORY_SERVICE_TARGET): $(MAIN_OBJ) $(SERVER_OBJ) $(ASSET_HANDLERS_OBJ) $(STATS_HANDLERS_OBJ) $(ASSET_MODEL_OBJ) $(ASSET_REPOSITORY_OBJ) $(ASSET_SERVICE_OBJ) $(JSON_UTILS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

# Object files
$(MAIN_OBJ): $(MAIN_SRC) $(SRC_DIR)/server.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC) $(SRC_DIR)/server.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASSET_HANDLERS_OBJ): $(ASSET_HANDLERS_SRC) $(SRC_DIR)/handlers/asset_handlers.h
	$(CC) $(CFLAGS) -c $< -o $@

$(STATS_HANDLERS_OBJ): $(STATS_HANDLERS_SRC) $(SRC_DIR)/handlers/stats_handlers.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASSET_MODEL_OBJ): $(ASSET_MODEL_SRC) $(SRC_DIR)/models/asset.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASSET_REPOSITORY_OBJ): $(ASSET_REPOSITORY_SRC) $(SRC_DIR)/repository/asset_repository.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASSET_SERVICE_OBJ): $(ASSET_SERVICE_SRC) $(SRC_DIR)/service/asset_service.h
	$(CC) $(CFLAGS) -c $< -o $@

$(JSON_UTILS_OBJ): $(JSON_UTILS_SRC) $(SRC_DIR)/utils/json_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

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
	@echo "Inventory Service Makefile"
	@echo "Available targets:"
	@echo "  all               - Build inventory service (default)"
	@echo "  inventory_service - Build inventory service"
	@echo "  debug             - Build with debug symbols"
	@echo "  release           - Build optimized release"
	@echo "  clean             - Remove build artifacts"
	@echo "  help              - Show this help message"

.PHONY: all inventory_service debug release clean help directories