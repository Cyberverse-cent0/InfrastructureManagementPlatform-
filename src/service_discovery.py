"""
Service Discovery System for Infrastructure Management Platform
Handles dynamic service registration, discovery, and health checking
for distributed deployment across multiple machines
"""

import asyncio
import json
import logging
import os
import socket
import threading
import time
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Any
from pathlib import Path
import httpx
from dataclasses import dataclass, asdict
from enum import Enum

# Import configuration loader
try:
    from config_loader import get_config, ServiceConfig
except ImportError:
    logger = logging.getLogger(__name__)
    logger.warning("config_loader not available, service discovery will use defaults")
    get_config = None
    ServiceConfig = None

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('logs/service_discovery.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

class ServiceStatus(Enum):
    """Service health status"""
    HEALTHY = "healthy"
    UNHEALTHY = "unhealthy"
    UNKNOWN = "unknown"
    MAINTENANCE = "maintenance"

@dataclass
class ServiceInstance:
    """Represents a service instance"""
    service_id: str
    service_name: str
    host: str
    port: int
    protocol: str = "http"
    health_check_endpoint: str = "/health"
    last_heartbeat: Optional[datetime] = None
    status: ServiceStatus = ServiceStatus.UNKNOWN
    metadata: Dict[str, Any] = None
    version: str = "1.0.0"
    environment: str = "production"

    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}
        if self.last_heartbeat is None:
            self.last_heartbeat = datetime.now()

    def get_url(self) -> str:
        """Get the full service URL"""
        return f"{self.protocol}://{self.host}:{self.port}"

    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        data = asdict(self)
        data['last_heartbeat'] = self.last_heartbeat.isoformat() if self.last_heartbeat else None
        data['status'] = self.status.value
        return data

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'ServiceInstance':
        """Create from dictionary"""
        if 'last_heartbeat' in data and data['last_heartbeat']:
            data['last_heartbeat'] = datetime.fromisoformat(data['last_heartbeat'])
        if 'status' in data and isinstance(data['status'], str):
            data['status'] = ServiceStatus(data['status'])
        return cls(**data)

