#include <stdio.h>
#include "../server/db/db_api.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include "../server/db/db_error.h"
#include <string.h>
#include <stdlib.h>
int main()
{
    DATABASE db = db_open("test_database", "../data/", DB_OPEN);

    if (db != NULL)
    {
        printf("Success!\n");
    }
    else
    {
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Fail!\n");
        return 0;
    }

    // delete row in database
    {
        
        db_condition_t cond;
        cond.num_cond = 1;
        cond.operator_conditions[0] = DB_COND_EQUAL;
        cond.field_conditions[0] = "hung_id";
        cond.val_conditions[0] = "hung1";
        if(db_delete(db, "test_table", &cond) == DB_SUCCESS)
        {
            printf("Delete success!\n");
        }else{
            printf("Delete fail!\n");
            printf("Error: %s\n", db_error_str[db_error_no]);
        }
        
    }

    db_close(db);
    return 1;
}