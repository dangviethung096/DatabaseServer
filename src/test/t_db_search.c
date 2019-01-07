#include <stdio.h>
#include "../server/db/db_def.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include "../server/db/db_api.h"
#include "../server/db/db_alloc.h"
#include "t_db_const.h"
#include <time.h>
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
    
    int i;
    FILE * file = fopen("../data/data.txt", "rb");

    U8bit * field_name[NUM_COLS];
    for(i = 0; i < NUM_COLS; i++)
    {
        field_name[i] = malloc(MAX_VALUE * DB_U_8_BIT_SIZE);
        fscanf(file, "%s ", field_name[i]);
    }
    fclose(file);

    int num_field = 0;
    db_condition_t cond;
    cond.num_cond = 0;
    cond.operator_conditions[0] = DB_COND_EQUAL;
    cond.field_conditions[0] = "hung_id";
    cond.val_conditions[0] = "hung1";
    cond.num_cond = 0;
    db_search_ret_t * ret;
    if(db_search(db, "student", field_name, num_field, &cond, &ret) == DB_SUCCESS)
    {
        printf("Search Success with number return %d\n", ret[0].num_ret);
        int i, j;
        for(i = 0; i < ret[0].num_ret; i++)
        {
            for(j = 0; j < ret[0].num_field; j++)
            {
                printf("field_name = %s, value = %s with i = %d, j = %d\n", ret[i].field_names[j], ret[i].values[j], i, j);
                // printf("Free value[%d]\n", i);
            }
            
        }
        
        for(i = 0; i < ret[0].num_ret; i++)
        {
            for(j = 0; j < ret[0].num_field;j ++)
            {
                free(ret[i].field_names[j]);
                free(ret[i].values[j]);
            }
            free(ret[i].values);
            free(ret[i].field_names);
        }
        free(ret);
    }else
    {
        printf("Error_no: %s\n", db_error_str[db_error_no]);
        printf("Search Fail!\n");
        return 0;
    }

    for(i = 0; i < NUM_COLS; i++)
    {
        free(field_name[i]);
    }
    
    db_close(db);
    return 1;
}