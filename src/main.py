"""
Infrastructure Management Platform - API Gateway Server
Acts as connection gateway between Next.js frontend and C-based backend services
"""

from fastapi import FastAPI, HTTPException, Request, Response, Depends, status
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.gzip import GZipMiddleware
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field
from typing import Optional, Dict, Any, List
import httpx
import asyncio
import logging
import json
import os
import sys
from pathlib import Path
from datetime import datetime, timedelta
import uvicorn

# Add parent directory to path for imports
sys.path.append(str(Path(__file__).parent.parent))

# Configuration
class GatewayConfig:
    """Gateway configuration settings"""
    
    # Server settings
    HOST: str = os.getenv("GATEWAY_HOST", "0.0.0.0")
    PORT: int = int(os.getenv("GATEWAY_PORT", "8000"))
    DEBUG: bool = os.getenv("DEBUG", "false").lower() == "true"
    
    # Backend service URLs
    C_BACKEND_URL: str = os.getenv("C_BACKEND_URL", "http://localhost:8080")
    INVENTORY_SERVICE_URL: str = os.getenv("INVENTORY_SERVICE_URL", "http://localhost:8081")
    POSTGRES_SERVICE_URL: str = os.getenv("POSTGRES_SERVICE_URL", "http://localhost:5432")
    
    # Security settings
    SECRET_KEY: str = os.getenv("SECRET_KEY", "dev-secret-key-change-in-production")
    SESSION_EXPIRY_HOURS: int = int(os.getenv("SESSION_EXPIRY_HOURS", "24"))
    
    # CORS settings
    ALLOWED_ORIGINS: List[str] = json.loads(os.getenv("ALLOWED_ORIGINS", '["http://localhost:3000", "http://localhost:3001"]'))
    ALLOWED_METHODS: List[str] = ["GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH"]
    ALLOWED_HEADERS: List[str] = ["*"]
    
    # Rate limiting
    RATE_LIMIT_REQUESTS: int = int(os.getenv("RATE_LIMIT_REQUESTS", "100"))
    RATE_LIMIT_PERIOD: int = int(os.getenv("RATE_LIMIT_PERIOD", "60"))  # seconds

config = GatewayConfig()

