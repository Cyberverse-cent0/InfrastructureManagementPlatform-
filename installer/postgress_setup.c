#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include "postgress_setup.h"
#include "common_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <string.h>
#include "../scipts/get_thead.h"



#ifdef DEBUG
bool DEBUG_MODE = true;
#else
bool DEBUG_MODE = false;
#endif
#ifdef BUFFER_SIZE
#undef BUFFER_SIZE
#endif
#define BUFFER_SIZE 1024
const char* default_password = "password";
const int minimal_disk_size = 10;
char install_dir[BUFFER_SIZE] = "/var/InfrastructureManagementPlatform";

static UserInfo get_current_user() {
    UserInfo user;
    user.uid = getuid();
    user.gid = getgid();
    user.username = getlogin();
    // Simplified group handling
    gid_t groups[32];
    int ngroups = getgroups(32, groups);
    (void)ngroups; // Suppress unused warning
    return user;
}
 
static const char* installation_dir(char* installation_dir){ // to do fix any spelling errors
    if (installation_dir != NULL) {
        return installation_dir;
    }
    return install_dir;
}

static int get_disk_size(const char* path) {
    struct statvfs fs;
    if (statvfs(path, &fs) == 0) {
        return (int)((fs.f_blocks * fs.f_frsize) / (1024 * 1024 * 1024));
    }
    return 0;
}

static system_check must_system_check() {
    system_check check;
    check.is_linux = system("uname -s") == 0; // to do reduce and redant code theat still check if it linux in the code bock
    if(check.is_linux == 0) {
        fprintf(stderr, "Error: Linux distribution required\n");
        fprintf(stderr, "Please run this script on a Linux Kernel\n");
        exit(1);
    }
    check.disk_size = get_disk_size(install_dir);
    
    if (check.disk_size < minimal_disk_size) {
        fprintf(stderr, "Error: Disk size must be at least %dGB\n", minimal_disk_size);
        exit(1);
    }
    check.suport_threading = check_if_support_threading();
    check.thread_count = get_thread_count();

    // Check kernel version by parsing uname -r output
    FILE* pipe = popen("uname -r", "r");
    if (pipe != NULL) {
        char kernel_version[32];
        if (fgets(kernel_version, sizeof(kernel_version), pipe) != NULL) {
            float version = strtof(kernel_version, NULL);
            if (version < 5.0f) {
                fprintf(stderr, "Error: Linux kernel version must be at least 5.0\n");
                exit(1);
            }
        }
        pclose(pipe);
    }
    
    return check;
}




static Platform get_platform(bool DEBUG_MODE) {
    Platform platform;
    platform.is_linux = system("uname -s") == 0;
    platform.is_root = getuid() == 0;
    if(platform.is_root != 0 && !DEBUG_MODE) {
        fprintf(stderr, "Error: Root privileges required\n");
        exit(1);
    } else if(DEBUG_MODE) {
        platform.is_root = true;
    }
    platform.has_root_privileges = platform.is_root;
    
    return platform;
}




static int install_postgress_on_arch(bool is_root, bool DEBUG_MODE, char *postgress_pkg_name_or_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Postgres on Arch...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Postgress is installed\n");
        return status;
    } else{
        int status;
        const char *package_name;
        if (postgress_pkg_name_or_version != NULL) {
           package_name = postgress_pkg_name_or_version;
        } else{
            package_name = "postgresql";
        }
        
        int install_pkg(const char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Postgres on Arch...\n");
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
            // Try default package name
            status = install_pkg("postgresql");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
    }
    
    return 0;
}pkg_

// to impoemnt tpkg_he code for other distos 

