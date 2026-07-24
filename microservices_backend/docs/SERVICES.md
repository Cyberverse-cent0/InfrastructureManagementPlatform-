# Microservices Backend API Documentation

## Overview
The Microservices Backend provides specialized services for the Infrastructure Management Platform. Each service handles a specific domain with independent deployment and scaling capabilities.

## Services Architecture

### Inventory Service
**Port:** 8080  
**Purpose:** Asset and inventory management  
**Base URL:** `http://localhost:8080`

#### Endpoints

##### GET /
Health check endpoint.

**Response:**
```json
{
  "status": "ok",
  "service": "inventory-microservice"
}
```

##### GET /health
Health check endpoint (same as GET /).

##### GET /api/assets
Get all assets with pagination.

**Query Parameters:**
- `page` (optional): Page number (default: 1)
- `limit` (optional): Items per page (default: 10)

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": "1",
      "name": "Web Server 01",
      "type": "server",
      "status": "active",
      "priority": "high",
      "location": "US-East-1",
      "cpu": 4,
      "memory": 16,
      "storage": 500,
      "ipAddress": "192.168.1.100",
      "macAddress": "00:1A:2B:3C:4D:5E",
      "operatingSystem": "Ubuntu 22.04",
      "createdAt": "2024-01-15T00:00:00Z",
      "updatedAt": "2024-07-20T00:00:00Z",
      "lastChecked": "2024-07-24T00:00:00Z",
      "tags": ["web", "production"],
      "notes": "Main web server for production traffic"
    }
  ],
  "pagination": {
    "page": 1,
    "limit": 10,
    "total": 2
  }
}
```

##### GET /api/assets/{id}
Get specific asset by ID.

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "1",
    "name": "Web Server 01",
    "type": "server",
    "status": "active",
    "priority": "high",
    "location": "US-East-1",
    "cpu": 4,
    "memory": 16,
    "storage": 500,
    "ipAddress": "192.168.1.100",
    "macAddress": "00:1A:2B:3C:4D:5E",
    "operatingSystem": "Ubuntu 22.04",
    "createdAt": "2024-01-15T00:00:00Z",
    "updatedAt": "2024-07-20T00:00:00Z",
    "lastChecked": "2024-07-24T00:00:00Z",
    "tags": ["web", "production"],
    "notes": "Main web server for production traffic"
  }
}
```

##### POST /api/assets
Create new asset.

**Request:**
```json
{
  "name": "Database Server 02",
  "type": "database",
  "status": "active",
  "priority": "critical",
  "location": "US-West-1",
  "cpu": 8,
  "memory": 32,
  "storage": 1000,
  "ipAddress": "192.168.1.102",
  "macAddress": "00:1A:2B:3C:4D:60",
  "operatingSystem": "CentOS 8",
  "tags": ["database", "production", "critical"],
  "notes": "Secondary database server"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "3",
    "name": "Database Server 02",
    "createdAt": "2024-07-24T00:00:00Z"
  }
}
```

##### PUT /api/assets/{id}
Update existing asset.

**Request:**
```json
{
  "name": "Updated Server Name",
  "status": "maintenance",
  "cpu": 12
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "1",
    "name": "Updated Server Name",
    "status": "maintenance",
    "updatedAt": "2024-07-24T00:00:00Z"
  }
}
```

##### DELETE /api/assets/{id}
Delete asset by ID.

**Response:**
```json
{
  "success": true,
  "message": "Asset deleted successfully"
}
```

##### GET /api/stats
Get inventory statistics.

**Response:**
```json
{
  "success": true,
  "data": {
    "totalAssets": 2,
    "activeAssets": 2,
    "inactiveAssets": 0,
    "maintenanceAssets": 0,
    "errorAssets": 0,
    "totalServers": 1,
    "totalNetwork": 0,
    "totalStorage": 0,
    "totalDatabases": 1,
    "avgCpuUsage": 65.5,
    "avgMemoryUsage": 72.3,
    "uptime": 99.9
  }
}
```

##### GET /api/stats/{type}
Get specific statistics type.

**Available types:**
- `overview` - General overview statistics
- `performance` - Performance metrics
- `capacity` - Capacity planning data
- `health` - Health status information

**Response:**
```json
{
  "success": true,
  "data": {
    "type": "overview",
    "metrics": {
      // Type-specific metrics
    }
  }
}
```

### User Service (Planned)
**Port:** 8081  
**Purpose:** User management and authentication  
**Status:** In Development

### Order Service (Planned)
**Port:** 8082  
**Purpose:** Order processing and management  
**Status:** In Development

### Notification Service (Planned)
**Port:** 8083  
**Purpose:** Notification and alert management  
**Status:** In Development

## Service Communication

### Inter-Service Communication
Services communicate via HTTP/REST API calls. Future implementations will include:
- Message queues for asynchronous communication
- Service discovery mechanism
- Load balancing between service instances

### API Gateway
An API Gateway will be implemented to:
- Route requests to appropriate services
- Handle authentication and authorization
- Provide rate limiting and throttling
- Aggregate responses from multiple services

## Error Handling

All services follow consistent error response format:

```json
{
  "success": false,
  "error": "Error message description"
}
```

### Common HTTP Status Codes
- `200 OK` - Request successful
- `201 Created` - Resource created successfully
- `400 Bad Request` - Invalid request data
- `404 Not Found` - Resource not found
- `500 Internal Server Error` - Service error
- `503 Service Unavailable` - Service temporarily down

## Service Discovery

Services will register with a service discovery mechanism (Consul, etcd, or custom implementation) to enable:
- Dynamic service registration
- Health checking
- Load balancing
- Failover handling

## Monitoring and Logging

Each service implements:
- Structured logging (JSON format)
- Health check endpoints
- Performance metrics collection
- Error tracking

## Deployment

### Docker Deployment
Each service includes a Dockerfile for containerization:
```bash
docker build -t inventory-service .
docker run -p 8080:8080 inventory-service
```

### Kubernetes Deployment
Kubernetes manifests will be provided for:
- Service deployment
- ConfigMaps and Secrets
- Service discovery
- Load balancing

## Scaling

Services can be scaled independently:
- Horizontal scaling via container orchestration
- Database connection pooling
- Caching layer integration
- Load balancing configuration

## Security

### Authentication
- JWT token validation
- Service-to-service authentication
- API key management

### Authorization
- Role-based access control (RBAC)
- Service-level permissions
- Resource-level permissions

### Encryption
- TLS/SSL for service communication
- Data encryption at rest
- Secure configuration management