class ServiceRegistry:
    """Central service registry for service discovery"""
    
    def __init__(self, registry_file: str = "data/service_registry.json", auto_load_config: bool = True):
        self.registry_file = registry_file
        self.services: Dict[str, ServiceInstance] = {}
        self.services_lock = threading.Lock()
        self.http_client = httpx.AsyncClient(timeout=10.0)
        self.health_check_interval = 30  # seconds
        self.service_timeout = 120  # seconds before considering service dead
        self.running = False
        self.health_check_thread = None
        self.auto_load_config = auto_load_config
        
        # Ensure data directory exists
        Path("data").mkdir(exist_ok=True)
        
        # Load existing registry
        self.load_registry()
        
        # Auto-load services from platform configuration
        if auto_load_config and get_config:
            self.load_from_platform_config()
    
    def load_registry(self):
        """Load service registry from file"""
        try:
            if os.path.exists(self.registry_file):
                with open(self.registry_file, 'r') as f:
                    data = json.load(f)
                
                with self.services_lock:
                    for service_id, service_data in data.items():
                        self.services[service_id] = ServiceInstance.from_dict(service_data)
                
                logger.info(f"Loaded {len(self.services)} services from registry")
        except Exception as e:
            logger.error(f"Error loading registry: {e}")
    
    def load_from_platform_config(self):
        """Load services from platform configuration file"""
        try:
            if not get_config:
                logger.warning("config_loader not available, skipping platform config load")
                return
            
            config = get_config()
            
            for service_key, service_config in config.services.items():
                service_id = f"{service_config.name}_{service_config.host}_{service_config.port}"
                
                # Create service instance from config
                service_instance = ServiceInstance(
                    service_id=service_id,
                    service_name=service_config.name,
                    host=service_config.host,
                    port=service_config.port,
                    protocol=service_config.protocol,
                    health_check_endpoint=service_config.health_check_endpoint,
                    metadata=service_config.metadata,
                    environment=config.environment.value,
                    version=config.version
                )
                
                # Register service if not already registered
                if service_id not in self.services:
                    self.register_service(service_instance)
                    logger.info(f"Auto-registered service from config: {service_config.name} at {service_config.get_url()}")
                else:
                    # Update existing service
                    self.services[service_id] = service_instance
                    logger.info(f"Updated service from config: {service_config.name}")
            
        except Exception as e:
            logger.error(f"Error loading services from platform config: {e}")
    
    def save_registry(self):
        """Save service registry to file"""
        try:
            with self.services_lock:
                data = {
                    service_id: service.to_dict()
                    for service_id, service in self.services.items()
                }
            
            with open(self.registry_file, 'w') as f:
                json.dump(data, f, indent=2)
            
            logger.debug("Saved service registry to file")
        except Exception as e:
            logger.error(f"Error saving registry: {e}")
    
    def register_service(self, service: ServiceInstance) -> bool:
        """Register a new service"""
        try:
            with self.services_lock:
                self.services[service.service_id] = service
            
            self.save_registry()
            logger.info(f"Registered service: {service.service_name} at {service.get_url()}")
            return True
        except Exception as e:
            logger.error(f"Error registering service: {e}")
            return False
    
    def unregister_service(self, service_id: str) -> bool:
        """Unregister a service"""
        try:
            with self.services_lock:
                if service_id in self.services:
                    service_name = self.services[service_id].service_name
                    del self.services[service_id]
                    self.save_registry()
                    logger.info(f"Unregistered service: {service_name} ({service_id})")
                    return True
            return False
        except Exception as e:
            logger.error(f"Error unregistering service: {e}")
            return False
    
    def get_service(self, service_name: str) -> Optional[ServiceInstance]:
        """Get a service by name (returns first match)"""
        with self.services_lock:
            for service in self.services.values():
                if service.service_name == service_name and service.status == ServiceStatus.HEALTHY:
                    return service
        return None
    
    def get_all_services(self, service_name: str = None) -> List[ServiceInstance]:
        """Get all services, optionally filtered by name"""
        with self.services_lock:
            services = list(self.services.values())
            if service_name:
                services = [s for s in services if s.service_name == service_name]
            return services
    
    def get_service_url(self, service_name: str) -> Optional[str]:
        """Get service URL by name"""
        service = self.get_service(service_name)
        return service.get_url() if service else None
    
    def update_service_status(self, service_id: str, status: ServiceStatus):
        """Update service status"""
        with self.services_lock:
            if service_id in self.services:
                self.services[service_id].status = status
                self.services[service_id].last_heartbeat = datetime.now()
                self.save_registry()
    
    async def check_service_health(self, service: ServiceInstance) -> ServiceStatus:
        """Check health of a single service"""
        try:
            url = f"{service.get_url()}{service.health_check_endpoint}"
            response = await self.http_client.get(url, timeout=5.0)
            
            if response.status_code == 200:
                return ServiceStatus.HEALTHY
            else:
                logger.warning(f"Service {service.service_name} returned status {response.status_code}")
                return ServiceStatus.UNHEALTHY
        except httpx.TimeoutException:
            logger.warning(f"Service {service.service_name} health check timed out")
            return ServiceStatus.UNKNOWN
        except Exception as e:
            logger.error(f"Health check failed for {service.service_name}: {e}")
            return ServiceStatus.UNHEALTHY
    
    async def health_check_worker(self):
        """Background worker for health checking"""
        logger.info("Starting health check worker")
        
        while self.running:
            try:
                with self.services_lock:
                    services_to_check = list(self.services.values())
                
                for service in services_to_check:
                    status = await self.check_service_health(service)
                    self.update_service_status(service.service_id, status)
                
                # Remove dead services
                await self.cleanup_dead_services()
                
            except Exception as e:
                logger.error(f"Error in health check worker: {e}")
            
            await asyncio.sleep(self.health_check_interval)
    
    async def cleanup_dead_services(self):
        """Remove services that haven't sent heartbeat recently"""
        now = datetime.now()
        dead_services = []
        
        with self.services_lock:
            for service_id, service in self.services.items():
                if service.last_heartbeat:
                    time_since_heartbeat = (now - service.last_heartbeat).total_seconds()
                    if time_since_heartbeat > self.service_timeout:
                        dead_services.append(service_id)
                        logger.warning(f"Service {service.service_name} appears dead, removing")
            
            for service_id in dead_services:
                del self.services[service_id]
            
            if dead_services:
                self.save_registry()
    
    def start_health_checks(self):
        """Start the health check background worker"""
        if not self.running:
            self.running = True
            self.health_check_thread = threading.Thread(
                target=lambda: asyncio.run(self.health_check_worker()),
                daemon=True
            )
            self.health_check_thread.start()
            logger.info("Health check worker started")
    
    def stop_health_checks(self):
        """Stop the health check background worker"""
        self.running = False
        if self.health_check_thread:
            self.health_check_thread.join(timeout=5)
        logger.info("Health check worker stopped")
    
    def get_registry_stats(self) -> Dict[str, Any]:
        """Get registry statistics"""
        with self.services_lock:
            total = len(self.services)
            healthy = sum(1 for s in self.services.values() if s.status == ServiceStatus.HEALTHY)
            unhealthy = sum(1 for s in self.services.values() if s.status == ServiceStatus.UNHEALTHY)
            unknown = sum(1 for s in self.services.values() if s.status == ServiceStatus.UNKNOWN)
            
            # Group by service name
            by_name = {}
            for service in self.services.values():
                name = service.service_name
                if name not in by_name:
                    by_name[name] = []
                by_name[name].append(service.status.value)
            
            return {
                "total": total,
                "healthy": healthy,
                "unhealthy": unhealthy,
                "unknown": unknown,
                "by_name": by_name
            }
    
    def reload_configuration(self):
        """Reload services from platform configuration"""
        logger.info("Reloading service configuration...")
        if self.auto_load_config and get_config:
            try:
                # Import reload_config function
                from config_loader import reload_config
                reload_config()
                self.load_from_platform_config()
                logger.info("Service configuration reloaded successfully")
            except ImportError:
                logger.warning("config_loader reload not available")
            except Exception as e:
                logger.error(f"Error reloading configuration: {e}")
        else:
            logger.warning("Auto config load is disabled, reload not available")
            
            return {
                "total_services": total,
                "healthy_services": healthy,
                "unhealthy_services": unhealthy,
                "unknown_services": unknown,
                "services_by_name": {
                    name: {
                        "total": len(statuses),
                        "healthy": statuses.count("healthy"),
                        "unhealthy": statuses.count("unhealthy"),
                        "unknown": statuses.count("unknown")
                    }
                    for name, statuses in by_name.items()
                }
            }

