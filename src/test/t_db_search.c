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

    char * field_name[] = {"hung_id"};
    int num_field = 1;
    db_condition_t cond;
    db_search_ret_t * ret;
    if(db_search(db, "test_table", field_name, num_field, NULL, ret) == DB_SUCCESS)
    {
        printf("Success\n");
        DB_TRACE(("ret_val"));
        int i, j;
        for(i = 0; i < ret[0].num_ret; i++)
        {
            for(j = 0; j < num_field; j++)
            {
                DB_TRACE(("field_name = %s, value = %s\n", ret[i].field_names[j], ret[i].values[j]));
            }
        }
    }else
    {
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Fail!\n");
        return 0;
    }

    db_close(db);
    return 1;
}