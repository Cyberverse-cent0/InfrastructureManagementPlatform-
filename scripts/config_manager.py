#!/usr/bin/env python3
"""
Configuration Manager for Infrastructure Management Platform
CLI tool for managing service configuration and service discovery
"""

import json
import sys
import os
import argparse
from pathlib import Path
from typing import Dict, Any, Optional
import requests
import subprocess

# Add parent directory to path for imports
sys.path.append(str(Path(__file__).parent.parent))

try:
    from config_loader import get_config, reload_config, PlatformConfig
    from service_discovery import ServiceRegistry, ServiceInstance, ServiceStatus
except ImportError:
    print("Error: Required modules not available. Please run from the project root.")
    sys.exit(1)

class ConfigManager:
    """Configuration management CLI"""
    
    def __init__(self, config_file: str = "config/platform_config.json"):
        self.config_file = config_file
        self.config: Optional[PlatformConfig] = None
        self.registry: Optional[ServiceRegistry] = None
        self.load_configuration()
    
    def load_configuration(self):
        """Load platform configuration"""
        try:
            self.config = get_config(self.config_file)
            print(f"✓ Loaded configuration from {self.config_file}")
            print(f"  Environment: {self.config.environment.value}")
            print(f"  Deployment mode: {self.config.deployment_mode.value}")
        except Exception as e:
            print(f"✗ Failed to load configuration: {e}")
            sys.exit(1)
    
    def load_service_registry(self):
        """Load service registry"""
        try:
            registry_file = self.config.service_discovery.get('registry_file', 'data/service_registry.json')
            self.registry = ServiceRegistry(registry_file, auto_load_config=True)
            print(f"✓ Loaded service registry from {registry_file}")
        except Exception as e:
            print(f"✗ Failed to load service registry: {e}")
    
    def validate_config(self) -> bool:
        """Validate current configuration"""
        print("\n=== Configuration Validation ===")
        
        try:
            # Check if config file exists
            if not Path(self.config_file).exists():
                print(f"✗ Configuration file not found: {self.config_file}")
                return False
            
            # Validate structure
            with open(self.config_file, 'r') as f:
                data = json.load(f)
            
            required_keys = ['version', 'environment', 'services']
            for key in required_keys:
                if key not in data:
                    print(f"✗ Missing required key: {key}")
                    return False
            
            # Validate services
            for service_key, service_data in data['services'].items():
                required_service_keys = ['name', 'host', 'port', 'protocol']
                for key in required_service_keys:
                    if key not in service_data:
                        print(f"✗ Service {service_key} missing required key: {key}")
                        return False
                
                # Validate port range
                port = service_data.get('port')
                if not (1 <= port <= 65535):
                    print(f"✗ Service {service_key} has invalid port: {port}")
                    return False
            
            print("✓ Configuration structure is valid")
            print(f"✓ Found {len(data['services'])} services configured")
            return True
            
        except Exception as e:
            print(f"✗ Validation failed: {e}")
            return False
    
    def list_services(self):
        """List all configured services"""
        print("\n=== Configured Services ===")
        
        for service_key, service_config in self.config.services.items():
            print(f"\n{service_key}:")
            print(f"  Name: {service_config.name}")
            print(f"  URL: {service_config.get_url()}")
            print(f"  Protocol: {service_config.protocol}")
            print(f"  Health Check: {service_config.health_check_endpoint}")
    
    def list_discovered_services(self):
        """List all discovered services from registry"""
        if not self.registry:
            self.load_service_registry()
        
        print("\n=== Discovered Services ===")
        
        services = self.registry.get_all_services()
        stats = self.registry.get_registry_stats()
        
        print(f"\nTotal: {stats['total']} services")
        print(f"Healthy: {stats['healthy']}")
        print(f"Unhealthy: {stats['unhealthy']}")
        print(f"Unknown: {stats['unknown']}")
        
        print("\nService Details:")
        for service in services:
            status_icon = "✓" if service.status == ServiceStatus.HEALTHY else "✗"
            print(f"\n{status_icon} {service.service_name} ({service.service_id})")
            print(f"  URL: {service.get_url()}")
            print(f"  Status: {service.status.value}")
            print(f"  Last Heartbeat: {service.last_heartbeat}")
    
    def register_service(self, service_name: str, url: str):
        """Register a service with the discovery system"""
        if not self.registry:
            self.load_service_registry()
        
        try:
            # Parse URL
            from urllib.parse import urlparse
            parsed = urlparse(url)
            host = parsed.hostname or 'localhost'
            port = parsed.port or 80
            protocol = parsed.scheme or 'http'
            
            # Create service instance
            service_id = f"{service_name}_{host}_{port}"
            service = ServiceInstance(
                service_id=service_id,
                service_name=service_name,
                host=host,
                port=port,
                protocol=protocol,
                health_check_endpoint="/health",
                metadata={"manually_registered": True}
            )
            
            # Register service
            if self.registry.register_service(service):
                print(f"✓ Successfully registered service: {service_name} at {url}")
            else:
                print(f"✗ Failed to register service: {service_name}")
                
        except Exception as e:
            print(f"✗ Error registering service: {e}")
    
    def unregister_service(self, service_name: str):
        """Unregister a service from the discovery system"""
        if not self.registry:
            self.load_service_registry()
        
        try:
            services = self.registry.get_all_services(service_name)
            if not services:
                print(f"✗ Service not found: {service_name}")
                return
            
            for service in services:
                if self.registry.unregister_service(service.service_id):
                    print(f"✓ Successfully unregistered service: {service_name} ({service.service_id})")
                else:
                    print(f"✗ Failed to unregister service: {service_name}")
                    
        except Exception as e:
            print(f"✗ Error unregistering service: {e}")
    
    def check_service_health(self, service_name: str):
        """Check health of a specific service"""
        if not self.registry:
            self.load_service_registry()
        
        try:
            service = self.registry.get_service(service_name)
            if not service:
                print(f"✗ Service not found: {service_name}")
                return
            
            # Check health via HTTP
            import asyncio
            async def check_health():
                status = await self.registry.check_service_health(service)
                print(f"\n=== Health Check: {service_name} ===")
                print(f"URL: {service.get_url()}")
                print(f"Status: {status.value}")
                return status
            
            result = asyncio.run(check_health())
            
        except Exception as e:
            print(f"✗ Error checking service health: {e}")
    
    def reload_configuration(self):
        """Reload platform configuration and service discovery"""
        print("\n=== Reloading Configuration ===")
        
        try:
            # Reload platform config
            reload_config()
            print("✓ Platform configuration reloaded")
            
            # Reload service discovery
            if self.registry:
                self.registry.reload_configuration()
                print("✓ Service discovery reloaded")
            
            print("✓ Configuration reload completed")
            
        except Exception as e:
            print(f"✗ Error reloading configuration: {e}")
    
    def generate_deployment_config(self, deployment_type: str, output_file: str):
        """Generate deployment configuration for specific scenario"""
        print(f"\n=== Generating {deployment_type} Deployment Configuration ===")
        
        try:
            # Load current config
            with open(self.config_file, 'r') as f:
                config = json.load(f)
            
            # Update based on deployment type
            if deployment_type == "single_machine":
                # Use localhost for all services
                for service_key in config['services']:
                    config['services'][service_key]['host'] = 'localhost'
                config['deployment_mode'] = 'single_machine'
                
            elif deployment_type == "distributed":
                # Use environment variable placeholders
                service_env_vars = {
                    'gateway': 'GATEWAY_HOST',
                    'core_backend': 'CORE_BACKEND_HOST',
                    'inventory_service': 'INVENTORY_SERVICE_HOST',
                    'postgres': 'POSTGRES_HOST',
                    'frontend': 'FRONTEND_HOST'
                }
                
                for service_key, env_var in service_env_vars.items():
                    if service_key in config['services']:
                        config['services'][service_key]['host'] = f"${env_var}"
                
                config['deployment_mode'] = 'distributed'
            
            # Write to output file
            with open(output_file, 'w') as f:
                json.dump(config, f, indent=2)
            
            print(f"✓ Generated {deployment_type} configuration: {output_file}")
            print(f"  Set deployment_mode to: {config['deployment_mode']}")
            
        except Exception as e:
            print(f"✗ Error generating deployment config: {e}")
    
    def test_gateway_connection(self):
        """Test connection to the API gateway"""
        print("\n=== Testing Gateway Connection ===")
        
        try:
            gateway_config = self.config.get_service('gateway')
            if not gateway_config:
                print("✗ Gateway service not found in configuration")
                return
            
            gateway_url = gateway_config.get_url()
            print(f"Testing connection to: {gateway_url}")
            
            # Test health endpoint
            response = requests.get(f"{gateway_url}/health", timeout=5)
            
            if response.status_code == 200:
                print("✓ Gateway is healthy")
                data = response.json()
                print(f"  Status: {data.get('status')}")
                print(f"  Services: {list(data.get('services', {}).keys())}")
            else:
                print(f"✗ Gateway returned status: {response.status_code}")
                
        except requests.exceptions.ConnectionError:
            print("✗ Cannot connect to gateway (connection refused)")
        except requests.exceptions.Timeout:
            print("✗ Gateway connection timed out")
        except Exception as e:
            print(f"✗ Error testing gateway: {e}")

