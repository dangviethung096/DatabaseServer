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
#include "db_alloc.h"
#include <fcntl.h>
#include <sys/stat.h>

db_table_info * db_create_table(DATABASE db, char *table_name, db_field_t * fields, int num_field)
{
    // Declare variable
    off_t pos_table = db->last_position;
    off_t new_last_pos;
    ssize_t io_ret_val;
    int num_table = db->num_table + 1;
    int index_table = db->num_table;
    int now_id_table;

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

    int i, j;
    char val = 0x00;
    DB_TRACE(("DB:db_create_table:set Unused table info at %ld\n", pos_table));
    for(i = 0; i < DB_TABLE_INFO_DATA_SIZE; i++)
    {
        io_ret_val = db_write(db->fd, &val, 1);
        if(io_ret_val == -1)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
    }

    // Set unused for rows bucket
    DB_TRACE(("DB:db_create_table:Set unused in rows bucket at %ld\n", pos_table + DB_POS_ROWS_BUCKET_IN_TABLE));
    for(i = 0; i <= DB_MAX_ROWS_IN_BUCKET; i++)
    {
        for(j = 0; j <= DB_MAX_FIELDS_IN_TABLE; j++)
        {
            off_t val_pos = -1;
            if(db_set_value_pos_in_rows_bucket_by_field_index(db->fd, pos_table, i, j, val_pos) == DB_FAILURE)
            {
                return DB_NULL;
            }
        }

        db_flag_t flag = DB_FLAG_NOT_USED;
        if(db_set_flag_in_rows_bucket(db->fd, pos_table, i, flag) == DB_FAILURE)
        {
            return DB_NULL;
        }
    }

    // Set unused for fields bucket
    DB_TRACE(("DB:db_create_table:Set unused in fields bucket at %ld\n", pos_table + DB_POS_FIELDS_BUCKET_IN_TABLE));
    for(i = 0; i <= DB_MAX_FIELDS_IN_TABLE; i++)
    {   
        db_flag_t flag = DB_FLAG_NOT_USED;
        if(db_set_flag_in_fields_bucket(db->fd, pos_table, i, flag) == DB_FAILURE)
        {
            return DB_NULL;
        }
        
        for(j = 0; j <= DB_MAX_ROWS_IN_BUCKET; j++)
        {
            db_value_field_t value;
            value.flag = DB_FLAG_NOT_USED;
            value.row_id = -1;
            value.size = 0;
            memcpy(value.value, DB_STR_NULL, db_length_str(DB_STR_NULL));

            off_t field_pos = pos_table + DB_POS_FIELDS_BUCKET_IN_TABLE + i * DB_FIELD_SIZE_IN_FIELD_BUCKET;
            if(db_set_value_in_fields_bucket(db->fd, field_pos, j, value) == DB_FAILURE)
            {
                return DB_NULL;
            }
        }
    }

    // Seek to new last position
    // Find new last position
    new_last_pos = DB_POS_LAST_POSITION + DB_OFF_T_SIZE;
    int flag_last_position;
    do{
        flag_last_position = 0;
        off_t pos_table_i;

        if(new_last_pos == db->last_position)
        {
            DB_TRACE(("DB:db_create_table:new_last_pos = last_position = %ld\n", new_last_pos));
            flag_last_position = 1;
            new_last_pos += DB_SINGLE_TABLE_SIZE;
            continue;
        }

        for (i = 0; i < db->num_table; i++)
        {
            pos_table_i = db_get_position_index_table(db->fd, i);
            if(pos_table_i == -1)
            {
                return DB_NULL;
            }
            // Check is any table_pos equal last_pos
            if(new_last_pos == pos_table_i)
            {
                DB_TRACE(("DB:db_create_table:new_last_pos = table_pos_%d = %ld\n", i, new_last_pos));
                flag_last_position = 1;
                new_last_pos += DB_SINGLE_TABLE_SIZE;
                break;
            }
        }
    }while(flag_last_position);

    DB_TRACE(("DB:db_create_table:new_last_pos = %ld\n", new_last_pos));
    if (db_seek(db->fd, new_last_pos, DB_BEGIN_FD) == -1)
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
        now_id_table = db->tables[index_table - 1].id_table + 1;
    }

    DB_TRACE(("DB:db_create_table:id_table = %d in pos %ld\n", now_id_table, db_seek(db->fd, 0, DB_CURRENT_FD) ));
    io_ret_val = db_write(db->fd, &now_id_table, DB_INT_SIZE);
    if(io_ret_val != DB_INT_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    // Write table name
    if(db_seek(db->fd, pos_table + DB_POS_NAME_TABLE, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }

    if(db_length_str(table_name) >= DB_MAX_LENGTH_TABLE_NAME)
    {
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return DB_NULL;
    }
    
    DB_TRACE(("DB:db_create_table:table_name = %s in pos %ld\n", table_name, db_seek(db->fd, 0, DB_CURRENT_FD) ));
    io_ret_val = db_write(db->fd, table_name, db_length_str(table_name));
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
    DB_TRACE(("DB:db_create_table:num_fields = %d at %ld\n", (int) db->tables[index_table].num_fields, db_seek(db->fd, 0, DB_CURRENT_FD) ));
    io_ret_val = db_write(db->fd, &(db->tables[index_table].num_fields), DB_U_8_BIT_SIZE);
    if(io_ret_val != DB_U_8_BIT_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    /* Write fields in table */
    DB_TRACE(("\nDB:db_create_table: write %d fields\n", num_field));
    // Write each field in table to database
    int index;
    for(index = 0; index < num_field; index++)
    {
        /* Field_id */
        DB_TRACE(("\nDB:db_create_table: write field %d\n", index));
        fields[index].field_id = index + 1;

        /* Field name */
        int num_write = db_length_str(fields[index].field_name);
        // Check valid
        if(num_write > DB_MAX_LENGTH_FIELD_NAME)
        {
            DB_SET_ERROR(DB_OUT_OF_BOUNDS);
            return DB_NULL;
        }

        /* Hashing to find index fd */

        fields[index].index = db_get_empty_index_field_in_fields_bucket_by_field_name(db->fd, pos_table,(U8bit *) fields[index].field_name);
        DB_TRACE(("DB:db_create_table: pos_table = %ld\n", pos_table));
        if(db_set_index_field_info_in_table(db->fd, pos_table, index, fields[index]) == DB_FAILURE)
        {
            return DB_NULL;
        }
        
        /* Write in fields bucket */
        // Set field is used in fields bucket
        db_flag_t flag = DB_FLAG_USED;
        if(db_set_flag_in_fields_bucket(db->fd, pos_table, fields[index].index, flag) == DB_FAILURE)
        {
            return DB_NULL;
        }
    }
    
    /* Write num_row, because table is just created, num_row = 0 */
    int num_row = 0;
    DB_TRACE(("DB:db_create_table: set num_row = %d\n", num_row));
    if(db_set_num_row_in_table(db->fd, pos_table, num_row) == DB_FAILURE)
    {
        return DB_NULL;
    }

    
    /* Write number table info to database info */
    DB_TRACE(("DB:db_set_num_table: set num_table = %d\n", num_table));
    if(db_set_num_table(db->fd, num_table) == DB_FAILURE)
    {
        return DB_NULL;
    }

    /* Write position of table to database info */
    if(db_set_position_index_table(db->fd, index_table, pos_table) == DB_FAILURE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    /* Set last position */
    if(db_set_last_position(db->fd, new_last_pos) == DB_FAILURE)
    {
        return DB_NULL;
    }
    

    /* Increase num_table */
    db->num_table = num_table;
    db->last_position = new_last_pos;
    /* Table pos */
    db->tables[index_table].position_table = pos_table;
    /* ID table */
    db->tables[index_table].id_table = now_id_table;
    /* Write table name */
    memcpy(db->tables[index_table].table_name, table_name, db_length_str(table_name));
    /* Num_field */
    db->tables[index_table].num_fields = num_field;
    /* Fields */
    db->tables[index_table].fields = (db_field_t *) db_alloc(DB_MAX_FIELDS_IN_TABLE * DB_FIELD_INFO_SIZE);
    /* Init all field */
    for(i = 0; i < DB_MAX_FIELDS_IN_TABLE; i++)
    {
        db->tables[index_table].fields[i].index = -1;
    }

    for(i = 0; i < num_field; i++)
    {
        int index_field = fields[i].index;
        db->tables[index_table].fields[index_field].field_id = fields[i].field_id;
        memcpy(db->tables[index_table].fields[index_field].field_name, fields[i].field_name, DB_MAX_LENGTH_FIELD_NAME);
        DB_TRACE(("DB:db_create_table:field_name[%d] = %s\n", index_field, db->tables[index_table].fields[index_field].field_name));
        db->tables[index_table].fields[index_field].index = fields[i].index;
    }

    return &(db->tables[index_table]);
}