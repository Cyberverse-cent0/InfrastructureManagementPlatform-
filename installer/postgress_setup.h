#ifndef POSTGRESS_SETUP_H
#define POSTGRESS_SETUP_H

#include <sys/types.h>
#include <stdbool.h>

typedef struct {
    uid_t uid;
    gid_t gid;
    char *username;
    char *groups[];
} UserInfo;

typedef struct {
    char *debina;
    char *ubuntu;
    char *centos;
    char *fedora;
    char *arch;
    char *opensuse;
    char *alpine;
} distribution;

typedef struct {
    bool is_linux;
    bool is_root;
    bool has_root_privileges;
    distribution *distribution_info;
} Platform;

typedef enum {
    DB_OK = 0,
    DB_ERROR,
    DB_CONNECTION_ERROR,
    DB_QUERY_ERROR,
    DB_AUTH_ERROR,
    DB_INVALID_ARGUMENT
} db_status_t;

// must system check pass 
typedef struct {
    bool is_linux;
    int disk_size;
    bool suport_threading;
    int thread_count;
    int linux_kernel_version;
} system_check;

#endif // POSTGRESS_SETUP_H


