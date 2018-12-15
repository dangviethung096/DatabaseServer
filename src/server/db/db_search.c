/* 
    File: db_search.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_struct.h"
#include "db_def.h"
#include "db_core_funcs.h"
#include "db_global.h"
#include "db_alloc.h"
#include "db_error.h"
/* 
    Function: db_search
    Params: 
    Description: search by using field_name with some condition or not
                 if field_name == NULL then search all field in table
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_search(DATABASE db, U8bit * table_name, U8bit * field_name[], int num_field, db_condition_t * cond, db_search_ret_t ** ret)
{
    // int count_value = 0;
    int field_indexes[num_field];
    off_t field_pos[num_field];
    int i, j;
    int table_index = db_get_index_table_from_table_name(db, table_name);
    DB_TRACE(("DB:db_search: get table index = %d\n", table_index));
    db_table_info *table = &(db->tables[table_index]);
    int num_row = table->num_rows;

    /* Search field_index */
    for(i = 0; i < num_field; i++)
    {
        field_indexes[i] = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, field_name[i]);
        if (field_indexes[i] == -1)
        {
            return DB_FAILURE;
        }

        // Get field pos
        field_pos[i] = db_point_to_fields_bucket_by_index(db->fd, table->position_table, field_indexes[i]);
        if(field_pos[i] == -1)
        {
            return DB_FAILURE;
        }
    }

    /* Choose value from condition */



    /* Search all value */
    *ret = (db_search_ret_t *) db_alloc(num_row * sizeof(db_search_ret_t));
    off_t val_field_pos[num_field];
    int row_index;
    for (i = 0, row_index = 0; i < table->num_rows; row_index++)
    {
        DB_TRACE(("DB:db_search: row_index = %d\n", row_index));
        if (db_is_row_in_rows_bucket_used(db->fd, table->position_table, row_index) == DB_TRUE)
        {
            (*ret)[i].field_names = (U8bit **) db_alloc(num_field * sizeof(U8bit *) );
            (*ret)[i].values = (U8bit **)db_alloc(num_field * sizeof(U8bit *));
            for(j = 0; j < num_field; j++)
            {
                // Alloc memory
                (*ret)[i].field_names[j] = field_name[j];
                (*ret)[i].values[j] = (U8bit *)db_alloc(DB_MAX_SIZE_IN_VALUE);

                val_field_pos[j] = db_get_value_pos_from_rows_bucket_by_field_index(db->fd, table->position_table, row_index, field_indexes[j] );
                if(val_field_pos[j] == -1)
                {
                    if(db_error_no != DB_NO_ERROR)
                    {
                        return DB_FAILURE;
                    }
                    memcpy((*ret)[i].values[j], DB_STR_NULL, db_length_str(DB_STR_NULL));
                    continue;
                }
                
                // Get value from db
                db_value_field_t value;
                if(db_get_value_in_fields_bucket_by_value_pos(db->fd, val_field_pos[j], &value) == DB_FAILURE )
                {
                    return DB_FAILURE;
                }

                // Copy value from return to ret value.
                memcpy((*ret)[i].values[j], value.value, value.size);
                (*ret)[i].values[j][value.size] = '\0';

                DB_TRACE(("DB:db_search: field_name = %s, value = %s at i = %d, j = %d\n", (*ret)[i].field_names[j], (*ret)[i].values[j], i, j));
            }
            // Plus one row
            DB_TRACE(("DB:db_search: num_row = %d\n", num_row));
            (*ret)[i].num_ret = num_row;
            i++;
        }

        if(row_index > DB_MAX_ROWS_IN_BUCKET)
        {
            DB_SET_ERROR(DB_OUT_OF_BOUNDS);
            return DB_FAILURE;
        }
        
    }

    return DB_SUCCESS;
}