def main():
    parser = argparse.ArgumentParser(
        description="Configuration Manager for Infrastructure Management Platform"
    )
    
    parser.add_argument(
        '--config',
        default='config/platform_config.json',
        help='Path to configuration file'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Validate command
    subparsers.add_parser('validate', help='Validate configuration')
    
    # List services command
    subparsers.add_parser('list', help='List configured services')
    
    # List discovered services command
    subparsers.add_parser('list-discovered', help='List discovered services')
    
    # Register service command
    register_parser = subparsers.add_parser('register-service', help='Register a service')
    register_parser.add_argument('name', help='Service name')
    register_parser.add_argument('url', help='Service URL')
    
    # Unregister service command
    unregister_parser = subparsers.add_parser('unregister-service', help='Unregister a service')
    unregister_parser.add_argument('name', help='Service name')
    
    # Health check command
    health_parser = subparsers.add_parser('health', help='Check service health')
    health_parser.add_argument('name', help='Service name')
    
    # Reload command
    subparsers.add_parser('reload', help='Reload configuration')
    
    # Generate deployment config command
    generate_parser = subparsers.add_parser('generate', help='Generate deployment configuration')
    generate_parser.add_argument('type', choices=['single_machine', 'distributed'], help='Deployment type')
    generate_parser.add_argument('output', help='Output file path')
    
    # Test gateway command
    subparsers.add_parser('test-gateway', help='Test gateway connection')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        sys.exit(1)
    
    # Initialize config manager
    manager = ConfigManager(args.config)
    
    # Execute command
    if args.command == 'validate':
        success = manager.validate_config()
        sys.exit(0 if success else 1)
    
    elif args.command == 'list':
        manager.list_services()
    
    elif args.command == 'list-discovered':
        manager.list_discovered_services()
    
    elif args.command == 'register-service':
        manager.register_service(args.name, args.url)
    
    elif args.command == 'unregister-service':
        manager.unregister_service(args.name)
    
    elif args.command == 'health':
        manager.check_service_health(args.name)
    
    elif args.command == 'reload':
        manager.reload_configuration()
    
    elif args.command == 'generate':
        manager.generate_deployment_config(args.type, args.output)
    
    elif args.command == 'test-gateway':
        manager.test_gateway_connection()

if __name__ == '__main__':
    main()