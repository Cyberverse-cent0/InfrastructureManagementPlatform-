# Infrastructure Management Platform Installer

This directory contains installation scripts for setting up PostgreSQL and Python environments across different Linux distributions.

## Components

### 1. PostgreSQL Installation (`postgress_setup.c`)
- **Distribution Support**: Arch Linux, Debian/Ubuntu, Fedora/RHEL/CentOS, openSUSE
- **Features**:
  - Automatic distribution detection
  - Package manager integration (pacman, apt, dnf/yum, zypper)
  - Dependency installation
  - System configuration
  - Debug mode support

### 2. Python Installation (`python_setup.c`)
- **Distribution Support**: Arch Linux, Debian/Ubuntu, Fedora/RHEL/CentOS, openSUSE
- **Features**:
  - Automatic distribution detection
  - Python version management
  - pip installation and configuration
  - Virtual environment setup
  - Project configuration with requirements.txt
  - Debug mode support

### 3. Common Utilities (`common_utils.c/h`)
- Shared distribution detection function
- Cross-platform compatibility utilities

## Building

```bash
# Build all installers
make

# Build with debug symbols
make debug

# Clean build artifacts
make clean
```

## Usage

### PostgreSQL Installer
```bash
# Run with default settings
sudo ./postgres_installer

# Debug mode
sudo ./postgres_installer --debug

# Specify package version
sudo ./postgres_installer --version postgresql-14
```

### Python Installer
```bash
# Run with default settings
sudo ./python_installer

# Debug mode
sudo ./python_installer --debug

# Specify Python version
sudo ./python_installer --version python3.11

# Setup project environment
sudo ./python_installer --project-path /path/to/project

# Install with dependencies
sudo ./python_installer --dependency python3-dev
```

## Installation

```bash
# Install system-wide
make install

# Uninstall
make uninstall
```

## Distribution Support Matrix

| Distribution | PostgreSQL | Python | Package Manager |
|--------------|------------|--------|-----------------|
| Arch Linux   | ✅         | ✅     | pacman          |
| Debian       | ✅         | ✅     | apt-get         |
| Ubuntu       | ✅         | ✅     | apt-get         |
| Fedora       | ✅         | ✅     | dnf             |
| RHEL/CentOS  | ✅         | ✅     | dnf/yum         |
| openSUSE     | ✅         | ✅     | zypper          |

## Architecture

The installer follows a modular design:
1. **Distribution Detection**: Automatically identifies the Linux distribution
2. **Package Manager Selection**: Chooses appropriate package manager
3. **Installation**: Installs software using distribution-specific commands
4. **Configuration**: Sets up environment and dependencies
5. **Verification**: Checks installation success

## Debug Mode

Both installers support debug mode for testing and troubleshooting:
```bash
sudo ./installer --debug
```

Debug mode:
- Skips actual package installation
- Tests distribution detection
- Validates command generation
- Provides detailed logging

## File Structure

```
installer/
├── common_utils.c           # Shared utilities
├── common_utils.h           # Shared header
├── postgress_setup.c        # PostgreSQL installer
├── postgress_setup.h        # PostgreSQL header
├── python_setup.c           # Python installer
├── python_setup.h           # Python header
├── Makefile                 # Build configuration
└── README.md               # This file
```

## Requirements

- Linux kernel 5.0+
- Root privileges (or sudo)
- Network connectivity for package downloads
- Minimum 10GB disk space

## Troubleshooting

### Distribution not detected
- Ensure `/etc/os-release` exists and is readable
- Check that the distribution name matches supported distributions

### Installation fails
- Verify network connectivity
- Check package manager configuration
- Ensure sufficient disk space
- Run with `--debug` flag for detailed logging

### Permission errors
- Run with sudo or as root
- Check file system permissions

## Contributing

When adding support for new distributions:
1. Add distribution detection logic in `common_utils.c`
2. Create installation function in respective setup file
3. Add dispatcher case in `install_*_by_distribution()`
4. Update this README with distribution support matrix
