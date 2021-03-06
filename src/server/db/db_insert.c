/* 
    File: db_insert.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_def.h"
#include "db_struct.h"
#include "db_core_funcs.h"
#include "db_hash_function.h"
#include "db_global.h"
#include "db_error.h"


db_boolean_t db_insert(DATABASE db, U8bit * table_name, U8bit * field_name[], U8bit * value[], int num_value)
{
    DB_RESET_ERROR();
    // Declare value
    int row_id = -1;
    int num_row;
    int i;
    db_table_info * table;
    //Search table_id
    int table_index = db_get_index_table_from_table_name(db, table_name);
    if(table_index == -1)
    {
        return DB_FAILURE;
    }

    table = &(db->tables[table_index]);
    num_row = table->num_rows + 1;

    /* Find row_id. row_id = index_row in rows_bucket */
    for(i = 0; i <= DB_MAX_ROWS_IN_BUCKET; i++)
    {
        if(db_is_row_in_rows_bucket_used(db->fd, table->position_table, i) == DB_FALSE)
        {
            row_id = i;
            break;
        }
    }
    DB_TRACE(("DB:db_insert: row_id = %d!\n", row_id));
    if(row_id == -1)
    {
        DB_SET_ERROR(DB_ERROR_FULL);
        return DB_FAILURE;
    }


    for(i = 0; i < num_value; i++)
    {
        /* Search position of field_name */
        int field_index = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, field_name[i]);
        if(field_index < 1 || field_index > DB_MAX_FIELDS_IN_TABLE)
        {
            return DB_FAILURE;
        }
        DB_TRACE(("DB:db_insert: get field_index = %d!\n", field_index));
        off_t field_pos = db_point_to_fields_bucket_by_index(db->fd, table->position_table, field_index);
        DB_TRACE(("DB:db_insert: get field_pos = %ld!\n", field_pos));
        //Search position of value
        int num_hash = 0, value_index = 0;
        db_first_hash_ret_t hval = 0;
        db_hash_function(value[i] , &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &value_index);
        
        if(db_is_value_in_field_bucket_used(db->fd, field_pos, value_index) == DB_TRUE)
        {
            if(db_error_no != DB_NO_ERROR)
            {
                return DB_FAILURE;
            }
            int first_index = value_index;
            do
            {
                db_hash_function(value[i] , &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &value_index);
                if(db_is_value_in_field_bucket_used(db->fd, field_pos, value_index) == DB_FALSE)
                {
                    if(db_error_no != DB_NO_ERROR)
                    {
                        return DB_FAILURE;
                    }
                    break;
                }
            }while(first_index != value_index);

        }
        
        
        //Insert value
        db_value_field_t value_in_field;
        value_in_field.flag = DB_FLAG_USED;
        value_in_field.size = db_length_str(value[i]);
        value_in_field.row_id = row_id;
        
        
        memcpy(value_in_field.value, value[i], value_in_field.size);
        DB_TRACE(("DB:db_insert:insert data: flag = %d, size = %d, row_id = %d, value = %s!\n", 
                                            (int)value_in_field.flag, value_in_field.size, value_in_field.row_id, value_in_field.value));
        if(db_set_value_in_fields_bucket(db->fd, field_pos, value_index, value_in_field) == DB_FAILURE)
        {
            return DB_FAILURE;
        }
        
        /* Insert pos of this value in fields bucket to rows bucket */
        DB_TRACE(("DB:db_insert: insert value_pos of value_index (%d)\n", field_index)); 
        off_t val_pos = db_point_to_fields_bucket_by_value_index(db->fd, field_pos, value_index);
        
        if(val_pos == -1)
        {
            return DB_FAILURE;
        }

        if(db_set_value_pos_in_rows_bucket_by_field_index(db->fd, table->position_table, row_id, field_index, val_pos) == DB_FAILURE)
        {
            return DB_FAILURE;
        }
    }
    
    

    
    /* Insert to row */
    DB_TRACE(("DB:db_insert: mark flag USED to rows %d\n", row_id)); 
    db_flag_t flag = DB_FLAG_USED;
    if(db_set_flag_in_rows_bucket(db->fd, table->position_table, row_id, flag) == DB_FAILURE)
    {
        return DB_FAILURE;
    }

    /* Increate num_row in db */
    DB_TRACE(("DB:db_insert: increase num_row = %d\n", num_row));
    if(db_set_num_row_in_table(db->fd, table->position_table, num_row) == DB_FAILURE)
    {
        return DB_FAILURE;
    }

    db->tables[table_index].num_rows = num_row;
    return DB_SUCCESS;
}
