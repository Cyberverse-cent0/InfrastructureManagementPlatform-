# Inventory Service Dockerfile
FROM gcc:12

WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy source files
COPY src/ /app/src/
COPY config/ /app/config/

# Build the service
RUN gcc -o inventory_service \
    src/main.c \
    src/server.c \
    src/handlers/asset_handlers.c \
    src/handlers/stats_handlers.c \
    src/models/asset.c \
    src/repository/asset_repository.c \
    src/service/asset_service.c \
    src/utils/json_utils.c \
    -lpthread \
    -Wall -Wextra -O2

# Expose port
EXPOSE 8080

# Run the service
CMD ["./inventory_service"]