# API Gateway Server

FastAPI-based gateway server that acts as a connection layer between the Next.js frontend and C-based backend services.

## Features

- **Service Gateway**: Proxies requests between frontend and backend services
- **Session Management**: User authentication and session handling
- **CORS Support**: Cross-origin resource sharing for frontend integration
- **Request Logging**: Comprehensive request/response logging
- **Health Monitoring**: Backend service health checks
- **Error Handling**: Centralized error handling and responses
- **Security**: Session-based authentication and authorization

## Architecture

```
Next.js Frontend (Port 3000)
        ↓
API Gateway (Port 8000) ←→ Python Session Manager
        ↓
C Backend Services (Port 8080+)
- HTTP Server
- Inventory Service  
- PostgreSQL Interface
```

## Installation

1. **Install Python dependencies**:
```bash
pip install -r requirements.txt
```

2. **Configure environment variables**:
```bash
cp .env.example .env
# Edit .env with your configuration
```

3. **Create necessary directories**:
```bash
mkdir -p logs data
```

## Usage

### Quick Start
```bash
./start_gateway.sh
```

### Manual Start
```bash
# Activate virtual environment
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Start server
python src/main.py
```

### Development Mode
```bash
# Set DEBUG=true in .env file
DEBUG=true python src/main.py
```

## API Endpoints

### Health & Info
- `GET /` - Gateway information
- `GET /health` - Health check with service status
- `GET /api/v1` - API version information

### Authentication
- `POST /session/login` - User login
- `POST /session/logout` - User logout  
- `GET /session/me` - Get current session

### Proxy Endpoint
- `POST /proxy` - Generic proxy to backend services

### Inventory API
- `GET /api/v1/inventory` - Get inventory items
- `POST /api/v1/inventory` - Create inventory item

### Network API
- `GET /api/v1/network/interfaces` - Get network interfaces

### System API
- `GET /api/v1/system/status` - Get system status

## Configuration

Environment variables in `.env`:

```bash
# Server
GATEWAY_HOST=0.0.0.0
GATEWAY_PORT=8000
DEBUG=true

# Backend Services
C_BACKEND_URL=http://localhost:8080
INVENTORY_SERVICE_URL=http://localhost:8081
POSTGRES_SERVICE_URL=http://localhost:5432

# Security
SECRET_KEY=your-secret-key
SESSION_EXPIRY_HOURS=24

# CORS
ALLOWED_ORIGINS=["http://localhost:3000"]
```

## Session Management

The gateway includes a built-in session manager with:

- In-memory session storage (can be upgraded to Redis)
- Automatic session cleanup
- User authentication
- Role-based access control
- Session expiration handling

### Default Users

On first run, a default admin user is created:
- Username: `admin`
- Password: `admin123`
- **Important**: Change this password in production!

## Backend Integration

The gateway connects to several backend services:

1. **C HTTP Server**: Main backend service for HTTP handling
2. **Inventory Service**: Microservice for inventory management
3. **PostgreSQL**: Database operations

Each service is health-checked on startup and monitored during operation.

## Frontend Integration

From your Next.js frontend, make requests to the gateway:

```javascript
// Login
const response = await fetch('http://localhost:8000/session/login', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ username: 'admin', password: 'admin123' })
});

// Get inventory
const token = response.data.token;
const inventory = await fetch('http://localhost:8000/api/v1/inventory', {
  headers: { 'Authorization': `Bearer ${token}` }
});
```

## Logging

Logs are stored in `logs/gateway.log` with:
- Request timestamps
- Response times
- Error details
- Backend service status

## Development

### Adding New Endpoints

1. Define the route in `src/main.py`
2. Add authentication if needed: `session: SessionData = Depends(get_session)`
3. Proxy to backend service or implement logic
4. Add error handling

### Testing

```bash
# Test health endpoint
curl http://localhost:8000/health

# Test login
curl -X POST http://localhost:8000/session/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}'
```

## Production Deployment

For production deployment:

1. **Use Redis** for session storage instead of in-memory
2. **Set strong SECRET_KEY** using `openssl rand -hex 32`
3. **Enable HTTPS** with proper SSL certificates
4. **Set DEBUG=false** in environment
5. **Use production WSGI server** like Gunicorn with Uvicorn workers
6. **Implement rate limiting** for API protection
7. **Set up monitoring** and alerting

## Troubleshooting

### Gateway won't start
- Check if port 8000 is available
- Verify Python dependencies are installed
- Check logs in `logs/gateway.log`

### Backend connection errors
- Verify backend services are running
- Check service URLs in `.env`
- Ensure CORS settings are correct

### Authentication failures
- Verify default user credentials
- Check session timeout settings
- Review session manager logs

## Future Enhancements

- [ ] Redis integration for session storage
- [ ] JWT token authentication
- [ ] OAuth2 integration
- [ ] Rate limiting per user
- [ ] API request caching
- [ ] WebSocket support for real-time updates
- [ ] GraphQL endpoint
- [ ] API versioning strategy
- [ ] Advanced monitoring and metrics