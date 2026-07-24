#!/bin/bash

# Inventory Microservice Startup Script

echo "Starting Inventory Microservice..."

# Check if the service is already compiled
if [ ! -f "./inventory_service" ]; then
    echo "Compiling the service..."
    make
fi

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Starting the service on port 8080..."
    ./inventory_service
else
    echo "Failed to compile the service."
    exit 1
fi
