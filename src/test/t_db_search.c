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

    U8bit * field_name[] = {"hung_id", "thanh_id"};
    int num_field = 2;
    db_condition_t cond;
    db_search_ret_t * ret;
    if(db_search(db, "test_table", field_name, num_field, DB_NULL, &ret) == DB_SUCCESS)
    {
        printf("Search Success with number return %d\n", ret[0].num_ret);
        int i, j;
        for(i = 0; i < ret[0].num_ret; i++)
        {
            for(j = 0; j < num_field; j++)
            {
                printf("field_name = %s, value = %s with i = %d, j = %d\n", ret[i].field_names[j], ret[i].values[j], i, j);
                free(ret[i].values[i]);
                free(ret[i].field_names[i]);
            }
            free(ret[i].values);
            free(ret[i].field_names);
        }
        free(ret);
    }else
    {
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Search Fail!\n");
        return 0;
    }

    db_close(db);
    return 1;
}