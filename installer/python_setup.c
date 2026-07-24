#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include "python_setup.h"
#include "common_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#ifdef DEBUG
bool DEBUG_MODE = true;
#else
bool DEBUG_MODE = false;
#endif

#ifdef BUFFER_SIZE
#undef BUFFER_SIZE
#endif
#define BUFFER_SIZE 1024

static int install_python_on_arch(bool is_root, bool DEBUG_MODE, char *python_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Python on Arch...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Python is installed\n");
        return status;
    } else {
        int status;
        char *package_name;
        if (python_version != NULL) {
            package_name = python_version;
        } else {
            package_name = "python";
        }
        
        int install_pkg(char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Python on Arch...\n");
            printf("Package: %s\n", pkg_name);
            
            // sync packages 
            pkg_status = system("pacman -Syy");
            if (pkg_status != 0) {
                printf("Error: Failed to sync packages\n");
                return pkg_status;
            }
            
            snprintf(command, sizeof(command), "pacman -S --noconfirm %s", pkg_name);
            pkg_status = system(command);
            if (pkg_status != 0) {
                printf("Error: Failed to install package\n");
                return pkg_status;
            }
            
            snprintf(command, sizeof(command), "pacman -Q %s", pkg_name);
            pkg_status = system(command);
            
            return pkg_status;
        }
        
        status = install_pkg(package_name);
        if (status != 0) {
            printf("Error: Failed to install package\n");
            status = install_pkg("python");
        }
        
        // Install pip
        status = install_pkg("python-pip");
        if (status != 0) {
            printf("Warning: Failed to install python-pip\n");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return 0;
    }
}

static int install_python_on_debian(bool is_root, bool DEBUG_MODE, char *python_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Python on Debian/Ubuntu...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Python is installed\n");
        return status;
    } else {
        int status;
        char *package_name;
        if (python_version != NULL) {
            package_name = python_version;
        } else {
            package_name = "python3";
        }
        
        int install_pkg(char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Python on Debian/Ubuntu...\n");
            printf("Package: %s\n", pkg_name);
            
            // Update package list
            pkg_status = system("apt-get update");
            if (pkg_status != 0) {
                printf("Error: Failed to update package list\n");
                return pkg_status;
            }
            
            // Install package
            snprintf(command, sizeof(command), "apt-get install -y %s", pkg_name);
            pkg_status = system(command);
            
            return pkg_status;
        }
        
        status = install_pkg(package_name);
        if (status != 0) {
            printf("Error: Failed to install package\n");
            status = install_pkg("python3");
        }
        
        // Install pip
        status = install_pkg("python3-pip");
        if (status != 0) {
            printf("Warning: Failed to install python3-pip\n");
        }
        
        // Install venv
        status = install_pkg("python3-venv");
        if (status != 0) {
            printf("Warning: Failed to install python3-venv\n");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return 0;
    }
}

static int install_python_on_fedora(bool is_root, bool DEBUG_MODE, char *python_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Python on Fedora/RHEL/CentOS...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Python is installed\n");
        return status;
    } else {
        int status;
        char *package_name;
        if (python_version != NULL) {
            package_name = python_version;
        } else {
            package_name = "python3";
        }
        
        int install_pkg(char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Python on Fedora/RHEL/CentOS...\n");
            printf("Package: %s\n", pkg_name);
            
            // Try dnf first (Fedora, RHEL 8+, CentOS 8+)
            snprintf(command, sizeof(command), "dnf install -y %s", pkg_name);
            pkg_status = system(command);
            
            // Fallback to yum if dnf not available
            if (pkg_status != 0) {
                snprintf(command, sizeof(command), "yum install -y %s", pkg_name);
                pkg_status = system(command);
            }
            
            return pkg_status;
        }
        
        status = install_pkg(package_name);
        if (status != 0) {
            printf("Error: Failed to install package\n");
            status = install_pkg("python3");
        }
        
        // Install pip
        status = install_pkg("python3-pip");
        if (status != 0) {
            printf("Warning: Failed to install python3-pip\n");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return 0;
    }
}