# Setup logging
logging.basicConfig(
    level=logging.DEBUG if config.DEBUG else logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('logs/gateway.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Initialize FastAPI app
app = FastAPI(
    title="Infrastructure Management Platform API Gateway",
    description="Gateway server connecting Next.js frontend to C-based backend services",
    version="1.0.0",
    debug=config.DEBUG
)

# Middleware configuration
app.add_middleware(
    CORSMiddleware,
    allow_origins=config.ALLOWED_ORIGINS,
    allow_credentials=True,
    allow_methods=config.ALLOWED_METHODS,
    allow_headers=config.ALLOWED_HEADERS,
)

app.add_middleware(GZipMiddleware, minimum_size=1000)

# Request/Response models
class HealthResponse(BaseModel):
    """Health check response"""
    status: str
    timestamp: datetime
    services: Dict[str, str]

class ProxyRequest(BaseModel):
    """Generic proxy request model"""
    service: str
    endpoint: str
    method: str = "GET"
    headers: Optional[Dict[str, str]] = None
    params: Optional[Dict[str, Any]] = None
    body: Optional[Dict[str, Any]] = None

class ErrorResponse(BaseModel):
    """Error response model"""
    error: str
    detail: str
    timestamp: datetime
    path: str

class SessionData(BaseModel):
    """Session data model"""
    user_id: str
    username: str
    role: str
    created_at: datetime
    expires_at: datetime

# In-memory session storage (replace with Redis in production)
sessions: Dict[str, SessionData] = {}
session_lock = asyncio.Lock()

# HTTP client for backend communication
http_client = httpx.AsyncClient(timeout=30.0)

# Service registry
SERVICE_URLS = {
    "c_backend": config.C_BACKEND_URL,
    "inventory": config.INVENTORY_SERVICE_URL,
    "postgres": config.POSTGRES_SERVICE_URL
}

# Startup and shutdown events
@app.on_event("startup")
async def startup_event():
    """Initialize gateway on startup"""
    logger.info("Starting API Gateway...")
    logger.info(f"Debug mode: {config.DEBUG}")
    logger.info(f"C Backend URL: {config.C_BACKEND_URL}")
    logger.info(f"Inventory Service URL: {config.INVENTORY_SERVICE_URL}")
    
    # Create logs directory if it doesn't exist
    os.makedirs("logs", exist_ok=True)
    
    # Test backend connections
    await test_backend_connections()

@app.on_event("shutdown")
async def shutdown_event():
    """Cleanup on shutdown"""
    logger.info("Shutting down API Gateway...")
    await http_client.aclose()

async def test_backend_connections():
    """Test connectivity to backend services"""
    logger.info("Testing backend connections...")
    
    for service_name, service_url in SERVICE_URLS.items():
        try:
            # Try basic connection test
            if service_name == "postgres":
                # Skip direct HTTP test for PostgreSQL
                logger.info(f"PostgreSQL service: {service_url} (skipping HTTP test)")
            else:
                response = await http_client.get(f"{service_url}/health", timeout=5.0)
                if response.status_code == 200:
                    logger.info(f"✓ {service_name} at {service_url} - Healthy")
                else:
                    logger.warning(f"⚠ {service_name} at {service_url} - Status: {response.status_code}")
        except Exception as e:
            logger.warning(f"✗ {service_name} at {service_url} - Connection failed: {str(e)}")

# Authentication middleware
async def get_session(request: Request) -> Optional[SessionData]:
    """Extract and validate session from request"""
    session_token = request.headers.get("Authorization")
    if not session_token:
        return None
    
    # Remove "Bearer " prefix if present
    if session_token.startswith("Bearer "):
        session_token = session_token[7:]
    
    async with session_lock:
        session = sessions.get(session_token)
        if session:
            # Check if session is expired
            if datetime.now() > session.expires_at:
                del sessions[session_token]
                return None
            return session
    
    return None

# Middleware for request logging
@app.middleware("http")
async def log_requests(request: Request, call_next):
    """Log all incoming requests"""
    start_time = datetime.now()
    
    logger.info(f"Incoming request: {request.method} {request.url.path}")
    
    try:
        response = await call_next(request)
        
        duration = (datetime.now() - start_time).total_seconds()
        logger.info(f"Request completed: {request.method} {request.url.path} - Status: {response.status_code} - Duration: {duration:.3f}s")
        
        return response
    except Exception as e:
        duration = (datetime.now() - start_time).total_seconds()
        logger.error(f"Request failed: {request.method} {request.url.path} - Error: {str(e)} - Duration: {duration:.3f}s")
        raise

# Health check endpoint
@app.get("/health", response_model=HealthResponse)
async def health_check():
    """Gateway health check"""
    service_status = {}
    
    for service_name, service_url in SERVICE_URLS.items():
        try:
            if service_name == "postgres":
                service_status[service_name] = "configured"
            else:
                response = await http_client.get(f"{service_url}/health", timeout=2.0)
                service_status[service_name] = "healthy" if response.status_code == 200 else "unhealthy"
        except:
            service_status[service_name] = "unreachable"
    
    return HealthResponse(
        status="healthy",
        timestamp=datetime.now(),
        services=service_status
    )

# Root endpoint
@app.get("/")
async def root():
    """Root endpoint with API information"""
    return {
        "name": "Infrastructure Management Platform API Gateway",
        "version": "1.0.0",
        "status": "running",
        "timestamp": datetime.now().isoformat(),
        "endpoints": {
            "health": "/health",
            "api": "/api/v1",
            "proxy": "/proxy",
            "session": "/session"
        }
    }

# API Routes
@app.get("/api/v1")
async def api_info():
    """API information endpoint"""
    return {
        "version": "1.0.0",
        "services": list(SERVICE_URLS.keys()),
        "endpoints": {
            "inventory": "/api/v1/inventory",
            "users": "/api/v1/users",
            "network": "/api/v1/network",
            "system": "/api/v1/system"
        }
    }

# Proxy endpoint for generic backend communication
@app.post("/proxy")
async def proxy_request(
    proxy_req: ProxyRequest,
    session: Optional[SessionData] = Depends(get_session)
):
    """Proxy requests to backend services"""
    
    # Check authentication for non-public endpoints
    if proxy_req.service not in ["c_backend"] and not session:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Authentication required"
        )
    
    # Get service URL
    service_url = SERVICE_URLS.get(proxy_req.service)
    if not service_url:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"Service '{proxy_req.service}' not found"
        )
    
    # Construct full URL
    url = f"{service_url}/{proxy_req.endpoint.lstrip('/')}"
    
    try:
        # Make request to backend service
        headers = proxy_req.headers or {}
        headers.update({
            "X-Gateway-Session": session.user_id if session else "anonymous",
            "X-Forwarded-For": "gateway"
        })
        
        if proxy_req.method.upper() == "GET":
            response = await http_client.get(url, params=proxy_req.params, headers=headers)
        elif proxy_req.method.upper() == "POST":
            response = await http_client.post(url, json=proxy_req.body, params=proxy_req.params, headers=headers)
        elif proxy_req.method.upper() == "PUT":
            response = await http_client.put(url, json=proxy_req.body, params=proxy_req.params, headers=headers)
        elif proxy_req.method.upper() == "DELETE":
            response = await http_client.delete(url, params=proxy_req.params, headers=headers)
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Unsupported method: {proxy_req.method}"
            )
        
        # Return backend response
        return JSONResponse(
            content=response.json(),
            status_code=response.status_code
        )
        
    except httpx.HTTPError as e:
        logger.error(f"Backend service error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail=f"Backend service unavailable: {str(e)}"
        )

