/* 
    File: db_create_table.h
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"
#include "db_def.h"
#include "db_core_funcs.h"
#include "db_error.h"
#include "db_hash_function.h"
#include "db_global.h"
#include <fcntl.h>
#include <sys/stat.h>

db_table_info * db_create_table(DATABASE db, char *table_name, db_field * fields, int num_field)
{
    // Declare variable
    off_t pos_table = db->last_position;
    off_t new_last_pos;
    ssize_t io_ret_val;
    int num_table = db->num_table + 1;
    int index_table = db->num_table;
    int now_id_table;
    off_t fields_bucket_pos;

    // Reset db_error_no
    DB_RESET_ERROR();

    // Check validate table
    if(db->num_table == DB_MAX_TABLE_IN_DATABASE)
    {
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return DB_NULL;
    }
    
    /* START: Add new block to db (block contains buckets, table info) */
        
    // Reset mem to 0 in db
    if(db_seek(db->fd, pos_table, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    int i;
    char val = 0x00;
    DB_TRACE(("DB:db_create_table:DB_SINGLE_TABLE_SIZE = %lu, DB_TABLE_DATA_SIZE = %lu\n", DB_SINGLE_TABLE_SIZE, DB_TABLE_DATA_SIZE));
    for(i = 0; i < DB_TABLE_DATA_SIZE; i++)
    {
        io_ret_val = db_write(db->fd, &val, 1);
        if(io_ret_val == -1)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
    }

    // Set unused for rows bucket
    if(db_seek(db->fd, pos_table + DB_POS_ROWS_BUCKET_IN_TABLE, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    off_t rows_bucket_pos = pos_table + DB_POS_ROWS_BUCKET_IN_TABLE;
    DB_TRACE(("DB:db_create_table:Set unused in rows bucket at %ld\n", rows_bucket_pos));
    for(i = 0; i < DB_MAX_ROWS_IN_BUCKET; i++)
    {
        db_flag_t flag = DB_FLAG_USED;
        if(db_set_flag_in_rows_bucket(db->fd, rows_bucket_pos, i, flag) == DB_FAILURE)
        {
            return DB_NULL;
        }
    }

    // Set unused for fields bucket
    fields_bucket_pos = pos_table + DB_POS_FIELDS_BUCKET_IN_TABLE;

    if(db_seek(db->fd, fields_bucket_pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    DB_TRACE(("DB:db_create_table:Set unused in fields bucket at %ld\n", pos_table + DB_POS_FIELDS_BUCKET_IN_TABLE));
    for(i = 0; i < DB_MAX_FIELDS_IN_TABLE; i++)
    {   
        db_flag_t flag = DB_FLAG_NOT_USED;
        if(db_set_flag_in_fields_bucket(db->fd, fields_bucket_pos, i, flag) == DB_FAILURE)
        {
            return DB_NULL;
        }
    }


    // Seek to new last position

    new_last_pos = db_seek(db->fd, pos_table + DB_SINGLE_TABLE_SIZE, DB_BEGIN_FD);
    DB_TRACE(("DB:db_create_table:new_last_pos = %ld\n", new_last_pos));
    if(new_last_pos == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    
    char end = '\0';
    io_ret_val = db_write(db->fd, &end, 1);


    /* END: Add new block to db (block contains buckets, table info) */

    //Back to table_position
    if(db_seek(db->fd, pos_table, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    
    // Write id table
    if(db_seek(db->fd, pos_table +  DB_POS_ID_TABLE, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }

    if(num_table == 1)
    {
        now_id_table = 1;
    }else
    {
        now_id_table = db->tables[index_table].id_table + 1;
    }

    DB_TRACE(("DB:db_create_table:id_table = %d\n", now_id_table));
    io_ret_val = db_write(db->fd, &now_id_table, DB_INT_SIZE);
    if(io_ret_val != DB_INT_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    // Write table name
    if(db_seek(db->fd, pos_table + DB_POS_NAME_TABLE, DB_CURRENT_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }

    if(db_strlen(table_name) > DB_MAX_LENGTH_TABLE_NAME)
    {
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return DB_NULL;
    }

    DB_TRACE(("DB:db_create_table:table_name = %s\n", table_name));
    io_ret_val = db_write(db->fd, table_name, db_strlen(table_name));
    if(io_ret_val == -1)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    
    // Write number fields
    off_t pos_num_field = pos_table + DB_POS_NUM_FIELD_IN_TABLE;
    if (db_seek(db->fd, pos_num_field, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }

    db->tables[index_table].num_fields = (U8bit) num_field;
    DB_TRACE(("DB:db_create_table:num_fields = %d\n", (int) db->tables[index_table].num_fields));
    io_ret_val = db_write(db->fd, &(db->tables[index_table].num_fields), DB_U_8_BIT_SIZE);
    if(io_ret_val != DB_U_8_BIT_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    // Write fields in table
    off_t pos_first_field_in_table = pos_table + DB_POS_FIELDS_IN_TABLE;

    // Write each field in table to database
    int index;
    for(index = 0; index < num_field; index++)
    {
        off_t field_pos = db_seek(db->fd, pos_first_field_in_table + index * DB_FIELD_IN_TABLE_SIZE , DB_BEGIN_FD);
        // Write flag
        db_flag_t flag = DB_FLAG_USED;
        io_ret_val = db_write(db->fd, &flag, 1);
        if(io_ret_val != 1)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        
        int num_write = db_strlen(fields[index].field_name) + 1;
        DB_TRACE(("DB:db_create_table:field_name = %s\n", fields[index].field_name));
        io_ret_val = db_write(db->fd, fields[index].field_name, num_write);
        if(io_ret_val != num_write)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }

        // Hashing to find index fd
        db_key_t key;
        key.val = fields[index].field_name;
        key.size = db_strlen(key.val);
        DB_TRACE(("DB:db_create_table:hashing index with key.val = %s, key.size = %d\n", key.val, key.size));
        db_first_hash_ret_t hval = db_first_hash(key);
        db_index_t first_idx = hval % DB_MAX_FIELDS_IN_TABLE;
        DB_TRACE(("DB:db_create_table:first_idx = %d\n", first_idx));

        if(db_is_field_in_fields_bucket_used(db->fd, fields_bucket_pos, first_idx) == DB_FALSE)
        {
            if(db_error_no != DB_NO_ERROR)
            {
                return DB_NULL;
            }
            // Add new field to here
            fields[index].index = first_idx;

        }else
        {
            int hval2 = 1 + hval % (DB_MAX_FIELDS_IN_TABLE);
            db_second_hash_ret_t second_index = first_idx;
            do
            {
                second_index = db_second_hash(hval2, DB_MAX_FIELDS_IN_TABLE, second_index);
                DB_TRACE(("DB:db_create_table:index in second hash = %d\n", second_index));
                if(db_is_field_in_fields_bucket_used(db->fd, fields_bucket_pos, second_index) == DB_FALSE)
                {
                    if(db_error_no != DB_NO_ERROR)
                    {
                        return DB_NULL;
                    }
                    // Add new field to here
                    fields[index].index = second_index;
                    break;
                }
            }while(second_index != first_idx);
        }

        // Seek to index fd
        
        if(db_seek(db->fd, field_pos + DB_POS_INDEX_IN_FIELD_INFO, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        DB_TRACE(("DB:db_create_table:write index of field\n"));
        io_ret_val = db_write(db->fd, &(fields[index].index), DB_INDEX_T_SIZE);
        if(io_ret_val != DB_INDEX_T_SIZE)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        /* Write in fields bucket */
        // Set field is used in fields bucket
        flag = DB_FLAG_USED;
        if(db_set_flag_in_fields_bucket(db->fd, fields_bucket_pos, fields[index].index, flag) == DB_FAILURE)
        {
            return DB_NULL;
        }
    }

    /* Write new last position in db */
    
    off_t pos = DB_POS_LAST_POSITION;
    if(db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    
    
    db->last_position = new_last_pos;
    DB_TRACE(("DB:db_create_table:write last position: %ld\n", new_last_pos));
    io_ret_val = db_write(db->fd, &db->last_position, DB_OFF_T_SIZE);
    if(io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    /* Increase num_table */
    db->num_table = num_table;

    /* Write number table info to database info */
    
    pos = DB_POS_NUMBER_TABLE;
    if(db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }
    
    // Increase number table index
    DB_TRACE(("DB:db_create_table:write number table: %d\n", num_table));
    io_ret_val = db_write(db->fd, &num_table, DB_OFF_T_SIZE);
    if(io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    /* Write position of table to database info */
    pos = db_point_to_index_table_info_in_db(db->fd, index_table);
    if(pos == -1)
    {
        return DB_NULL;
    }

    DB_TRACE(("DB:db_create_table:write position of table in database info: pos = %ld, position of table = %ld\n", pos, pos_table));
    io_ret_val = db_write(db->fd, &pos_table, DB_OFF_T_SIZE);
    if(io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    return &(db->tables[index_table]);
}