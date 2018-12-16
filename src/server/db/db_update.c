/* 
    File: db_update.c
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_def.h"
#include "db_core_funcs.h"
#include "db_struct.h"
#include "db_hash_function.h"
#include "db_error.h"


/* 
    Function: db_update
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_update(DATABASE db, U8bit * table_name, U8bit * field_names[], U8bit * update_values[], int num_field, db_condition_t * cond)
{
    DB_RESET_ERROR();
    /* Init variables */
    int i, j;
    int row_ids[DB_MAX_ROWS_IN_BUCKET + 1];
    int num_row = 0;

    // Get table_id
    int table_index = db_get_index_table_from_table_name(db, table_name);
    db_table_info * table = &(db->tables[table_index]);

    /* Get rows is udpate */
    if(cond != DB_NULL)
    {
        /* Get rows on condition */
         int num_condition = cond->num_cond;
        DB_TRACE(("DB:db_update: num_condition = %d\n", num_condition));
        for (i = 0; i < num_condition; i++)
        {
            int cond_field_index = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, cond->field_conditions[i]);
            DB_TRACE(("DB:db_update:condition = %d\n", i));
            switch (cond->operator_conditions[i])
            {
                case DB_COND_EQUAL:
                    if(db_search_row_with_equal_condition(db->fd, table->position_table, cond_field_index, cond->val_conditions[i], row_ids, &num_row) == DB_FAILURE)
                    {
                        return DB_FAILURE;
                    }
                    break;
                default:
                    DB_TRACE(("DB:db_update: CANNOT detect condition's operator, %d\n", cond->operator_conditions[i]));
                    DB_SET_ERROR(DB_OUT_OF_BOUNDS);
                    return DB_FAILURE;
            }

        }
    }else
    {
        /* Get all rows in table */
        for(i = 0; i <= DB_MAX_ROWS_IN_BUCKET; i++)
        {
            if(db_is_row_in_rows_bucket_used(db->fd, table->position_table, i) == DB_TRUE)
            {
                row_ids[num_row++] = i;
            }
        }
    }
    
    
    /* Update all row */
    int field_indexes[num_field];
    DB_TRACE(("DB:db_update: get field index\n"));
    // Get field_index
    for(i = 0; i < num_field; i++)
    {
        field_indexes[i] = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, field_names[i]);
        if(field_indexes[i] == -1)
        {
            return DB_FAILURE;
        }
    }


    for(i = 0; i < num_row; i++)
    {
        for(j = 0; j < num_field; j++)
        {
            
        }
    }


    return DB_SUCCESS;
}