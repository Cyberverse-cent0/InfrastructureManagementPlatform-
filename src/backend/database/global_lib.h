#ifndef GLOBAL_LIB_H
#define GLOBAL_LIB_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>
#include "../../../scipts/print_function.h"

#ifdef _DATA_DIR
#define DATA_DIR "working_dir/data"
#endif
#ifdef DATABASE_NAME
#define DATABASE_NAME "infrastructure_platform"
#endif

typedef enum {
    ADMIN,
    NORMAL,
    VISITOR
} role;

typedef struct {
    PGconn *connection;
    bool is_connected;
    char connection_string[512];
} database_t;

#endif // GLOBAL_LIB_H