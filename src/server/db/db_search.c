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
#include "db_hash_function.h"

/* 
    Function: db_search_row_with_equal_condition
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_search_row_with_equal_condition(int fd, off_t table_pos, int field_index, U8bit * value, int * row_ids, int * num_row)
{
    DB_RESET_ERROR();
    /* Init */
    db_key_t key;
    key.val = value;
    key.size = db_length_str(value);
    
    db_first_hash_ret_t hval =  db_first_hash(key);

    int first_index = hval % DB_MAX_ROWS_IN_BUCKET + 1;
    int index = first_index;
    int hval2 = 1 + hval % (DB_MAX_ROWS_IN_BUCKET - 2);
    *num_row = 0;
    do
    {
        if (db_is_value_in_field_bucket_used_with_field_index(fd, table_pos, field_index, index) == DB_FALSE)
        {
            if(db_error_no != DB_NO_ERROR)
            {
                return DB_FAILURE;
            }
            break;
        }
        db_value_field_t ret_value;
        if(db_get_value_in_fields_bucket_with_field_index(fd, table_pos, field_index, index, &ret_value) == DB_FAILURE)
        {
            return DB_FAILURE;
        }

        row_ids[(*num_row)++] =  ret_value.row_id;
        index = db_second_hash(hval2, DB_MAX_ROWS_IN_BUCKET, index);

    }while(index != first_index);
    
    return DB_SUCCESS;
}
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
    /* Init variable */
    int field_indexes[DB_MAX_FIELDS_IN_TABLE];
    off_t field_pos[DB_MAX_FIELDS_IN_TABLE];
    int i, j;
    int row_index;
    int table_index = db_get_index_table_from_table_name(db, table_name);
    DB_TRACE(("DB:db_search: get table index = %d\n", table_index));

    db_table_info * table = &(db->tables[table_index]);
    int num_row = table->num_rows;
    int num_condition = cond->num_cond;

    int row_ids[DB_MAX_ROWS_IN_BUCKET];
    int num_ret_row = 0;
    /* Init row id */
    for(i = 0; i < num_row; i++)
    {
        row_ids[i] = -1;    
    }
    /* Search field_index */
    if(cond != DB_NULL)
    {
        /* Choose value from condition */
        for (i = 0; i < num_condition; i++)
        {
            int cond_field_index = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, cond->field_conditions[i]);
            switch (cond->operator_conditions[i])
            {
                case DB_COND_EQUAL:
                    if(db_search_row_with_equal_condition(db->fd, table->position_table, cond_field_index, cond->val_conditions[i], row_ids, &num_ret_row) == DB_FAILURE)
                    {
                        return DB_FAILURE;
                    }
                    break;
                default:
                    DB_TRACE(("DB:db_search: CANNOT detect condition's operator, %d\n", cond->operator_conditions[i]));
                    DB_SET_ERROR(DB_OUT_OF_BOUNDS);
                    return DB_FAILURE;
            }
        }
    }else
    {
        /* Search all row id because no condition */
        for(row_index = 0, i = 0; i < num_row && row_index <= DB_MAX_ROWS_IN_BUCKET; row_index++)
        {
            if(db_is_row_in_rows_bucket_used(db->fd, table->position_table, row_index) == DB_TRUE)
            {
                row_ids[i] = row_index;
                i++;
            }
        }

        if(i < num_row)
        {
            DB_SET_ERROR(DB_ERROR_NOT_ENOUGH);
            return DB_FAILURE;
        }

    }

    if(num_field > 0)
    {
        for (i = 0; i < num_field; i++)
        {
            field_indexes[i] = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, field_name[i]);
            if (field_indexes[i] == -1)
            {
                return DB_FAILURE;
            }

            // Get field pos
            field_pos[i] = db_point_to_fields_bucket_by_index(db->fd, table->position_table, field_indexes[i]);
            if (field_pos[i] == -1)
            {
                return DB_FAILURE;
            }
        }
    }else if (num_field == 0)
    {
        /* Search all field. get all field index */
        num_field = table->num_fields;
        for(i = 0; i < num_field; i++)
        {
            field_indexes[i] = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table,(U8bit *) table->fields[i].field_name);
            if (field_indexes[i] == -1)
            {
                return DB_FAILURE;
            }

            // Get field pos
            field_pos[i] = db_point_to_fields_bucket_by_index(db->fd, table->position_table, field_indexes[i]);
            if (field_pos[i] == -1)
            {
                return DB_FAILURE;
            }
        }
    }else
    {
        DB_TRACE(("DB:db_search: num_field = %d\n", num_field));
        return DB_FAILURE;
    }
    

    /* Search all value */
    *ret = (db_search_ret_t *) db_alloc(num_ret_row * sizeof(db_search_ret_t));
    off_t val_field_pos[num_field];
    
    for (i = 0; i < num_ret_row; i++)
    {
        DB_TRACE(("DB:db_search: row_index = %d\n", row_index));

        (*ret)[i].field_names = (U8bit **) db_alloc(num_field * sizeof(U8bit *) );
        (*ret)[i].values = (U8bit **)db_alloc(num_field * sizeof(U8bit *));
        for(j = 0; j < num_field; j++)
        {
            // Alloc memory
            (*ret)[i].field_names[j] = field_name[j];
            (*ret)[i].values[j] = (U8bit *)db_alloc(DB_MAX_SIZE_IN_VALUE);

            val_field_pos[j] = db_get_value_pos_from_rows_bucket_by_field_index(db->fd, table->position_table, row_ids[i], field_indexes[j] );
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
        (*ret)[i].num_ret = num_ret_row;
        
    }

    return DB_SUCCESS;
}
