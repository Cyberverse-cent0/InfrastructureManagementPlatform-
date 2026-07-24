#ifndef PYTHON_SETUP_H
#define PYTHON_SETUP_H

#include <sys/types.h>
#include <stdbool.h>

typedef enum {
    PYTHON_OK = 0,
    PYTHON_ERROR,
    PYTHON_NOT_INSTALLED,
    PYTHON_VERSION_ERROR,
    PYTHON_PIP_ERROR,
    PYTHON_VENV_ERROR
} python_status_t;

typedef struct {
    char *version;
    char *executable_path;
    bool pip_available;
    bool venv_available;
} PythonInfo;

#endif // PYTHON_SETUP_H
