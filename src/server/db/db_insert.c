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


db_boolean_t db_insert(DATABASE db, U8bit * table_name, U8bit * field_name, U8bit * value)
{
    DB_RESET_ERROR();
    //Search table_id
    int table_index = db_get_index_table_from_table_name(db, table_name);
    db_table_info * table = &(db->tables[table_index]);
    
    //Search position of field_name
    int field_index = db_get_index_field_in_fields_bucket_by_field_name(db->fd, table, field_name);
    DB_TRACE(("DB:db_insert: get field_index = %d!\n", field_index));
    off_t field_pos = db_point_to_fields_bucket_by_index(db->fd, table->position_table, field_index);
    DB_TRACE(("DB:db_insert: get field_pos = %ld!\n", field_pos));
    //Search position of value
    int num_hash = 0, value_index = 0;
    db_first_hash_ret_t hval = 0;
    db_hash_function(value, &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &value_index);
    
    if(db_is_value_in_field_bucket_used(db->fd, field_pos, value_index) == DB_TRUE)
    {
        int first_index = value_index;
        do
        {
            db_hash_function(value, &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &value_index);
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
    value_in_field.size = db_length_str(value);
    value_in_field.row_id = db->tables[table_index].num_rows;
    db->tables[table_index].num_rows++;
    memcpy(value_in_field.value, value, value_in_field.size);
    if(db_set_value_in_fields_bucket(db->fd, field_pos, value_index, value_in_field) == DB_FAILURE)
    {
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}
