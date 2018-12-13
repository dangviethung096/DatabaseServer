#include <stdio.h>
#include "../server/db/db_api.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
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

    // Insert field to database
    {
        U8bit * field_name[] = {"hung_id"};
        U8bit * value[] = {"say_hi"};
        if(db_insert(db, "test_table", field_name, value, 1) == DB_SUCCESS)
        {
            printf("Success!\n");
        }else
        {
            printf("Fail\n");
            DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        }
    }

    db_close(db);
    return 1;
}