static int install_postgress_on_debian(bool is_root, bool DEBUG_MODE, char *postgress_pkg_name_or_version, char* dependency) {
    if (is_root == pkg_false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
   } el{
        if (DEBUG_MODE) {
            printf("Installing Postgres on Debian/Ubuntu...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Postgress is installed\n");
        return status;
    } else {
        int status;
        char *package_name;
        if (postgress_pkg_name_or_version != NULL) {
            package_name = postgress_pkg_name_or_version;
        } else {
            package_name = "postgresql";
        }
        
        int install_pkg(char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Postgres on Debian/Ubuntu...\n");
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
            // Try with default postgresql package
            status = install_pkg("postgresql");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return status;
    }
}

static int install_postgress_on_fedora(bool is_root, bool DEBUG_MODE, char *postgress_pkg_name_or_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Postgres on Fedora/RHEL/CentOS...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Postgress is installed\n");
        return status;
    } else {
        char *package_name;
        if (postgress_pkg_name_or_version != NULL) {
            package_name = postgress_pkg_name_or_version;
        } else {
            package_name = "postgresql-server";
        }
        
        static int install_pkg(char *package_name) {
            char command[BUFFER_SIZE];
            int status;
            
            printf("Installing Postgres on Fedora/RHEL/CentOS...\n");
            printf("Package: %s\n", package_name);
            
            // Try dnf first (Fedora, RHEL 8+, CentOS 8+)
            snprintf(command, sizeof(command), "dnf install -y %s", package_name);
            status = system(command);
            
            // Fallback to yum if dnf not available
            if (status != 0) {
                snprintf(command, sizeof(command), "yum install -y %s", package_name);
                status = system(command);
            }
            
            return status;
        }
        
        int status = install_pkg(package_name);
        if (status != 0) {
            printf("Error: Failed to install package\n");
            // Try with default postgresql-server package
            status = install_pkg("postgresql-server");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return status;
    }
}

static int install_postgress_on_opensuse(bool is_root, bool DEBUG_MODE, char *postgress_pkg_name_or_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return 1;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Postgres on openSUSE...\n");
            printf("Note running in debug mode\n");
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Postgress is installed\n");
        return status;
    } else {
        int status;
        char *package_name;
        if (postgress_pkg_name_or_version != NULL) {
            package_name = postgress_pkg_name_or_version;
        } else {
            package_name = "postgresql-server";
        }
        
        int install_pkg(char *pkg_name) {
            char command[BUFFER_SIZE];
            int pkg_status;
            
            printf("Installing Postgres on openSUSE...\n");
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
            // Try with default postgresql-server package
            status = install_pkg("postgresql-server");
        }
        
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
        
        return status;
    }
}

static int install_postgress_by_distribution(bool is_root, bool DEBUG_MODE, char *postgress_pkg_name_or_version, char* dependency) {
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
        return install_postgress_on_arch(is_root, DEBUG_MODE, postgress_pkg_name_or_version, dependency);
    }
    // Check for Debian/Ubuntu-based distributions
    else if (strstr(distribution, "Debian") != NULL || 
             strstr(distribution, "Ubuntu") != NULL ||
             strstr(distribution, "Mint") != NULL ||
             strstr(distribution, "Pop") != NULL) {
        return install_postgress_on_debian(is_root, DEBUG_MODE, postgress_pkg_name_or_version, dependency);
    }
    // Check for Fedora/RHEL/CentOS-based distributions
    else if (strstr(distribution, "Fedora") != NULL || 
             strstr(distribution, "Red Hat") != NULL ||
             strstr(distribution, "RHEL") != NULL ||
             strstr(distribution, "CentOS") != NULL ||
             strstr(distribution, "Rocky") != NULL ||
             strstr(distribution, "Alma") != NULL) {
        return install_postgress_on_fedora(is_root, DEBUG_MODE, postgress_pkg_name_or_version, dependency);
    }
    // Check for openSUSE-based distributions
    else if (strstr(distribution, "openSUSE") != NULL || 
             strstr(distribution, "SUSE") != NULL) {
        return install_postgress_on_opensuse(is_root, DEBUG_MODE, postgress_pkg_name_or_version, dependency);
    }
    else {
        printf("Error: Unsupported distribution: %s\n", distribution);
        printf("Please manually install PostgreSQL\n");
        return 1;
    }
}

static int postgress_system_setup(bool DEBUG_MODE, bool is_root, char *package_name) {
    int status;
    (void)DEBUG_MODE; // Suppress unused parameter warning
    (void)is_root; // Suppress unused parameter warning
    
    if(package_name == NULL) {
        printf("Error: Package name is null\n");
        return 1;
    }
    
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "%s --version > /dev/null 2>&1", package_name);
    status = system(command);
    if (status != 0){
        printf("Error: %s is not installed\n", package_name);
        return 1;
    }

    if (DEBUG_MODE) {
        printf("Postgress is setup\n");
    }
    
    return 0;
}

