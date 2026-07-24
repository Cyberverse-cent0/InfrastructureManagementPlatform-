#include "../global_lib.h"


// load workind from rest of programm 





static bool check_if_database_exist(char* working_dir){
    int retn = system("ls %s", working_dir);
    if(retn == 0) {
        retn = system("ls %s/%s", DATA_DIR);
        if(retn == 0) {
            print_info("Database file exists in %s", DATA_DIR);
            return true;
        } else {
            print_error("Database file does not exist");
        }
    } else {
        print_error("Database directory does not exist");
    }
    return false;
}


database_t* database_init(const char* connection_string){
    database_t* db = calloc(1, sizeof(database_t));
    if(db == NULL){
        return NULL;
    }
    
    strncpy(db->connection_string, connection_string, sizeof(db->connection_string) - 1);
    db->connection = PQconnectdb(connection_string);

    if (PQstatus(db->connection) != CONNECTION_OK) {
        print_error("Connection to database failed: %s", PQerrorMessage(db->connection));
        PQfinish(db->connection);
        free(db);
        return NULL;
    }
    
    db->is_connected = true;
    return db;
}

void database_disconnect(database_t *database)
{
    if (database == NULL)
        return;

    if (database->connection != NULL)
        PQfinish(database->connection);

    free(database);
}
