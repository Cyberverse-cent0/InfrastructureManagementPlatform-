"""
Configuration Loader for Infrastructure Management Platform
Handles loading and validation of platform configuration from files and environment variables
"""

import json
import os
import logging
from typing import Dict, Any, Optional
from pathlib import Path
from dataclasses import dataclass, field
from enum import Enum

logger = logging.getLogger(__name__)

class DeploymentMode(Enum):
    """Deployment mode options"""
    SINGLE_MACHINE = "single_machine"
    DISTRIBUTED = "distributed"
    FLEXIBLE = "flexible"

class Environment(Enum):
    """Environment options"""
    DEVELOPMENT = "development"
    STAGING = "staging"
    PRODUCTION = "production"

@dataclass
class ServiceConfig:
    """Configuration for a single service"""
    name: str
    host: str
    port: int
    protocol: str = "http"
    health_check_endpoint: Optional[str] = "/health"
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def get_url(self) -> str:
        """Get the full service URL"""
        return f"{self.protocol}://{self.host}:{self.port}"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            "name": self.name,
            "host": self.host,
            "port": self.port,
            "protocol": self.protocol,
            "health_check_endpoint": self.health_check_endpoint,
            "metadata": self.metadata
        }

@dataclass
class PlatformConfig:
    """Main platform configuration"""
    version: str
    environment: Environment
    deployment_mode: DeploymentMode
    services: Dict[str, ServiceConfig] = field(default_factory=dict)
    service_discovery: Dict[str, Any] = field(default_factory=dict)
    network: Dict[str, Any] = field(default_factory=dict)
    
    def get_service(self, service_name: str) -> Optional[ServiceConfig]:
        """Get service configuration by name"""
        return self.services.get(service_name)
    
    def get_service_url(self, service_name: str) -> Optional[str]:
        """Get service URL by name"""
        service = self.get_service(service_name)
        return service.get_url() if service else None

class ConfigLoader:
    """Configuration loader with environment variable support"""
    
    def __init__(self, config_file: str = "config/platform_config.json"):
        self.config_file = config_file
        self.config: Optional[PlatformConfig] = None
        self.schema_file = "config/config_schema.json"
    
    def load(self) -> PlatformConfig:
        """Load configuration from file and environment variables"""
        # Load base configuration from file
        file_config = self._load_file()
        
        # Apply environment variable overrides
        self._apply_env_overrides(file_config)
        
        # Create configuration object
        self.config = self._create_config_object(file_config)
        
        logger.info(f"Loaded configuration for {self.config.environment.value} environment")
        logger.info(f"Deployment mode: {self.config.deployment_mode.value}")
        
        return self.config
    
    def _load_file(self) -> Dict[str, Any]:
        """Load configuration from JSON file"""
        try:
            config_path = Path(self.config_file)
            if not config_path.exists():
                logger.warning(f"Configuration file not found: {self.config_file}, using defaults")
                return self._get_default_config()
            
            with open(config_path, 'r') as f:
                config = json.load(f)
            
            # Validate configuration if schema exists
            if Path(self.schema_file).exists():
                self._validate_config(config)
            
            return config
        except Exception as e:
            logger.error(f"Error loading configuration file: {e}")
            return self._get_default_config()
    
    def _apply_env_overrides(self, config: Dict[str, Any]):
        """Apply environment variable overrides to configuration"""
        # Override deployment mode
        deployment_mode = os.getenv("DEPLOYMENT_MODE")
        if deployment_mode:
            config["deployment_mode"] = deployment_mode
        
        # Override service hosts for distributed deployment
        service_overrides = {
            "CORE_BACKEND_HOST": "core_backend",
            "INVENTORY_SERVICE_HOST": "inventory_service",
            "POSTGRES_HOST": "postgres",
            "FRONTEND_HOST": "frontend",
            "GATEWAY_HOST": "gateway"
        }
        
        for env_var, service_key in service_overrides.items():
            host_value = os.getenv(env_var)
            if host_value and service_key in config.get("services", {}):
                config["services"][service_key]["host"] = host_value
                logger.info(f"Override {service_key} host with environment variable: {host_value}")
        
        # Override service discovery settings
        if os.getenv("SERVICE_DISCOVERY_ENABLED"):
            config["service_discovery"]["enabled"] = os.getenv("SERVICE_DISCOVERY_ENABLED").lower() == "true"
        
        # Override network settings
        if os.getenv("CONNECTION_TIMEOUT"):
            config["network"]["connection_timeout"] = int(os.getenv("CONNECTION_TIMEOUT"))
        if os.getenv("RETRY_ATTEMPTS"):
            config["network"]["retry_attempts"] = int(os.getenv("RETRY_ATTEMPTS"))
        if os.getenv("RETRY_DELAY"):
            config["network"]["retry_delay"] = int(os.getenv("RETRY_DELAY"))
    
    def _create_config_object(self, config: Dict[str, Any]) -> PlatformConfig:
        """Create PlatformConfig object from dictionary"""
        try:
            # Parse environment
            environment = Environment(config.get("environment", "development"))
            
            # Parse deployment mode
            deployment_mode = DeploymentMode(config.get("deployment_mode", "flexible"))
            
            # Parse services
            services = {}
            for service_key, service_data in config.get("services", {}).items():
                services[service_key] = ServiceConfig(
                    name=service_data.get("name", service_key),
                    host=service_data.get("host", "localhost"),
                    port=service_data.get("port", 8000),
                    protocol=service_data.get("protocol", "http"),
                    health_check_endpoint=service_data.get("health_check_endpoint", "/health"),
                    metadata=service_data.get("metadata", {})
                )
            
            return PlatformConfig(
                version=config.get("version", "1.0.0"),
                environment=environment,
                deployment_mode=deployment_mode,
                services=services,
                service_discovery=config.get("service_discovery", {}),
                network=config.get("network", {})
            )
        except Exception as e:
            logger.error(f"Error creating configuration object: {e}")
            raise
    
    def _validate_config(self, config: Dict[str, Any]):
        """Validate configuration against schema"""
        try:
            import jsonschema
            with open(self.schema_file, 'r') as f:
                schema = json.load(f)
            jsonschema.validate(config, schema)
            logger.info("Configuration validation passed")
        except ImportError:
            logger.warning("jsonschema not installed, skipping validation")
        except Exception as e:
            logger.warning(f"Configuration validation failed: {e}")
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default configuration"""
        return {
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
            },
            "service_discovery": {
                "enabled": True,
                "registry_file": "data/service_registry.json",
                "health_check_interval": 30,
                "service_timeout": 120
            },
            "network": {
                "connection_timeout": 10,
                "retry_attempts": 3,
                "retry_delay": 1
            }
        }
    
    def reload(self) -> PlatformConfig:
        """Reload configuration"""
        logger.info("Reloading configuration...")
        return self.load()

# Global configuration instance
_config_loader: Optional[ConfigLoader] = None
_global_config: Optional[PlatformConfig] = None

def get_config(config_file: str = None) -> PlatformConfig:
    """Get global configuration instance"""
    global _config_loader, _global_config
    
    if _global_config is None:
        config_file = config_file or os.getenv("CONFIG_FILE", "config/platform_config.json")
        _config_loader = ConfigLoader(config_file)
        _global_config = _config_loader.load()
    
    return _global_config

def reload_config():
    """Reload global configuration"""
    global _global_config
    if _config_loader:
        _global_config = _config_loader.reload()
    return _global_config