static int __init_without_system_d(char *package_name[])
{
    if (package_name == NULL || package_name[0] == NULL) {
        fprintf(stderr, "Error: Package name is null\n");
        return 1;
    }

    for (int i = 0; package_name[i] != NULL; i++) {

        pid_t installer_pid = fork();

        if (installer_pid < 0) {
            fprintf(stderr,
                    "Error: fork failed for %s: %s\n",
                    package_name[i],
                    strerror(errno));

            return 1;
        }

        if (installer_pid == 0) {
            /*
             * CHILD PROCESS
             *
             * execlp() searches PATH automatically.
             */
            execlp(package_name[i],
                   package_name[i],
                   (char *)NULL);

            /*
             * If exec succeeds, this code is never reached.
             */
            fprintf(stderr,
                    "Error: Failed to execute %s: %s\n",
                    package_name[i],
                    strerror(errno));

            _exit(127);
        }

        /*
         * PARENT PROCESS
         */
        int status;

        if (waitpid(installer_pid, &status, 0) == -1) {
            fprintf(stderr,
                    "Error: waitpid failed: %s\n",
                    strerror(errno));

            return 1;
        }

        if (WIFEXITED(status)) {

            int exit_status = WEXITSTATUS(status);

            if (exit_status != 0) {
                fprintf(stderr,
                        "%s exited with status %d\n",
                        package_name[i],
                        exit_status);

                return exit_status;
            }

        } else if (WIFSIGNALED(status)) {

            fprintf(stderr,
                    "%s was killed by signal %d\n",
                    package_name[i],
                    WTERMSIG(status));

            return 1;
        }
    }

    return 0;
}



static bool check_for_network_connectivity(
    bool DEBUG_MODE,
    int timeout,
    int trials
)
{
    if (DEBUG_MODE) {
        printf("[DEBUG] Network connectivity assumed.\n");
        // still run just for the sake
        int status = system("ping -c 1 8.8.8.8 > /dev/null 2>&1");
        fprintf(stderr, "[DEBUG] ping status: %d\n", status);
        return true;
    }

    if (timeout <= 0) {
        fprintf(stderr, "[ERROR] Timeout must be greater than 0.\n");
        return false;
    }

    if (trials <= 0) {
        fprintf(stderr, "[ERROR] Trials must be greater than 0.\n");
        return false;
    }

    for (int attempt = 1; attempt <= trials; attempt++) {

        printf("[NETWORK] Connectivity test %d/%d...\n",
               attempt, trials);

        char command[128];

        snprintf(
            command,
            sizeof(command),
            "ping -c 1 -W %d 8.8.8.8 > /dev/null 2>&1",
            timeout
        );

        int status = system(command);

        if (status == -1) {
            perror("[ERROR] system");
            continue;
        }

        if (WIFEXITED(status) &&
            WEXITSTATUS(status) == 0) {

            printf("[NETWORK] Connection available.\n");
            return true;
        }
    }

    printf("[NETWORK] No network connection detected.\n");

    return false;
}

static int init_database(
    bool DEBUG_MODE,
    char* dir
)
{
    bool is_root = geteuid() == 0;
    if (DEBUG_MODE && is_root) {
        printf("[DEBUG] Database initialization assumed.\n");
        return 0;
    }
    if (dir == NULL) {
        fprintf(stderr, "[WARNING] Directory is NULL.\n");
        dir = "/var/lib/postgres/data/";

    }
    int status = systsem("sudo -u postgres initdb -D %s", dir);


    // TODO: Implement database initialization
    return 0;
}

// to do siwtch a type sture retun type for this function as it will be reusade on api
int user_setup( 
    char* user_name;
    char* password;
){
    if(!user_name){
        user_name = system("whoami");
    }
    if (!password){
        password = defalt_password;

    }
    if (DEBUG_MODE) {
        printf("[DEBUG] User setup assumed.\n");
        return 0;
    } else{
        postgress_pipe_in("CREATE USER cyberverse WITH PASSWORD %s ;", password);
        
    }
    

}

