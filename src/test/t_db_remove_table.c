#include <stdio.h>
#include "../server/db/db_def.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include "../server/db/db_api.h"
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

    if(db_remove_table(db, "test_table") == DB_SUCCESS)
    {
        printf("Remove success\n");
    }else{
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Remove fail\n");
    }
    
    db_close(db);
    return 1;
}