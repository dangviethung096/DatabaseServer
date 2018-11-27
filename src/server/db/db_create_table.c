/* 
    File: db_create_table.h
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"
#include "db_def.h"
#include "db_core_funcs.h"
#include "db_error.h"
#include <fcntl.h>
#include <sys/stat.h>

db_table_info * db_create_table(DATABASE db, char *table_name, db_field * fields, int num_field)
{
    // Declare variable
    off_t pos_table = db->last_position;
    off_t new_last_pos;
    ssize_t io_ret_val;
    int num_table = db->num_table;
    int now_id_table;
    if(num_table == 0)
    {
        now_id_table = 1;
    }else{
        now_id_table = db->tables[db->num_table].id_table + 1;
    }


    // Check validate table
    if(db->num_table == DB_MAX_TABLE_IN_DATABASE)
    {
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return DB_NULL;
    }
    
    /* Add new block to db (block contains buckets, table info) */
    {
        // Seek to new last position
        new_last_pos = db_seek(db->fd, db->last_position + DB_SINGLE_TABLE_SIZE, DB_BEGIN_FD);
        if(new_last_pos == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }

        // Mark new last position
        char end = '\0';
        io_ret_val = db_read(db->fd, &end, 1);
    }
    

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
    io_ret_val = db_write(db->fd, &now_id_table, DB_INT_SIZE);
    if(io_ret_val != DB_INT_SIZE){
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

    io_ret_val = db_write(db->fd, table_name, db_strlen(table_name));
    if(io_ret_val == -1)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_NULL;
    }

    // Write fields in table
    off_t pos_first_field_in_table = pos_table + DB_POS_FIELDS_IN_TABLE;
    if(db_seek(db->fd, pos_first_field_in_table, DB_CURRENT_FD) == -1)
    {
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return DB_NULL;
    }
    int index;
    for(index = 0; index < num_field; index++)
    {
        // Write each field in table to database
        int num_write = db_strlen(fields[index].field);
        io_ret_val = db_write(db->fd, fields[index].field, num_write);
        if(io_ret_val != num_write)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        
        if(db_seek(db->fd, pos_first_field_in_table + index * DB_FIELD_IN_TABLE_SIZE, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
    }
    // Write in rows bucket

    // Write in fields bucket


    // Increase num_table
    db->num_table++;
    db->last_position = new_last_pos;


    return &(db->tables[num_table]);
}