static int install_python_on_opensuse(bool is_root, bool DEBUG_MODE, char *python_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Python on openSUSE...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Python is installed\n");
        return status;
    } else {
        int status;
        char *package_name;
        if (python_version != NULL) {
            package_name = python_version;
        } else {
            package_name = "python3";
        }
        
        int install_pkg(char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Python on openSUSE...\n");
            printf("Package: %s\n", pkg_name);
            
            // Refresh repository
            pkg_status = system("zypper refresh");
            if (pkg_status != 0) {
                printf("Warning: Failed to refresh repositories\n");
            }
            
            // Install package
            snprintf(command, sizeof(command), "zypper install -y %s", pkg_name);
            pkg_status = system(command);
            
            return pkg_status;
        }
        
        status = install_pkg(package_name);
        if (status != 0) {
            printf("Error: Failed to install package\n");
            status = install_pkg("python3");
        }
        
        // Install pip
        status = install_pkg("python3-pip");
        if (status != 0) {
            printf("Warning: Failed to install python3-pip\n");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return 0;
    }
}

static int install_python_by_distribution(bool is_root, bool DEBUG_MODE, char *python_version, char* dependency) {
    char* distribution = get_distribution();
    
    if (distribution == NULL) {
        printf("Error: Could not determine distribution\n");
        return 1;
    }
    
    printf("Detected distribution: %s\n", distribution);
    
    // Check for Arch-based distributions
    if (strstr(distribution, "Arch") != NULL || 
        strstr(distribution, "Manjaro") != NULL ||
        strstr(distribution, "Endeavour") != NULL) {
        return install_python_on_arch(is_root, DEBUG_MODE, python_version, dependency);
    }
    // Check for Debian/Ubuntu-based distributions
    else if (strstr(distribution, "Debian") != NULL || 
             strstr(distribution, "Ubuntu") != NULL ||
             strstr(distribution, "Mint") != NULL ||
             strstr(distribution, "Pop") != NULL) {
        return install_python_on_debian(is_root, DEBUG_MODE, python_version, dependency);
    }
    // Check for Fedora/RHEL/CentOS-based distributions
    else if (strstr(distribution, "Fedora") != NULL || 
             strstr(distribution, "Red Hat") != NULL ||
             strstr(distribution, "RHEL") != NULL ||
             strstr(distribution, "CentOS") != NULL ||
             strstr(distribution, "Rocky") != NULL ||
             strstr(distribution, "Alma") != NULL) {
        return install_python_on_fedora(is_root, DEBUG_MODE, python_version, dependency);
    }
    // Check for openSUSE-based distributions
    else if (strstr(distribution, "openSUSE") != NULL || 
             strstr(distribution, "SUSE") != NULL) {
        return install_python_on_opensuse(is_root, DEBUG_MODE, python_version, dependency);
    }
    else {
        printf("Error: Unsupported distribution: %s\n", distribution);
        printf("Please manually install Python\n");
        return 1;
    }
}

static PythonInfo get_python_info(bool DEBUG_MODE) {
    PythonInfo info;
    char command[BUFFER_SIZE];
    FILE *fp;
    
    // Initialize fields
    info.version = NULL;
    info.executable_path = NULL;
    info.pip_available = false;
    info.venv_available = false;
    
    // Get Python version
    fp = popen("python3 --version 2>&1", "r");
    if (fp != NULL) {
        char buffer[BUFFER_SIZE];
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Remove trailing newline
            buffer[strcspn(buffer, "\n")] = 0;
            info.version = strdup(buffer);
        }
        pclose(fp);
    }
    
    // If python3 not found, try python
    if (info.version == NULL) {
        fp = popen("python --version 2>&1", "r");
        if (fp != NULL) {
            char buffer[BUFFER_SIZE];
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                buffer[strcspn(buffer, "\n")] = 0;
                info.version = strdup(buffer);
            }
            pclose(fp);
        }
    }
    
    // Get executable path
    fp = popen("which python3 2>&1", "r");
    if (fp != NULL) {
        char buffer[BUFFER_SIZE];
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            info.executable_path = strdup(buffer);
        }
        pclose(fp);
    }
    
    // Check if pip is available
    snprintf(command, sizeof(command), "pip3 --version > /dev/null 2>&1");
    if (system(command) == 0) {
        info.pip_available = true;
    }
    
    // Check if venv is available
    snprintf(command, sizeof(command), "python3 -m venv --help > /dev/null 2>&1");
    if (system(command) == 0) {
        info.venv_available = true;
    }
    
    if (DEBUG_MODE) {
        printf("Python Info:\n");
        printf("  Version: %s\n", info.version ? info.version : "Not found");
        printf("  Executable: %s\n", info.executable_path ? info.executable_path : "Not found");
        printf("  Pip available: %s\n", info.pip_available ? "Yes" : "No");
        printf("  Venv available: %s\n", info.venv_available ? "Yes" : "No");
    }
    
    return info;
}