class ServiceConfig:
    """Configuration for service discovery"""
    
    def __init__(self, config_file: str = "config/services.json"):
        self.config_file = config_file
        self.config = {}
        self.load_config()
    
    def load_config(self):
        """Load service configuration"""
        try:
            if os.path.exists(self.config_file):
                with open(self.config_file, 'r') as f:
                    self.config = json.load(f)
                logger.info(f"Loaded service configuration from {self.config_file}")
            else:
                # Create default configuration
                self.config = self.get_default_config()
                self.save_config()
                logger.info("Created default service configuration")
        except Exception as e:
            logger.error(f"Error loading config: {e}")
            self.config = self.get_default_config()
    
    def save_config(self):
        """Save service configuration"""
        try:
            os.makedirs(os.path.dirname(self.config_file), exist_ok=True)
            with open(self.config_file, 'w') as f:
                json.dump(self.config, f, indent=2)
        except Exception as e:
            logger.error(f"Error saving config: {e}")
    
    def get_default_config(self) -> Dict[str, Any]:
        """Get default service configuration"""
        return {
            "services": {
                "gateway": {
                    "default_host": "localhost",
                    "default_port": 8000,
                    "protocol": "http",
                    "health_check_endpoint": "/health"
                },
                "core_backend": {
                    "default_host": "localhost",
                    "default_port": 8080,
                    "protocol": "http",
                    "health_check_endpoint": "/health"
                },
                "inventory_service": {
                    "default_host": "localhost",
                    "default_port": 8081,
                    "protocol": "http",
                    "health_check_endpoint": "/health"
                },
                "frontend": {
                    "default_host": "localhost",
                    "default_port": 3000,
                    "protocol": "http",
                    "health_check_endpoint": "/api/health"
                }
            },
            "discovery": {
                "enabled": True,
                "health_check_interval": 30,
                "service_timeout": 120,
                "auto_registration": True
            },
            "network": {
                "auto_detect_ip": True,
                "preferred_interface": "eth0"
            }
        }
    
    def get_service_config(self, service_name: str) -> Optional[Dict[str, Any]]:
        """Get configuration for a specific service"""
        return self.config.get("services", {}).get(service_name)
    
    def update_service_config(self, service_name: str, config: Dict[str, Any]):
        """Update configuration for a specific service"""
        if "services" not in self.config:
            self.config["services"] = {}
        self.config["services"][service_name] = config
        self.save_config()

