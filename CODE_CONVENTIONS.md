# Code Conventions and Standards

## Overview
This document defines the coding standards and conventions for the Infrastructure Management Platform C backend projects (Core Backend and Microservices Backend).

## File Organization

### Directory Structure
```
project_name/
├── src/
│   ├── main/           # Entry points
│   ├── network/        # Networking components
│   ├── database/       # Database layer
│   ├── api/           # API layer
│   ├── business/      # Business logic
│   └── utils/         # Utilities
├── include/           # Public headers
├── config/           # Configuration files
├── tests/            # Test files
└── docs/             # Documentation
```

### File Naming
- **Source files**: `snake_case.c` (e.g., `http_server.c`)
- **Header files**: `snake_case.h` (e.g., `http_server.h`)
- **Test files**: `test_<module>.c` (e.g., `test_http_server.c`)
- **Configuration**: `<component>.conf` (e.g., `service.conf`)

## Code Style

### Indentation
- **Spaces**: 4 spaces (no tabs)
- **Indentation level**: Consistent 4-space increments
- **Alignment**: Align related elements vertically

### Line Length
- **Maximum**: 80 characters
- **Preferred**: 70-75 characters
- **Exceptions**: URLs, long string literals

### Brace Style
Use Allman style (braces on separate lines):

```c
if (condition)
{
    // code
}
else
{
    // code
}

void function_name(void)
{
    // code
}
```

### Spacing
- Space after commas: `function(arg1, arg2, arg3)`
- Space around operators: `a = b + c`
- No space after function names: `function()`
- Space after keywords: `if (condition)`

## Naming Conventions

### Functions
**Public functions**: `module_action_object()`
```c
user_create()
product_update()
database_connect()
```

**Private functions**: `static module_action_object()`
```c
static user_validate()
static product_format_json()
static database_execute_query()
```

**Handlers**: `<resource>_<method>_handler()`
```c
user_get_handler()
product_post_handler()
asset_delete_handler()
```

### Variables
**Local variables**: `snake_case`
```c
int user_id;
char *user_name;
bool is_active;
```

**Global variables**: `g_snake_case`
```c
static database_t *g_database_connection;
static int g_server_port;
```

**Constants**: `UPPER_SNAKE_CASE`
```c
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 8080
#define CONNECTION_TIMEOUT 30
```

### Structs/Types
**Type names**: `snake_case_t`
```c
typedef struct user user_t;
typedef struct server_context server_context_t;
typedef struct database_connection database_t;
```

**Struct members**: `snake_case`
```c
struct user
{
    int id;
    char *name;
    char *email;
};
```

### Enums
**Enum types**: `snake_case`
```c
typedef enum
{
    ACTIVE,
    INACTIVE,
    PENDING
} status_t;
```

**Enum values**: `UPPER_SNAKE_CASE`
```c
typedef enum
{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE
} http_method_t;
```

## Documentation Standards

### File Headers
Every source file should begin with a file header:

```c
/**
 * @file filename.c
 * @brief Brief description of file purpose
 * @author Author Name
 * @date 2024-07-24
 * 
 * Detailed description of the file's purpose,
 * main functionality, and usage examples.
 */
```

### Function Documentation
Every public function should have documentation:

```c
/**
 * @brief Brief function description
 * 
 * Detailed description of function functionality,
 * parameters, return values, and usage notes.
 * 
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 * 
 * @example
 * result = function_name(arg1, arg2);
 */
```

### Struct Documentation
Document struct definitions:

```c
/**
 * @brief Brief struct description
 * 
 * Detailed description of struct purpose and
 * member usage.
 */
typedef struct
{
    int id;              ///< Brief member description
    char *name;          ///< Brief member description
    bool is_active;      ///< Brief member description
} user_t;
```

### Inline Comments
Use inline comments to explain complex logic:

```c
// Check if user has permission to access resource
if (user_has_permission(user, resource))
{
    // Grant access
    return true;
}
```

## Error Handling

### Return Values
- Use `bool` for success/failure operations
- Use `int` for operations with multiple return codes
- Use `NULL` for pointer-returning functions on failure

```c
bool success = user_create(user_data);
if (!success)
{
    // Handle error
}

int result = database_execute_query(query);
if (result != 0)
{
    // Handle error
}

user_t *user = user_find_by_id(id);
if (user == NULL)
{
    // Handle error
}
```

### Error Messages
Use descriptive error messages with context:

```c
print_error("Failed to connect to database: %s", error_message);
print_error("User creation failed: invalid email format");
print_error("File not found: %s", file_path);
```

### Error Propagation
Propagate errors up the call stack when appropriate:

```c
bool process_user_data(user_t *user)
{
    if (!user_validate(user))
    {
        return false; // Validation failed
    }
    
    if (!user_save(user))
    {
        return false; // Save failed
    }
    
    return true; // Success
}
```

## Memory Management

### Allocation
Always check allocation results:

```c
char *buffer = malloc(size);
if (buffer == NULL)
{
    print_error("Memory allocation failed");
    return NULL;
}
```

### Deallocation
Always free allocated memory:

```c
void process_data(void)
{
    char *data = allocate_data();
    if (data != NULL)
    {
        // Use data
        free(data);
    }
}
```

### Memory Leaks
- Use tools like Valgrind to detect leaks
- Implement cleanup functions for complex structures
- Use RAII-like patterns when possible

## Threading

### Thread Safety
- Protect shared data with mutexes
- Use thread-safe functions
- Avoid global state when possible

```c
pthread_mutex_lock(&mutex);
// Access shared data
pthread_mutex_unlock(&mutex);
```

### Thread Functions
Use descriptive thread function names:

```c
void *worker_thread(void *arg)
{
    // Thread logic
    return NULL;
}
```

## Database Operations

### SQL Queries
- Use parameterized queries to prevent SQL injection
- Format SQL queries for readability
- Validate user input before database operations

```c
const char *query = "SELECT * FROM users WHERE id = $1";
const char *params[1] = {user_id};
PGresult *result = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);
```

### Connection Management
- Always check connection status
- Handle connection failures gracefully
- Implement connection pooling for performance

## Testing Standards

### Test Naming
Test files should be named `test_<module>.c`:

```c
test_user_crud.c
test_database_connection.c
test_http_server.c
```

### Test Structure
Follow AAA pattern (Arrange, Act, Assert):

```c
void test_user_create(void)
{
    // Arrange
    user_t *user = user_create_test_data();
    
    // Act
    bool result = user_create(user);
    
    // Assert
    assert(result == true);
    assert(user->id > 0);
    
    // Cleanup
    user_cleanup(user);
}
```

## Build Configuration

### Compiler Flags
Use consistent compiler flags:

```makefile
CFLAGS = -Wall -Wextra -std=c11 -pthread
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG
```

### Dependencies
- Document all required libraries
- Use compatible library versions
- Handle missing dependencies gracefully

## Security Best Practices

### Input Validation
- Validate all user input
- Sanitize data before processing
- Use safe string functions

### Buffer Overflow Prevention
- Use bounded string functions: `strncpy` instead of `strcpy`
- Check buffer sizes before operations
- Use dynamic allocation for large data

### Cryptography
- Use standard cryptographic libraries
- Never implement custom cryptography
- Keep cryptographic keys secure

## Code Review Checklist

- [ ] Code follows naming conventions
- [ ] Functions are properly documented
- [ ] Error handling is comprehensive
- [ ] Memory management is correct
- [ ] Thread safety is ensured
- [ ] Security best practices are followed
- [ ] Code is properly formatted
- [ ] Comments are clear and helpful
- [ ] Tests are included
- [ ] Build warnings are resolved

## Version Control

### Commit Messages
Use descriptive commit messages:

```
feat: Add user authentication functionality
fix: Resolve memory leak in database connection
docs: Update API documentation
refactor: Improve error handling in HTTP server
```

### Branch Naming
Use descriptive branch names:

```
feature/user-authentication
bugfix/database-connection-leak
hotfix/security-patch
refactor/code-cleanup
```

## Continuous Integration

### Build Process
- Ensure code compiles without warnings
- Run static analysis tools
- Execute automated tests
- Check code style compliance

### Quality Gates
- Code coverage threshold: 80%
- Zero critical security vulnerabilities
- All tests must pass
- Code review approval required

## Performance Guidelines

### Optimization
- Profile before optimizing
- Focus on hot paths
- Consider algorithm complexity
- Use appropriate data structures

### Resource Management
- Limit memory usage
- Manage file descriptors properly
- Implement connection pooling
- Use caching strategically

## Legacy Code Maintenance

### Refactoring
- Improve structure without changing behavior
- Add tests before refactoring
- Update documentation
- Maintain backward compatibility

### Deprecation
- Mark deprecated functions clearly
- Provide migration path
- Update documentation
- Remove in future versions

## Compliance and Standards

### Industry Standards
- Follow ISO C standards where applicable
- Adhere to security best practices
- Consider accessibility requirements
- Respect privacy regulations

### Licensing
- Include license headers
- Document third-party dependencies
- Respect license terms
- Provide attribution where required