#include <stdio.h>
#include "../server/db/db_def.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include "../server/db/db_api.h"
#include <string.h>
int main()
{
    DATABASE db = db_open("test_database", "../data/", DB_CREATE);

    if (db != NULL)
    {
        printf("Create db success!\n");
    }
    else
    {
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Fail!\n");
        return 0;
    }

    db_field_t field;
    memcpy(field.field_name, "hung_id", strlen("hung_id") + 1);

    if(db_create_table(db, "test_table", &field, 1) == DB_NULL)
    {
        printf("Error: %s\n", db_error_str[db_error_no]);
        printf("Fail!\n");
    }

    printf("Create table success!\n");

    if(db_insert(db, "test_table", "hung_id", "say_hi") == DB_SUCCESS)
    {
        printf("Insert success!\n");
    }else
    {
        printf("Fail\n");
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
    }

    db_close(db);
    return 1;
}