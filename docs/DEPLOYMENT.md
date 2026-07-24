# Deployment Guide

This guide covers deployment scenarios for the Infrastructure Management Platform, including single-machine and distributed deployments with dynamic service discovery.

## Table of Contents

- [Overview](#overview)
- [Deployment Scenarios](#deployment-scenarios)
- [Configuration System](#configuration-system)
- [Service Discovery](#service-discovery)
- [Single-Machine Deployment](#single-machine-deployment)
- [Distributed Deployment](#distributed-deployment)
- [Configuration Management](#configuration-management)
- [Troubleshooting](#troubleshooting)

## Overview

The Infrastructure Management Platform supports flexible deployment scenarios:

- **Single-Machine**: All services run on the same machine using localhost
- **Distributed**: Services run on different machines with configurable IP addresses/domain names
- **Flexible Mixed**: Some services on same machine, others distributed

The platform uses a centralized configuration system (`config/platform_config.json`) and service discovery for dynamic service connection.

## Deployment Scenarios

### Architecture Components

1. **API Gateway** (`src/main.py`) - Central gateway for frontend-backend communication
2. **Core Backend** (`core_backend/src/main/main.c`) - C-based core backend service
3. **Inventory Service** (`microservices_backend/services/inventory_service/`) - Inventory microservice
4. **PostgreSQL Database** - Data storage
5. **Frontend** (`frontend/`) - Next.js frontend application

### Communication Flow

```
Frontend → API Gateway → Service Discovery → Backend Services
                                  ↓
                         Service Registry
```

## Configuration System

### Platform Configuration File

The main configuration file is `config/platform_config.json`:

```json
{
  "version": "1.0.0",
  "environment": "development",
  "deployment_mode": "flexible",
  "services": {
    "gateway": {
      "name": "api-gateway",
      "host": "localhost",
      "port": 8000,
      "protocol": "http",
      "health_check_endpoint": "/health"
    },
    "core_backend": {
      "name": "core-backend",
      "host": "localhost",
      "port": 8080,
      "protocol": "http",
      "health_check_endpoint": "/health"
    },
    "inventory_service": {
      "name": "inventory-microservice",
      "host": "localhost",
      "port": 8081,
      "protocol": "http",
      "health_check_endpoint": "/health"
    }
  }
}
```

### Environment Variables

Environment variables override configuration file settings:

```bash
# Service Discovery
SERVICE_DISCOVERY_ENABLED=true
CONFIG_FILE=config/platform_config.json

# Service Host Configuration
CORE_BACKEND_HOST=192.168.1.100
INVENTORY_SERVICE_HOST=192.168.1.101
POSTGRES_HOST=192.168.1.102
FRONTEND_HOST=192.168.1.103
GATEWAY_HOST=192.168.1.104

# Gateway Configuration
GATEWAY_HOST=0.0.0.0
GATEWAY_PORT=8000
DEBUG=true

# Database Configuration
DATABASE_HOST=localhost
DATABASE_PORT=5432
DATABASE_NAME=infrastructure_platform
DATABASE_USER=infrastructure_user
DATABASE_PASSWORD=secure_password
```

## Service Discovery

### Overview

The service discovery system (`src/service_discovery.py`) provides:

- **Dynamic Service Registration**: Services register themselves on startup
- **Health Checking**: Periodic health checks for all registered services
- **Service Lookup**: Services can discover each other dynamically
- **Configuration Integration**: Auto-loads services from platform configuration

### Service Registry

Services are stored in `data/service_registry.json`:

```json
{
  "inventory-microservice_localhost_8081": {
    "service_id": "inventory-microservice_localhost_8081",
    "service_name": "inventory-microservice",
    "host": "localhost",
    "port": 8081,
    "protocol": "http",
    "health_check_endpoint": "/health",
    "status": "healthy",
    "last_heartbeat": "2024-07-24T10:30:00"
  }
}
```

### Health Checking

- **Interval**: 30 seconds (configurable)
- **Timeout**: 120 seconds before service considered dead
- **Endpoint**: `/health` (configurable per service)

## Single-Machine Deployment

### Prerequisites

- Python 3.8+
- Node.js 18+
- PostgreSQL 12+
- GCC compiler (for C services)

### Configuration

Use the default `config/platform_config.json` with localhost addresses:

```bash
# All services use localhost
CORE_BACKEND_HOST=localhost
INVENTORY_SERVICE_HOST=localhost
POSTGRES_HOST=localhost
```

### Startup Sequence

1. **Start PostgreSQL**
   ```bash
   sudo systemctl start postgresql
   ```

2. **Start Core Backend**
   ```bash
   cd core_backend
   ./build/core_backend
   ```

3. **Start Inventory Service**
   ```bash
   cd microservices_backend/services/inventory_service
   ./build/inventory_service
   ```

4. **Start API Gateway**
   ```bash
   cd src
   python main.py
   ```

5. **Start Frontend**
   ```bash
   cd frontend
   npm run dev
   ```

### Verification

```bash
# Test gateway health
curl http://localhost:8000/health

# Test service discovery
python scripts/config_manager.py list-discovered

# Test specific service health
python scripts/config_manager.py health inventory-microservice
```

## Distributed Deployment

### Network Requirements

- All machines must be able to communicate via TCP/IP
- Firewall rules must allow service ports
- DNS resolution or IP addresses must be configured

### Configuration

Generate distributed deployment configuration:

```bash
python scripts/config_manager.py generate distributed config/distributed.json
```

This creates a configuration with environment variable placeholders:

```json
{
  "services": {
    "gateway": {
      "host": "$GATEWAY_HOST",
      "port": 8000
    },
    "core_backend": {
      "host": "$CORE_BACKEND_HOST",
      "port": 8080
    }
  }
}
```

### Environment Setup

On each machine, set the appropriate environment variables:

**Gateway Machine** (`192.168.1.104`):
```bash
export GATEWAY_HOST=192.168.1.104
export CORE_BACKEND_HOST=192.168.1.100
export INVENTORY_SERVICE_HOST=192.168.1.101
export POSTGRES_HOST=192.168.1.102
```

**Core Backend Machine** (`192.168.1.100`):
```bash
export CORE_BACKEND_HOST=192.168.1.100
export DATABASE_HOST=192.168.1.102
```

**Inventory Service Machine** (`192.168.1.101`):
```bash
export INVENTORY_SERVICE_HOST=192.168.1.101
export DATABASE_HOST=192.168.1.102
```

### Startup Sequence

1. **Start PostgreSQL** on database machine
2. **Start Core Backend** on backend machine
3. **Start Inventory Service** on inventory machine
4. **Start API Gateway** on gateway machine
5. **Start Frontend** on frontend machine

### Service Registration

Services automatically register with the service discovery system on startup if `SERVICE_DISCOVERY_ENABLED=true`.

Manual registration:

```bash
python scripts/config_manager.py register-service inventory-microservice http://192.168.1.101:8081
```

## Configuration Management

### Configuration Manager CLI

The `scripts/config_manager.py` tool provides configuration management:

```bash
# Validate configuration
python scripts/config_manager.py validate

# List configured services
python scripts/config_manager.py list

# List discovered services
python scripts/config_manager.py list-discovered

# Register a service
python scripts/config_manager.py register-service <name> <url>

# Unregister a service
python scripts/config_manager.py unregister-service <name>

# Check service health
python scripts/config_manager.py health <service-name>

# Reload configuration
python scripts/config_manager.py reload

# Generate deployment configuration
python scripts/config_manager.py generate single_machine config/single_machine.json
python scripts/config_manager.py generate distributed config/distributed.json

# Test gateway connection
python scripts/config_manager.py test-gateway
```

### Gateway Configuration Endpoints

The API Gateway provides admin endpoints for configuration management:

```bash
# Get current configuration (admin only)
curl -H "Authorization: Bearer <admin-token>" http://localhost:8000/admin/config

# Reload configuration (admin only)
curl -X POST -H "Authorization: Bearer <admin-token>" http://localhost:8000/admin/config/reload
```

### Frontend Configuration

Frontend uses environment variables for API endpoints:

```bash
# frontend/.env.local
NEXT_PUBLIC_GATEWAY_URL=http://192.168.1.104:8000
NEXT_PUBLIC_CORE_BACKEND_URL=http://192.168.1.100:8080
NEXT_PUBLIC_INVENTORY_SERVICE_URL=http://192.168.1.101:8081
NEXT_PUBLIC_SERVICE_DISCOVERY_ENABLED=true
```

## Troubleshooting

### Service Discovery Issues

**Problem**: Services not discovering each other

**Solutions**:
1. Check service discovery is enabled: `SERVICE_DISCOVERY_ENABLED=true`
2. Verify service registry file: `data/service_registry.json`
3. Check network connectivity between machines
4. Review service discovery logs: `logs/service_discovery.log`

**Manual Registration**:
```bash
python scripts/config_manager.py register-service <name> <url>
```

### Connection Issues

**Problem**: Cannot connect to services

**Solutions**:
1. Verify service is running: `curl http://<host>:<port>/health`
2. Check firewall rules: `sudo ufw status`
3. Verify configuration: `python scripts/config_manager.py validate`
4. Test gateway connection: `python scripts/config_manager.py test-gateway`

### Configuration Issues

**Problem**: Configuration not loading correctly

**Solutions**:
1. Validate configuration file: `python scripts/config_manager.py validate`
2. Check file permissions: `ls -la config/platform_config.json`
3. Verify environment variables: `env | grep SERVICE`
4. Reload configuration: `python scripts/config_manager.py reload`

### Health Check Failures

**Problem**: Services marked as unhealthy

**Solutions**:
1. Check service health endpoint: `curl http://<host>:<port>/health`
2. Verify health check interval in configuration
3. Check service logs for errors
4. Manually trigger health check: `python scripts/config_manager.py health <service>`

### Port Conflicts

**Problem**: Services cannot bind to ports

**Solutions**:
1. Check port usage: `netstat -tulpn | grep <port>`
2. Update configuration to use different ports
3. Stop conflicting services
4. Verify no other instances are running

### Frontend Connection Issues

**Problem**: Frontend cannot connect to backend

**Solutions**:
1. Check frontend environment variables: `cat frontend/.env.local`
2. Verify CORS settings in gateway configuration
3. Test API endpoint directly: `curl http://<gateway-url>/health`
4. Check browser console for CORS errors

## Performance Considerations

### Service Discovery Overhead

- Health checks run every 30 seconds by default
- Increase interval for large deployments: `health_check_interval: 60`
- Use caching for frequently accessed services

### Network Latency

- Minimize cross-machine calls when possible
- Place frequently communicating services on same machine
- Use load balancing for high-availability scenarios

### Configuration Caching

- Gateway caches service locations for performance
- Configuration reload clears cache
- Use `/admin/config/reload` after configuration changes

## Security Considerations

### Environment Variables

- Never commit sensitive values to version control
- Use `.env` files for local development
- Use secret management systems for production
- Rotate credentials regularly

### Service Discovery

- Service discovery should run on internal network only
- Use authentication for admin endpoints
- Limit registration to authorized services
- Monitor for unauthorized service registrations

### Network Security

- Use firewall rules to restrict access
- Implement TLS/SSL for production deployments
- Use VPN for inter-service communication
- Regular security audits of network configuration

## Monitoring and Logging

### Log Files

- Gateway: `logs/gateway.log`
- Service Discovery: `logs/service_discovery.log`
- Inventory Service: `/var/log/inventory_service.log`

### Health Monitoring

```bash
# Check all service health
curl http://localhost:8000/health

# Monitor service discovery
python scripts/config_manager.py list-discovered

# Check specific service
python scripts/config_manager.py health <service-name>
```

### Metrics to Monitor

- Service registration/deregistration events
- Health check success/failure rates
- Service discovery response times
- Configuration reload events
- Connection error rates

## Backup and Recovery

### Configuration Backup

```bash
# Backup configuration
cp config/platform_config.json config/platform_config.json.backup

# Backup service registry
cp data/service_registry.json data/service_registry.json.backup
```

### Service Registry Recovery

If service registry becomes corrupted:

1. Stop all services
2. Restore from backup
3. Restart services
4. Services will re-register automatically

### Configuration Recovery

If configuration becomes invalid:

1. Restore from backup
2. Validate: `python scripts/config_manager.py validate`
3. Reload: `python scripts/config_manager.py reload`

## Best Practices

1. **Always validate configuration** before deployment
2. **Use environment variables** for sensitive data
3. **Monitor service discovery** for registration issues
4. **Test health checks** regularly
5. **Document network topology** for distributed deployments
6. **Use version control** for configuration files
7. **Implement monitoring** for all services
8. **Plan for failure** with backup configurations
9. **Security audit** regularly
10. **Test failover** scenarios

## Additional Resources

- [Configuration Schema](../config/config_schema.json)
- [Example Configurations](../config/)
- [Service Discovery Documentation](../src/service_discovery.py)
- [Configuration Manager Tool](../scripts/config_manager.py)