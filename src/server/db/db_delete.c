/* 
    File: db_delete.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_def.h"
#include "db_core_funcs.h"
#include "db_struct.h"
#include "db_hash_function.h"
#include "db_error.h"

/* 
    Function: db_delete
    Params: 
    Description:
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

db_boolean_t db_delete(DATABASE db, U8bit * table_name, db_condition_t * cond)
{
    DB_RESET_ERROR();
    /* Init variable */
    int i, j;
    int row_ids[DB_MAX_ROWS_IN_BUCKET + 1];
    int num_row = 0;
    
    // Get table_id
    int table_index = db_get_index_table_from_table_name(db, table_name);
    if(table_index == -1)
    {
        return DB_FAILURE;
    }

    db_table_info * table = &(db->tables[table_index]);

    int table_num_row = table->num_rows -1;
    /* Get condition */
    if(cond != DB_NULL)
    {
        /* Get rows on condition */
         int num_condition = cond->num_cond;
        DB_TRACE(("DB:db_delete: num_condition = %d\n", num_condition));
        for (i = 0; i < num_condition; i++)
        {
            int cond_field_index = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, cond->field_conditions[i]);
            DB_TRACE(("DB:db_delete:condition = %d\n", i));
            switch (cond->operator_conditions[i])
            {
                case DB_COND_EQUAL:
                    if(db_search_row_with_equal_condition(db->fd, table->position_table, cond_field_index, cond->val_conditions[i], row_ids, &num_row) == DB_FAILURE)
                    {
                        return DB_FAILURE;
                    }
                    break;
                default:
                    DB_TRACE(("DB:db_delete: CANNOT detect condition's operator, %d\n", cond->operator_conditions[i]));
                    DB_SET_ERROR(DB_OUT_OF_BOUNDS);
                    return DB_FAILURE;
            }

        }
    }else
    {
        DB_TRACE(("DB:db_delete: Wrong parameter\n"));
        DB_SET_ERROR(DB_ERROR_WRONG_PARAM);
        return DB_FAILURE;
    }
    /* Delete value */
    for(i = 0; i < num_row; i++)
    {
        /* Remove in fields bucket */
        for(j = 1; j <= DB_MAX_FIELDS_IN_TABLE; j++)
        {
            int value_index = db_get_value_index_from_rows_bucket(db->fd, table->position_table, row_ids[i], j);
            if(value_index == -1)
            {
                if(db_error_no != DB_NO_ERROR)
                {
                    return DB_FAILURE;
                }

            }else
            {
                if(db_remove_value_in_field_bucket(db->fd, table->position_table, j, value_index) == DB_FAILURE)
                {
                    return DB_FAILURE;
                }
                DB_TRACE(("DB:db_delete: Delete value in field_index = %d, value_index = %d\n", j, value_index));
            }
        }
        if(db_remove_row_in_rows_bucket(db->fd, table->position_table, row_ids[i]) == DB_FAILURE)
        {
            return DB_FAILURE;
        }
        DB_TRACE(("DB:db_delete: delete row_id = %d\n", row_ids[i]));
        // Decrease num row in table
        table->num_rows = table_num_row;
        if(db_set_num_row_in_table(db->fd, table->position_table, table_num_row) == DB_FAILURE)
        {
            return DB_FAILURE;
        }
        
    }

    return DB_SUCCESS;
}