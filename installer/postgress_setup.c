#include <unistd.h>
#include "postgress_setup.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "get_thead.h"



#ifdef DEBUG
bool DEBUG_MODE = true;
#else
bool DEBUG_MODE = false;
#endif
#ifdef BUFFER_SIZE
#undef BUFFER_SIZE
#endif
#define BUFFER_SIZE 1024
const chardefalt_password = password;
const int minimal_disk_size = 10;
char install_dir[BUFFER_SIZE] = "/var/InfrastructureManagementPlatform";

static UserInfo get_current_user() {
    UserInfo user;
    user.uid = getuid();
    user.gid = getgid();
    user.username = getlogin();
    for (int i = 0; i < 10; i++) {
        user.groups[i] = getgroups(i, &user.groups[i]);
    }
    return user;
}
 
static const char* instaion_dir(char* instation_dir){ // to do fix any spelling errors
    if (instation_dir != NULL) {
        instaion_dir = instaion_dir;
    }
    return const install_dir;
}

static system_check must_system_check() {
    system_check check;
    check.is_linux = system("uname -s") == 0; // to do reduce and redant code theat still check if it linux in the code bock
    if(check.is_linux == 0) {
        fprintf(stderr, "Error: Linux distribution required\n");
        fprintf(stderr, "Please run this script on a Linux Kernel\n");
        exit(1);
    }
    check.disk_size = 0;
    
    static int disk_size() {
        struct statvfs fs;
        if (statvfs("%s", install_dir, &fs) == 0) {
            check.disk_size = (fs.f_blocks * fs.f_frsize) / (1024 * 1024 * 1024);
        }
        return check.disk_size;
    } 

    check.disk_size = disk_size();
    if (check.disk_size < minimal_disk_size) {
        fprintf(stderr, "Error: Disk size must be at least %dGB\n", minimal_disk_size);
        exit(1);
    }
    check.suport_threading = check_if_suport_theadding();
    check.thread_count = get_thread_count();

    check.linux_kernel_version = system("uname -r");
    if (check.linux_kernel_version < 5.0.1) {
        fprintf(stderr, "Error: Linux kernel version must be at least 5.0.1\n");
        exit(1);
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
    char* get_distribution(void) {
        char* result = NULL;
        char buffer[BUFFER_SIZE] = {0};
        FILE* fp = NULL;
        char* pretty_name = NULL;
        char* name = NULL;
        char* version = NULL;
        char* version_id = NULL;
        
        // Try to read /etc/os-release directly (more reliable than system())
        fp = fopen("/etc/os-release", "r");
        if (fp == NULL) {
            // Fallback to /etc/lsb-release
            fp = fopen("/etc/lsb-release", "r");
            if (fp == NULL) {
                // Last resort: try system command
                fp = popen("cat /etc/os-release 2>/dev/null", "r");
                if (fp == NULL) {
                    return strdup("Error: Could not determine distribution");
                }
            }
        }
        
        // Parse the file line by line
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Remove trailing newline
            buffer[strcspn(buffer, "\n")] = 0;
            
            // Look for PRETTY_NAME (preferred)
            if (strncmp(buffer, "PRETTY_NAME=", 12) == 0) {
                char* value = buffer + 12;
                // Remove quotes if present
                if (*value == '"' || *value == '\'') {
                    value++;
                    char* end = value + strlen(value) - 1;
                    if (*end == '"' || *end == '\'') *end = 0;
                }
                pretty_name = strdup(value);
            }
            // Look for NAME
            else if (strncmp(buffer, "NAME=", 5) == 0) {
                char* value = buffer + 5;
                if (*value == '"' || *value == '\'') {
                    value++;
                    char* end = value + strlen(value) - 1;
                    if (*end == '"' || *end == '\'') *end = 0;
                }
                name = strdup(value);
            }
            // Look for VERSION
            else if (strncmp(buffer, "VERSION=", 8) == 0) {
                char* value = buffer + 8;
                if (*value == '"' || *value == '\'') {
                    value++;
                    char* end = value + strlen(value) - 1;
                    if (*end == '"' || *end == '\'') *end = 0;
                }
                version = strdup(value);
            }
            // Look for VERSION_ID
            else if (strncmp(buffer, "VERSION_ID=", 11) == 0) {
                char* value = buffer + 11;
                if (*value == '"' || *value == '\'') {
                    value++;
                    char* end = value + strlen(value) - 1;
                    if (*end == '"' || *end == '\'') *end = 0;
                }
                version_id = strdup(value);
            }
        }
        
        fclose(fp);
        
        // Build result string with best available information
        if (pretty_name) {
            result = pretty_name;
            free(name);
            free(version);
            free(version_id);
        } else if (name && version) {
            asprintf(&result, "%s %s", name, version);
            free(name);
            free(version);
            free(version_id);
        } else if (name && version_id) {
            asprintf(&result, "%s %s", name, version_id);
            free(name);
            free(version_id);
        } else if (name) {
            result = name;
            free(version);
            free(version_id);
        } else {
            result = strdup("Unknown distribution");
        }
        
        return result;
    }
// to to perfet this function
    
    
    return platform;
}




static int install_postgress_on_arch(bool is_root, bool DEBUG_MODE, char *postgress_pkg_name_or_version, char* dependency) {
    if (is_root == false || DEBUG_MODE == false) {
        printf("Error: This function must be run as root\n");
        return;
    } else {
        if (DEBUG_MODE) {
            printf("Installing Postgres on Arch...\n");
            printf("Note ruing in debug mode")
        }
    }
    if (DEBUG_MODE == true) {
        int status = 0;
        printf("Postgress is installed")
    } else{

        if (postgress_pkg_name_or_version != NULL) {
            char *package_name = postgress_pkg_name_or_version;
        } else{
            char *package_name = "postgresql";
        }
        static int install_pkg(char *package_name){
            for (int i = 0; i < 10; i++) {
            printf("Installing Postgres on Arch...\n");
            printf("Package: %s\n", package_name);
            // sync packages 
            system("pacman -Syy");
            system("pacman -S --noconfirm %s", package_name);
            status = system("pacman -Q %s", package_name);
        }
        }
        install_pkg(package_name);
        if (status != 0) {
            printf("Error: Failed to install package\n");
            install_pkg(postgresql);
        }
        if (dependency != NULL) {
            install_pkg(dependency);
        } else {
            printf("No dependency found\n");
            printf("skipping.....\n");
        }
    }
    
    return status;
}

// to impoemnt the code for other distos 


static int postgress_system_setup(bool DEBUG_MODE, bool is_root, char *package_name) {
    int status;
    if(package_name == NULL) {
        printf("Error: Package name is null\n");
        return 1;
    } else if(
        status = system("%s --version > /dev/null 2>&1", package_name);
        if (status != 0){
            printf("Error: %s is not installed\n", package_name);
            status = 1;
            char packages[];
            packages[0] = package_name;
            packages[1] = "systemd";
        }

    }
    if (DEBUG_MODE) {
        printf("Postgress is setup\n");
    }

    static int check_systemd_exist(char *package_name){
        // check if systemd is installed
        status = system("systemctl --version > /dev/null 2>&1");
        status = system("systemctl status %s | grep active", package_name);
        if (status){
            status = system("systemctl restart --now %s", package_name);
            if (status != 0){
                printf("Error: Failed to restart %s\n", package_name);
                return 1;
            }
        } else {
            printf("%s is not running\n", package_name);
            status = system("systemctl start --now %s", package_name);
            if (status != 0){
                printf("Error: Failed to start %s\n", package_name);
                status = 1;
            }
        }
        

    }
    

    return status;
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





