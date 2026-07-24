# Makefile for Infrastructure Management Platform Installer

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
DEBUG_FLAGS = -g -DDEBUG
LDFLAGS = 

# Source files
COMMON_SRC = common_utils.c
POSTGRESS_SRC = postgress_setup.c ../scipts/get_thead.c
PYTHON_SRC = python_setup.c ../scipts/get_thead.c

# Object files
COMMON_OBJ = $(COMMON_SRC:.c=.o)
POSTGRESS_OBJ = $(POSTGRESS_SRC:.c=.o)
PYTHON_OBJ = $(PYTHON_SRC:.c=.o)

# Executables
POSTGRESS_TARGET = postgres_installer
PYTHON_TARGET = python_installer

# Default target
all: $(POSTGRESS_TARGET) $(PYTHON_TARGET)

# PostgreSQL installer
$(POSTGRESS_TARGET): $(COMMON_OBJ) $(POSTGRESS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Python installer  
$(PYTHON_TARGET): $(COMMON_OBJ) $(PYTHON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Debug builds
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Clean build artifacts
clean:
	rm -f $(COMMON_OBJ) $(POSTGRESS_OBJ) $(PYTHON_OBJ)
	rm -f $(POSTGRESS_TARGET) $(PYTHON_TARGET)

# Install targets
install: all
	@echo "Installing PostgreSQL and Python setup tools..."
	@echo "Note: This requires root privileges"
	sudo cp $(POSTGRESS_TARGET) /usr/local/bin/
	sudo cp $(PYTHON_TARGET) /usr/local/bin/
	@echo "Installation complete"

# Uninstall targets
uninstall:
	@echo "Removing PostgreSQL and Python setup tools..."
	sudo rm -f /usr/local/bin/$(POSTGRESS_TARGET)
	sudo rm -f /usr/local/bin/$(PYTHON_TARGET)
	@echo "Uninstallation complete"

# Test targets
test: all
	@echo "Running basic tests..."
	./$(PYTHON_TARGET) --debug
	@echo "Tests complete"

.PHONY: all debug clean install uninstall test
