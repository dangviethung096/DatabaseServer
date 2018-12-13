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

    db_field_t field[2];
    memcpy(field[0].field_name, "hung_id", strlen("hung_id") + 1);
    memcpy(field[1].field_name, "thanh_id", strlen("thanh_id") + 1);

    if(db_create_table(db, "test_table", field, 2) == DB_NULL)
    {
        printf("Error: %s\n", db_error_str[db_error_no]);
        printf("Fail!\n");
    }

    printf("Create table success!\n");
    U8bit * field_name[] = {"hung_id","thanh_id"};
    U8bit * value[] = {"say_hi", "say_yes"};
    
    if(db_insert(db, "test_table", field_name, value, 2) == DB_SUCCESS)
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