# Session management endpoints
@app.post("/session/login")
async def login(request: Request):
    """User login endpoint"""
    try:
        body = await request.json()
        username = body.get("username")
        password = body.get("password")
        
        if not username or not password:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Username and password required"
            )
        
        # TODO: Implement actual authentication with backend
        # For now, create a mock session
        session_token = f"session_{username}_{datetime.now().timestamp()}"
        
        session_data = SessionData(
            user_id=username,
            username=username,
            role="admin",  # TODO: Get from backend
            created_at=datetime.now(),
            expires_at=datetime.now() + timedelta(hours=config.SESSION_EXPIRY_HOURS)
        )
        
        async with session_lock:
            sessions[session_token] = session_data
        
        logger.info(f"User logged in: {username}")
        
        return {
            "token": session_token,
            "user": {
                "username": username,
                "role": session_data.role
            },
            "expires_at": session_data.expires_at.isoformat()
        }
        
    except Exception as e:
        logger.error(f"Login error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Login failed"
        )

@app.post("/session/logout")
async def logout(session: SessionData = Depends(get_session)):
    """User logout endpoint"""
    if session:
        async with session_lock:
            session_token = None
            # Find and remove session
            for token, data in sessions.items():
                if data.user_id == session.user_id:
                    session_token = token
                    break
            
            if session_token:
                del sessions[session_token]
        
        logger.info(f"User logged out: {session.username}")
    
    return {"message": "Logged out successfully"}

@app.get("/session/me")
async def get_current_session(session: SessionData = Depends(get_session)):
    """Get current session information"""
    if not session:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Not authenticated"
        )
    
    return {
        "user_id": session.user_id,
        "username": session.username,
        "role": session.role,
        "created_at": session.created_at.isoformat(),
        "expires_at": session.expires_at.isoformat()
    }

# Inventory API routes (proxy to C backend)
@app.get("/api/v1/inventory")
async def get_inventory(session: SessionData = Depends(get_session)):
    """Get inventory items"""
    try:
        response = await http_client.get(f"{config.C_BACKEND_URL}/inventory")
        return response.json()
    except Exception as e:
        logger.error(f"Inventory fetch error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail="Inventory service unavailable"
        )

@app.post("/api/v1/inventory")
async def create_inventory_item(item: Dict[str, Any], session: SessionData = Depends(get_session)):
    """Create new inventory item"""
    try:
        response = await http_client.post(
            f"{config.C_BACKEND_URL}/inventory",
            json=item,
            headers={"X-User-ID": session.user_id}
        )
        return response.json()
    except Exception as e:
        logger.error(f"Inventory creation error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail="Inventory service unavailable"
        )

# Network interface routes
@app.get("/api/v1/network/interfaces")
async def get_network_interfaces(session: SessionData = Depends(get_session)):
    """Get network interfaces"""
    try:
        response = await http_client.get(f"{config.C_BACKEND_URL}/network/interfaces")
        return response.json()
    except Exception as e:
        logger.error(f"Network interfaces error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail="Network service unavailable"
        )

# System status routes
@app.get("/api/v1/system/status")
async def get_system_status(session: SessionData = Depends(get_session)):
    """Get system status"""
    try:
        response = await http_client.get(f"{config.C_BACKEND_URL}/system/status")
        return response.json()
    except Exception as e:
        logger.error(f"System status error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail="System service unavailable"
        )

# Error handlers
@app.exception_handler(HTTPException)
async def http_exception_handler(request: Request, exc: HTTPException):
    """Custom HTTP exception handler"""
    return JSONResponse(
        status_code=exc.status_code,
        content={
            "error": "HTTP Error",
            "detail": exc.detail,
            "timestamp": datetime.now().isoformat(),
            "path": request.url.path
        }
    )

@app.exception_handler(Exception)
async def general_exception_handler(request: Request, exc: Exception):
    """General exception handler"""
    logger.error(f"Unhandled exception: {str(exc)}")
    return JSONResponse(
        status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
        content={
            "error": "Internal Server Error",
            "detail": str(exc) if config.DEBUG else "An unexpected error occurred",
            "timestamp": datetime.now().isoformat(),
            "path": request.url.path
        }
    )

# Main entry point
if __name__ == "__main__":
    uvicorn.run(
        "main:app",
        host=config.HOST,
        port=config.PORT,
        reload=config.DEBUG,
        log_level="debug" if config.DEBUG else "info"
    )