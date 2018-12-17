#include <stdio.h>
#include "../server/db/db_def.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include "../server/db/db_api.h"
#include "../server/db/db_alloc.h"
int main()
{
    DATABASE db = db_open("test_database", "../data/", DB_OPEN);

    if (db != NULL)
    {
        printf("Open Success!\n");
    }
    else
    {
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Open Fail!\n");
        return 0;
    }

    U8bit * field_name[] = {"hung_id"};
    U8bit * update_value[] = {"say hello"};
    int num_field = 1;
    db_condition_t cond;
    cond.operator_conditions[0] = DB_COND_EQUAL;
    cond.field_conditions[0] = "hung_id";
    cond.val_conditions[0] = "say_hi";
    cond.num_cond = 1;
    
    if(db_update(db, "test_table", field_name, update_value, num_field, &cond) == DB_FAILURE)
    {
        printf("Error = %s\n", db_error_str[db_error_no]);
    }else
    {
        printf("Update success!\n");
    }

    db_close(db);
    return 1;
}