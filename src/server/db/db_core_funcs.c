/* 
    File: db_core_funcs.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_struct.h"
#include "db_def.h"
#include "db_error.h"
#include <sys/types.h>
#include "db_core_funcs.h"
/* 
    Function: find_nearest_prime_number
    Params: input,
            
    Description: find nearest prime number is greater than input
        
 */
static int is_prime(long int input)
{
    int index;
    for(index = 2; index < input; index++)
    {
        if(!(input % index))
        {
            return 0;
        }
    }
    return 1;
}

/* 
    Function: find_nearest_prime_number
    Params: input,
            
    Description: find nearest prime number is greater than input
        
 */
long int db_find_nearest_prime_number(long int input)
{
    long int output = input;
    // Check number is greater or equal than input
    while(!is_prime(output))
        output++;
    
    return output;
}

/* 
    Function: db_point_to_index_fields_bucket
    Params: fd,
            pos_rows_bucket,
            index
    Description: point fd to index of fields bucket
                 
    Return value: -1 if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_index_fields_bucket(int fd, off_t pos_fields_bucket, db_index_t index)
{
    if(index > DB_MAX_FIELDS_IN_TABLE)
    {
        DB_TRACE(("DB:db_point_to_index_fields_bucket: out of bound!\n"));
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return -1;
    }

    off_t pos = pos_fields_bucket + index * DB_FIELD_SIZE_IN_FIELD_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_point_to_index_fields_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }

    return pos;
}
/* 
    Function: db_get_flag_in_fields_bucket
    Params: fd,
            pos_rows_bucket,
            index,
            flag
    Description: get flag from fields bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_get_flag_in_fields_bucket(int fd, off_t pos_fields_bucket, db_index_t index, db_flag_t * flag)
{
    off_t pos = db_point_to_index_fields_bucket(fd, pos_fields_bucket, index);
    if(db_point_to_index_fields_bucket(fd, pos_fields_bucket, index) == -1)
    {
        return DB_FAILURE;
    }

    pos += DB_POS_FLAG_IN_FIELDS_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_get_flag_in_fields_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    // get flag 
    ssize_t io_ret_val;
    io_ret_val =  db_read(fd, flag, DB_FLAG_T_SIZE);
    if(io_ret_val != DB_FLAG_T_SIZE)
    {
        DB_TRACE(("DB:db_get_flag_in_fields_bucket: read_wrong!\n"));
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}

/* 
    Function: db_set_flag_in_fields_bucket
    Params: fd,
            pos_rows_bucket,
            index,
            flag
    Description: set flag from fields bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_flag_in_fields_bucket(int fd, off_t pos_fields_bucket, db_index_t index, db_flag_t flag)
{
    
    off_t pos = db_point_to_index_fields_bucket(fd, pos_fields_bucket, index);
    if(pos == -1)
    {
        return DB_FAILURE;
    }
    pos += DB_POS_FLAG_IN_FIELDS_BUCKET;

    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_set_flag_in_fields_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    // Set flag
    ssize_t io_ret_val;
    io_ret_val = db_write(fd, &flag, DB_FLAG_T_SIZE);
    if(io_ret_val != DB_FLAG_T_SIZE)
    {
        DB_TRACE(("DB:db_set_flag_in_fields_bucket: write wrong!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}

/* 
    Function: db_is_field_in_fields_bucket_used
    Params: fd,
            pos_fields_bucket,
            index
    Description: check that field_index is used or unused
                 
    Return value: DB_FALSE(0) if error or not found
                  DB_TRUE (1) if found
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_is_field_in_fields_bucket_used(int fd, off_t pos_fields_bucket, db_index_t index)
{
    db_flag_t flag;
    if(db_get_flag_in_fields_bucket(fd, pos_fields_bucket, index, &flag) == DB_FAILURE)
    {
        return DB_FALSE;
    }

    if((flag & DB_FLAG_NOT_USED))
    {
        return DB_TRUE;
    }

    return DB_FALSE;
}

/* 
    Function: db_point_to_index_rows_bucket
    Params: fd,
            pos_rows_bucket,
            index
    Description: 
                 
    Return value: -1(0) if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_index_rows_bucket(int fd, off_t pos_rows_bucket, db_index_t index)
{
    if(index > DB_MAX_ROWS_IN_BUCKET)
    {
        DB_TRACE(("DB:db_is_row_in_rows_bucket_used: out of bound, index = %d!\n", index));
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return -1;
    }

    off_t pos = pos_rows_bucket + index * DB_ROW_SIZE_IN_ROW_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_is_row_in_rows_bucket_used: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }

    return pos;
}

/* 
    Function: db_get_flag_in_rows_bucket
    Params: fd,
            pos_rows_bucket,
            index,
            flag
    Description: get flag in rows bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_get_flag_in_rows_bucket(int fd, off_t pos_rows_bucket, db_index_t index, db_flag_t * flag)
{
    off_t pos = db_point_to_index_rows_bucket(fd, pos_rows_bucket, index);
    if(pos == -1)
    {
        return DB_FAILURE;
    }

    // Seek to flag in field bucket
    pos += DB_POS_FLAG_IN_FIELDS_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_get_flag_in_rows_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    // get flag 
    ssize_t io_ret_val;
    io_ret_val =  db_read(fd, &flag, DB_FLAG_T_SIZE);
    if(io_ret_val != DB_FLAG_T_SIZE)
    {
        DB_TRACE(("DB:db_get_flag_in_rows_bucket: read_wrong!\n"));
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}

/* 
    Function: db_set_flag_in_rows_bucket
    Params: fd,
            pos_rows_bucket,
            index,
            flag
    Description: set flag in rows bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_flag_in_rows_bucket(int fd, off_t pos_rows_bucket, db_index_t index, db_flag_t flag)
{
    off_t pos = db_point_to_index_rows_bucket(fd, pos_rows_bucket, index);
    if(pos == -1)
    {
        return DB_FAILURE;
    }

    // Seek to flag in field bucket
    pos += DB_POS_FLAG_IN_FIELDS_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_set_flag_in_rows_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    // Write flag
    ssize_t io_ret_val;
    io_ret_val = db_write(fd, &flag, DB_FLAG_T_SIZE);
    if(io_ret_val != DB_FLAG_T_SIZE)
    {
        DB_TRACE(("DB:db_set_flag_in_rows_bucket: write wrong!\n"));
    }

    return DB_SUCCESS;
}

/* 
    Function: db_is_row_in_rows_bucket_used
    Params: fd,
            pos_rows_bucket,
            index
    Description: check that rows_index is used or unused
                 
    Return value: DB_FALSE(0) if error or not found
                  DB_TRUE (1) if found
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_is_row_in_rows_bucket_used(int fd, off_t pos_rows_bucket, db_index_t index)
{
    db_flag_t flag;
    if((flag & DB_FLAG_NOT_USED))
    {
        return DB_TRUE;
    }
    return DB_FALSE;
}


/* 
    Function: db_point_to_index_table_info_in_db
    Params: fd,
            index
    Description: point fd to index of table info in database
                 database have a array of poisition of table
    Return value: -1 if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_index_table_info_in_db(int fd, int index)
{
    off_t pos = DB_POS_FIRST_TABLE_POS + index * DB_OFF_T_SIZE;
    DB_TRACE(("DB:db_point_to_index_table_info_in_db: seek to %ld\n", pos));
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_point_to_index_fields_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }

    return pos;
}

/* 
    Function: db_get_position_index_table
    Params: fd,
            index
    Description: get position of index table
    Return value: -1 if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_get_position_index_table(int fd, int index)
{
    if(db_point_to_index_table_info_in_db(fd, index) == -1)
    {
        return -1;
    }
    off_t pos_table = 0;
    DB_TRACE(("DB:db_get_position_index_table: read at %ld\n", db_seek(fd, 0, DB_CURRENT_FD) ));
    ssize_t io_ret_val = db_read(fd, &pos_table, DB_OFF_T_SIZE);
    DB_TRACE(("DB:db_get_position_index_table: read pos %ld in position table info\n", pos_table));
    if(io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return -1;
    }

    return pos_table;
}

/* 
    Function: db_set_position_index_table
    Params: fd,
            index
    Description: set position of index table
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_position_index_table(int fd, int index, off_t pos)
{
    if(db_point_to_index_table_info_in_db(fd, index) == -1)
    {
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_set_position_index_table: write at %ld\n", db_seek(fd, 0, DB_CURRENT_FD)));
    ssize_t io_ret_val = db_write(fd, &pos, DB_OFF_T_SIZE);
    DB_TRACE(("DB:db_set_position_index_table: write pos %ld to position table info\n", pos));
    if(io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}
/* 
    Function: db_get_last_position
    Params: fd
    Description: get last position
    Return value: -1 if error
                  last position if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_get_last_position(int fd)
{   
    off_t pos = DB_POS_LAST_POSITION;
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }

    off_t last_position;
    ssize_t io_ret_val;
    DB_TRACE(("DB:db_create_table:read last position: %ld at %ld\n", last_position, pos));
    io_ret_val = db_read(fd, &last_position, DB_OFF_T_SIZE);
    if (io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return -1;
    }

    return last_position;
}

/* 
    Function: db_set_last_position
    Params: fd
    Description: set last position
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_last_position(int fd, off_t last_position)
{
    off_t pos = DB_POS_LAST_POSITION;
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    ssize_t io_ret_val;
    DB_TRACE(("DB:db_create_table:write last position: %ld at %ld\n", last_position, pos));
    io_ret_val = db_write(fd , &last_position, DB_OFF_T_SIZE);
    if (io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}
/* 
    Function: point_to_num_table
    Params: fd
    Description: point to position of number table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
static inline off_t point_to_num_table(int fd)
{
    off_t pos = DB_POS_NUMBER_TABLE;
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    return pos;
}
/* 
    Function: db_get_num_table
    Params: fd
    Description: get number table
    Return value: -1 if error
                  number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
unsigned int db_get_num_table(int fd)
{
    off_t pos = point_to_num_table(fd);
    if (pos == -1)
    {
        return -1;
    }
    ssize_t io_ret_val;
    unsigned int num_table;
    io_ret_val = db_read(fd, &num_table, DB_INT_SIZE);
    DB_TRACE(("DB:db_get_num_table:get number table = %d at %ld\n", num_table, pos));
    if (io_ret_val != DB_INT_SIZE)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return -1;
    }

    return num_table;
}

/* 
    Function: db_set_num_table
    Params: fd
    Description: set number table in database
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_num_table(int fd, unsigned int num_table)
{
    off_t pos = point_to_num_table(fd);
    if (pos == -1)
    {
        return DB_FAILURE;
    }
    ssize_t io_ret_val;
    DB_TRACE(("DB:db_set_num_table:write number table = %d at %ld\n", num_table, pos));
    io_ret_val = db_write(fd, &num_table, DB_INT_SIZE);
    
    if (io_ret_val != DB_INT_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}