static int configure_python_environment(bool DEBUG_MODE, char *project_path) {
    char command[BUFFER_SIZE];
    int status;
    
    if (project_path == NULL) {
        printf("Error: Project path is null\n");
        return 1;
    }
    
    printf("Configuring Python environment in: %s\n", project_path);
    
    // Create virtual environment
    snprintf(command, sizeof(command), "python3 -m venv %s/venv", project_path);
    status = system(command);
    if (status != 0) {
        printf("Error: Failed to create virtual environment\n");
        return status;
    }
    
    printf("Virtual environment created successfully\n");
    
    // Create requirements.txt if it doesn't exist
    char req_file[BUFFER_SIZE];
    snprintf(req_file, sizeof(req_file), "%s/requirements.txt", project_path);
    FILE *fp = fopen(req_file, "r");
    if (fp == NULL) {
        // Create empty requirements.txt
        fp = fopen(req_file, "w");
        if (fp != NULL) {
            fprintf(fp, "# Add your Python dependencies here\n");
            fclose(fp);
            printf("Created requirements.txt\n");
        }
    } else {
        fclose(fp);
    }
    
    // Install requirements if file exists and has content
    snprintf(command, sizeof(command), "%s/venv/bin/pip install -r %s/requirements.txt", project_path, project_path);
    status = system(command);
    if (status != 0) {
        printf("Note: No requirements to install or requirements.txt is empty\n");
    }
    
    if (DEBUG_MODE) {
        printf("Python environment configured successfully\n");
    }
    
    return 0;
}

static int setup_python_pip(bool DEBUG_MODE) {
    char command[BUFFER_SIZE];
    int status;
    
    printf("Setting up pip...\n");
    
    // Upgrade pip
    snprintf(command, sizeof(command), "pip3 install --upgrade pip");
    status = system(command);
    if (status != 0) {
        printf("Warning: Failed to upgrade pip\n");
        return status;
    }
    
    printf("pip upgraded successfully\n");
    
    // Install common development tools
    if (DEBUG_MODE) {
        printf("Installing common Python development tools...\n");
    }
    
    snprintf(command, sizeof(command), "pip3 install setuptools wheel");
    status = system(command);
    if (status != 0) {
        printf("Warning: Failed to install setuptools and wheel\n");
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    bool is_root = getuid() == 0;
    char *python_version = NULL;
    char *dependency = NULL;
    char *project_path = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 && i + 1 < argc) {
            python_version = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--dependency") == 0 && i + 1 < argc) {
            dependency = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--project-path") == 0 && i + 1 < argc) {
            project_path = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--debug") == 0) {
            DEBUG_MODE = true;
        }
    }
    
    printf("Python Installation and Configuration\n");
    printf("=====================================\n\n");
    
    // Check current Python installation
    PythonInfo info = get_python_info(DEBUG_MODE);
    
    if (info.version == NULL) {
        printf("Python is not installed. Installing...\n");
        int install_status = install_python_by_distribution(is_root, DEBUG_MODE, python_version, dependency);
        if (install_status != 0) {
            printf("Error: Failed to install Python\n");
            return 1;
        }
        
        // Get updated info after installation
        info = get_python_info(DEBUG_MODE);
    } else {
        printf("Python is already installed: %s\n", info.version);
    }
    
    // Setup pip
    if (info.pip_available) {
        setup_python_pip(DEBUG_MODE);
    } else {
        printf("Warning: pip is not available\n");
    }
    
    // Configure environment if project path is provided
    if (project_path != NULL && info.venv_available) {
        configure_python_environment(DEBUG_MODE, project_path);
    }
    
    printf("\nPython setup completed successfully\n");
    
    // Clean up
    if (info.version) free(info.version);
    if (info.executable_path) free(info.executable_path);
    
    return 0;
}
