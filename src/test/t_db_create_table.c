#include <stdio.h>
#include "../server/db/db_api.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include <string.h>
#include <stdlib.h>
#include "t_db_const.h"

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
        int i;
        char * field_name[10] = 
            {"id", "name", "age", "class", "school", "status", "favorite", "detail", "home_land", "address"};
        db_field_t field[10];
        for(i = 0; i < 10; i++)
        {
            memcpy(field[i].field_name, field_name[i], strlen(field_name[i]));
            field[i].field_name[strlen(field_name[i])] = '\0';
        }
        
        
        if(db_create_table(db, "student", field, 10) != NULL)
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