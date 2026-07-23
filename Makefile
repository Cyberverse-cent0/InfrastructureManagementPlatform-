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
BUILD_DIR = build
TEST_DIR = test

# Source files
LOGGER_SRC = $(SRC_DIR)/logger.c
PRINT_FUNC_SRC = $(SRC_DIR)/print_function.c
USER_DB_SRC = $(SRC_DIR)/user_databse.c
GET_THREAD_SRC = $(SRC_DIR)/get_thead.c
SMART_LOGGER_SRC = $(SRC_DIR)/smart_logger.c
ASYNC_LOGGER_SRC = $(SRC_DIR)/async_logger.c
INSTALLER_SRC = $(INSTALLER_DIR)/postgress_setup.c

# Object files
LOGGER_OBJ = $(BUILD_DIR)/logger.o
PRINT_FUNC_OBJ = $(BUILD_DIR)/print_function.o
USER_DB_OBJ = $(BUILD_DIR)/user_databse.o
GET_THREAD_OBJ = $(BUILD_DIR)/get_thead.o
SMART_LOGGER_OBJ = $(BUILD_DIR)/smart_logger.o
ASYNC_LOGGER_OBJ = $(BUILD_DIR)/async_logger.o
INSTALLER_OBJ = $(BUILD_DIR)/postgress_setup.o

# Targets
INSTALLER_TARGET = $(BUILD_DIR)/installer$(EXE_EXT)
LOGGER_TARGET = $(BUILD_DIR)/logger_test$(EXE_EXT)
PRINT_FUNC_TARGET = $(BUILD_DIR)/print_test$(EXE_EXT)

# PostgreSQL library (optional)
ifdef USE_POSTGRES
    POSTGRES_LIBS = -lpq
    POSTGRES_FLAGS = -DUSE_POSTGRES
else
    POSTGRES_LIBS =
    POSTGRES_FLAGS =
endif

# Default target
all: directories $(INSTALLER_TARGET)

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

# Dependency management (example for package managers)
depedecy_manager:
	@echo "Dependency management:"
	@echo "Linux (Arch): sudo pacman -S postgresql"
	@echo "Linux (Ubuntu): sudo apt-get install libpq-dev"
	@echo "Windows: Download PostgreSQL installer from postgresql.org"

# Install dependencies
install_deps:
ifdef OS
	@echo "Please install PostgreSQL manually on Windows"
else
	@if command -v pacman >/dev/null 2>&1; then \
		echo "Detected Arch Linux"; \
		sudo pacman -S --needed postgresql; \
	elif command -v apt-get >/dev/null 2>&1; then \
		echo "Detected Debian/Ubuntu"; \
		sudo apt-get update && sudo apt-get install -y libpq-dev; \
	else \
		echo "Unknown package manager. Please install PostgreSQL manually."; \
	fi
endif

# Frontend placeholder
frontend:
	@echo "Frontend compilation not yet implemented"

frontend_run:
	@echo "Frontend run not yet implemented"

# Backend placeholder
backend:
	@echo "Backend compilation not yet implemented"

backend_run:
	@echo "Backend run not yet implemented"

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build the installer (default)"
	@echo "  installer     - Build the installer"
	@echo "  debug         - Build with debug symbols"
	@echo "  release       - Build optimized release"
	@echo "  test_compile  - Compile test programs"
	@echo "  test_run      - Compile and run tests"
	@echo "  clean         - Remove build artifacts"
	@echo "  install_deps  - Install system dependencies"
	@echo "  help          - Show this help message"

.PHONY: all installer debug release test_compile test_run clean depedecy_manager install_deps frontend frontend_run backend backend_run help directories
	