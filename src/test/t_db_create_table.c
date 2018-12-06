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
        
        db_field_t field[1];
        memcpy(field[0].field_name, "hung_id", strlen("hung_id"));
        field[0].field_name[strlen("hung_id")] = '\0';
        
        if(db_create_table(db, "test_table", field, 1) != NULL)
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