// witch to better typesture for better and one not one line code 
static postgres_user_status_t postgres_user_setup( PGconn *connection, const char *user_name, const char *password ) { /* * ------------------------- * 1. Validate arguments * ------------------------- */ if (connection == NULL) return POSTGRES_USER_CONNECTION_ERROR; if (user_name == NULL || user_name[0] == '\0') return POSTGRES_USER_INVALID_NAME; if (password == NULL || password[0] == '\0') return POSTGRES_USER_INVALID_PASSWORD; /* * ------------------------- * 2. Check connection * ------------------------- */ if (PQstatus(connection) != CONNECTION_OK) { fprintf(stderr, "[POSTGRES] Connection failed: %s\n", PQerrorMessage(connection)); return POSTGRES_USER_CONNECTION_ERROR; } /* * ------------------------- * 3. Check if user exists * ------------------------- */ const char *check_params[] = { user_name }; PGresult *result = PQexecParams( connection, "SELECT 1 FROM pg_roles WHERE rolname = $1;", 1, NULL, check_params, NULL, NULL, 0 ); if (result == NULL) return POSTGRES_USER_QUERY_ERROR; if (PQresultStatus(result) != PGRES_TUPLES_OK) { fprintf(stderr, "[POSTGRES] User check failed: %s\n", PQerrorMessage(connection)); PQclear(result); return POSTGRES_USER_QUERY_ERROR; } if (PQntuples(result) > 0) { PQclear(result); printf("[POSTGRES] User '%s' already exists.\n", user_name); return POSTGRES_USER_EXISTS; } PQclear(result); /* * ------------------------- * 4. Create the user * ------------------------- * * PostgreSQL parameters can safely * handle the password, but usernames * are identifiers and cannot be passed * as $1. * * PQescapeIdentifier() safely quotes it. */ char *safe_user_name = PQescapeIdentifier( connection, user_name, strlen(user_name) ); if (safe_user_name == NULL) return POSTGRES_USER_QUERY_ERROR; const char *create_params[] = { password }; char query[1024]; int written = snprintf( query, sizeof(query), "CREATE USER %s WITH PASSWORD $1;", safe_user_name ); PQfreemem(safe_user_name); if (written < 0 || (size_t)written >= sizeof(query)) { return POSTGRES_USER_QUERY_ERROR; } /* * ------------------------- * 5. Execute CREATE USER * ------------------------- */ result = PQexecParams( connection, query, 1, NULL, create_params, NULL, NULL, 0 ); if (result == NULL) return POSTGRES_USER_QUERY_ERROR; if (PQresultStatus(result) != PGRES_COMMAND_OK) { fprintf(stderr, "[POSTGRES] Failed to create user '%s': %s\n", user_name, PQerrorMessage(connection)); PQclear(result); return POSTGRES_USER_QUERY_ERROR; } PQclear(result); printf("[POSTGRES] User '%s' created successfully.\n", user_name); return POSTGRES_USER_OK; }








static install_python(){
    const int python_version   > 3.11;
    int curent_python_version = system("python --version")
    if (curent_python_version < python_version) {
        print_warring   ("Python version is too old\n");
        return 1;
    }    // impoent instaion of theon 
    
    
    return 0;
}

static install_python_depedecy(char *working_dir, char* instaion_dir){

    char *dependency[];
    FILE *file = fopen("requirements.txt", "r");
    if (file == NULL) {
        printf("Error: Failed to open requirements.txt\n");
        return 1;
    }
    
    // read the file line by line
    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file)) {
        // remove newline
        line[strcspn(line, "\n")] = 0;
        dependency = line;
        for (int i = 0; sizeof(dependency)/sizeof(dependency[0]); i++) {
            char command[256];
            snprintf(command, sizeof(command), "pip install %s", dependency);
            system(command);
            static venv_perser_for_install(char *working_dir, char* instaion_dir){
    
}
            
        }
        
    }
    
    fclose(file);
    
    return 0;
}


static pip_install(){

}

static venv_create(){

}

