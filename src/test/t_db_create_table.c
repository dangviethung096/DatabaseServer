#include <stdio.h>
#include "../server/db/db_api.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include <string.h>
#include <stdlib.h>
int main()
{
    DATABASE db = db_open("test_database", "../data/", DB_OPEN);
    
    if(db != NULL)
    {
        printf("Success!\n");
    }else{
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Fail!\n");
        return 0;
    }
    // Create a table
    {
        
        db_field_t field[3];
        memcpy(field[0].field_name, "id", strlen("id"));
        field[0].field_name[strlen("id")] = '\0';
        memcpy(field[1].field_name, "name", strlen("name"));
        field[1].field_name[strlen("name")] = '\0';
        memcpy(field[2].field_name, "age", strlen("age"));
        field[2].field_name[strlen("age")] = '\0';
        
        if(db_create_table(db, "student", field, 3) != NULL)
        {
            printf("Success\n");
        }else
        {
            DB_TRACE(("Error: %s\n", db_error_str[db_error_no]));
            printf("Fail\n");
        }
    }
    
    db_close(db);
    return 1;
}