def get_local_ip() -> str:
    """Get local IP address"""
    try:
        # Connect to a remote server to determine local IP
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.connect(("8.8.8.8", 80))
            local_ip = s.getsockname()[0]
        return local_ip
    except Exception:
        return "127.0.0.1"

def get_hostname() -> str:
    """Get system hostname"""
    return socket.gethostname()

# Global service registry instance
service_registry = ServiceRegistry()
service_config = ServiceConfig()

def initialize_service_discovery():
    """Initialize service discovery system"""
    logger.info("Initializing service discovery system")
    
    # Start health checks if enabled
    if service_config.config.get("discovery", {}).get("enabled", True):
        service_registry.start_health_checks()
    
    logger.info("Service discovery system initialized")

def shutdown_service_discovery():
    """Shutdown service discovery system"""
    logger.info("Shutting down service discovery system")
    service_registry.stop_health_checks()
    asyncio.run(service_registry.http_client.aclose())

# Auto-register current service
def auto_register_service(service_name: str, port: int, **kwargs):
    """Automatically register the current service"""
    if service_config.config.get("discovery", {}).get("auto_registration", True):
        # Get local configuration
        local_ip = get_local_ip()
        hostname = get_hostname()
        
        # Get service-specific config
        service_cfg = service_config.get_service_config(service_name) or {}
        
        # Create service instance
        service = ServiceInstance(
            service_id=f"{service_name}_{hostname}_{port}",
            service_name=service_name,
            host=service_cfg.get("default_host", local_ip),
            port=port,
            protocol=service_cfg.get("protocol", "http"),
            health_check_endpoint=service_cfg.get("health_check_endpoint", "/health"),
            metadata={
                "hostname": hostname,
                "registered_at": datetime.now().isoformat(),
                **kwargs
            }
        )
        
        service_registry.register_service(service)
        logger.info(f"Auto-registered service: {service_name} at {service.get_url()}")
        return service
    return None

if __name__ == "__main__":
    # Test service discovery
    initialize_service_discovery()
    
    # Register a test service
    test_service = ServiceInstance(
        service_id="test_service_1",
        service_name="test_service",
        host="localhost",
        port=8080,
        protocol="http"
    )
    
    service_registry.register_service(test_service)
    
    # Get service
    found_service = service_registry.get_service("test_service")
    print(f"Found service: {found_service.get_url() if found_service else 'None'}")
    
    # Get stats
    stats = service_registry.get_registry_stats()
    print(f"Registry stats: {json.dumps(stats, indent=2)}")
    
    shutdown_service_discovery()