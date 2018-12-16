/* 
    File: db_core_funcs.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_struct.h"
#include "db_def.h"
#include "db_error.h"
#include "db_hash_function.h"
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
    Function: db_point_to_fields_bucket_by_index
    Params: fd,
            table_pos,
            index
    Description: point fd to index of fields bucket
                 
    Return value: -1 if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_fields_bucket_by_index(int fd, off_t table_pos, db_index_t index)
{
    if(index > DB_MAX_FIELDS_IN_TABLE)
    {
        DB_TRACE(("DB:db_point_to_fields_bucket_by_index: out of bound!\n"));
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return -1;
    }

    off_t pos = table_pos + DB_POS_FIELDS_BUCKET_IN_TABLE + index * DB_FIELD_SIZE_IN_FIELD_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_point_to_fields_bucket_by_index: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:db_point_to_fields_bucket_by_index: field index = %d at %ld!\n", index, pos));
    return pos;
}
/* 
    Function: db_get_flag_in_fields_bucket
    Params: fd,
            table_pos,
            index,
            flag
    Description: get flag from fields bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_get_flag_in_fields_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t * flag)
{
    off_t pos = db_point_to_fields_bucket_by_index(fd, table_pos, index);
    if(pos == -1)
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
    DB_TRACE(("DB:db_get_flag_in_fields_bucket: flag = %x at %ld!\n", (int) *flag, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_set_flag_in_fields_bucket
    Params: fd,
            table_pos,
            index,
            flag
    Description: set flag from fields bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_flag_in_fields_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t flag)
{
    
    off_t pos = db_point_to_fields_bucket_by_index(fd, table_pos, index);
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
    DB_TRACE(("DB:db_set_flag_in_fields_bucket: write flag = %d at %ld!\n", (int) flag, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_is_field_in_fields_bucket_used
    Params: fd,
            table_pos,
            index
    Description: check that field_index is used or unused
                 
    Return value: DB_FALSE(0) if error or not found
                  DB_TRUE (1) if found
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_is_field_in_fields_bucket_used(int fd, off_t table_pos, db_index_t index)
{
    db_flag_t flag;
    if(db_get_flag_in_fields_bucket(fd, table_pos, index, &flag) == DB_FAILURE)
    {
        return DB_FALSE;
    }

    if( (flag & DB_FLAG_USED) == DB_FLAG_USED )
    {
        return DB_TRUE;
    }

    return DB_FALSE;
}

/* 
    Function: db_point_to_rows_bucket_by_index
    Params: fd,
            table_pos,
            index
    Description: 
                 
    Return value: -1(0) if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_rows_bucket_by_index(int fd, off_t table_pos, db_index_t index)
{
    if(index > DB_MAX_ROWS_IN_BUCKET)
    {
        DB_TRACE(("DB:db_point_to_rows_bucket_by_index: out of bound, index = %d!\n", index));
        DB_SET_ERROR(DB_OUT_OF_BOUNDS);
        return -1;
    }

    off_t pos = table_pos + DB_POS_ROWS_BUCKET_IN_TABLE + index * DB_ROW_SIZE_IN_ROW_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_point_to_rows_bucket_by_index: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:db_point_to_rows_bucket_by_index: index_rows = %d at %ld!\n", index, pos));
    return pos;
}

/* 
    Function: db_get_flag_in_rows_bucket
    Params: fd,
            table_pos,
            index,
            flag
    Description: get flag in rows bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_get_flag_in_rows_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t * flag)
{
    off_t pos = db_point_to_rows_bucket_by_index(fd, table_pos, index);
    if(pos == -1)
    {
        return DB_FAILURE;
    }

    // Seek to flag in field bucket
    pos += DB_POS_FLAG_IN_ROWS_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_get_flag_in_rows_bucket: seek_fail!\n"));
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }

    // get flag 
    ssize_t io_ret_val;
    io_ret_val =  db_read(fd, flag, DB_FLAG_T_SIZE);
    
    if(io_ret_val != DB_FLAG_T_SIZE)
    {
        DB_TRACE(("DB:db_get_flag_in_rows_bucket: read_wrong!\n"));
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_FAILURE;
    }
    DB_TRACE(("DB:db_get_flag_in_rows_bucket: read flag = %d at %ld!\n", (int) *flag, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_set_flag_in_rows_bucket
    Params: fd,
            table_pos,
            index,
            flag
    Description: set flag in rows bucket
                 
    Return value: DB_FAILURE(0) if error
                  DB_SUCCESS (1) if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_flag_in_rows_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t flag)
{
    off_t pos = db_point_to_rows_bucket_by_index(fd, table_pos, index);
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
    DB_TRACE(("DB:db_set_flag_in_rows_bucket: write flag = %d at %ld\n", flag, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_is_row_in_rows_bucket_used
    Params: fd,
            table_pos,
            index
    Description: check that rows_index is used or unused
                 
    Return value: DB_FALSE(0) if error or not found
                  DB_TRUE (1) if found
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_is_row_in_rows_bucket_used(int fd, off_t table_pos, db_index_t index)
{
    db_flag_t flag;
    if(db_get_flag_in_rows_bucket(fd, table_pos, index, &flag) == DB_FAILURE)
    {
        DB_TRACE(("DB:db_is_row_in_rows_bucket_used: get flag false!\n"));
        return DB_FALSE;
    }
    if((flag & DB_FLAG_USED) == DB_FLAG_USED)
    {
        DB_TRACE(("DB:db_is_row_in_rows_bucket_used: True!\n"));
        return DB_TRUE;
    }
    DB_TRACE(("DB:db_is_row_in_rows_bucket_used: False!\n"));
    return DB_FALSE;
}


/* 
    Function: db_point_to_table_info_in_db_by_index
    Params: fd,
            index
    Description: point fd to index of table info in database
                 database have a array of poisition of table
    Return value: -1 if error
                  current position of fd if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_table_info_in_db_by_index(int fd, int index)
{
    off_t pos = DB_POS_FIRST_TABLE_POS + index * DB_OFF_T_SIZE;
    DB_TRACE(("DB:db_point_to_table_info_in_db_by_index: seek to %ld\n", pos));
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_TRACE(("DB:db_point_to_table_info_in_db_by_index: seek_fail!\n"));
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
    if(db_point_to_table_info_in_db_by_index(fd, index) == -1)
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
            index,
            pos
    Description: set position of index table
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_position_index_table(int fd, int index, off_t pos)
{
    if(db_point_to_table_info_in_db_by_index(fd, index) == -1)
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
    Function: point_to_last_position
    Params: fd
    Description: point to last position
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
static inline off_t point_to_last_position(int fd)
{
    off_t pos = DB_POS_LAST_POSITION;
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    return pos;
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
    off_t pos = point_to_last_position(fd);
    if(pos == -1)
    {
        return -1;
    }

    off_t last_position;
    ssize_t io_ret_val;
    io_ret_val = db_read(fd, &last_position, DB_OFF_T_SIZE);
    DB_TRACE(("DB:db_create_table:read last position: %ld at %ld\n", last_position, pos));
    if (io_ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return -1;
    }

    return last_position;
}


/* 
    Function: db_set_last_position
    Params: fd,
            last_position
    Description: set last position
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_last_position(int fd, off_t last_position)
{
    off_t pos = point_to_last_position(fd);
    if(pos == -1)
    {
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
    /* Get num table */
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
    Params: fd,
            num_table
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
    /* Set num_table */
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

/* 
    Function: point_to_database_name
    Params: fd
    Description: point to name of database
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
static inline off_t point_to_database_name(int fd)
{
    off_t pos = DB_POS_NAME_DATABASE;
    
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:point_to_database_name:point to database_name at %ld\n", pos));
    return pos;
}

/* 
    Function: db_set_database_name
    Params: fd,
            database_name
    Description: set number database name
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_database_name(int fd, char * database_name)
{
    off_t pos = point_to_database_name(fd);
    if (pos == -1)
    {
        return DB_FAILURE;
    }
    ssize_t io_ret_val;
    DB_TRACE(("DB:db_set_database_name:write database name = %s\n", database_name));
    io_ret_val = db_write(fd, database_name, DB_MAX_LENGTH_DB_NAME);

    if (io_ret_val != DB_MAX_LENGTH_DB_NAME)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}

/* 
    Function: db_get_database_name
    Params: fd,
            database_name
    Description: get number database name
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_get_database_name(int fd, char *database_name)
{
    off_t pos = point_to_database_name(fd);
    if (pos == -1)
    {
        return DB_FAILURE;
    }
    ssize_t io_ret_val;
    io_ret_val = db_read(fd, database_name, DB_MAX_LENGTH_DB_NAME);
    DB_TRACE(("DB:db_set_database_name:read database name = %s\n", database_name));
    if (io_ret_val != DB_MAX_LENGTH_DB_NAME)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}



/* 
    Function: db_point_to_rows_bucket_by_field_index
    Params: fd,
            table_pos,
            rows_index,
            field_index
    Description: point to fields of rows bucket
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

off_t db_point_to_rows_bucket_by_field_index(int fd, off_t table_pos, int rows_index, int field_index)
{
    off_t pos = db_point_to_rows_bucket_by_index(fd, table_pos, rows_index);
    
    pos += (field_index * DB_UNIT_SIZE_IN_ROWS_BUCKET);

    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:db_point_to_rows_bucket_by_field_index: field_index = %d at %ld\n", field_index, pos));
    return pos;
}

/* 
    Function: db_point_to_table_info_in_table
    Params: fd,
            table_pos,
    Description: point to table info in table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

off_t db_point_to_table_info_in_table(int fd, off_t table_pos)
{
    off_t pos = table_pos + DB_POS_TABLE_INFO_IN_TABLE;
    
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:db_point_to_table_info_in_table:point table info at %ld\n", pos));
    return pos;
}

/* 
    Function: db_point_to_field_info_in_table_by_index
    Params: fd,
            table_pos,
            index
    Description: point to index field info in table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

off_t db_point_to_field_info_in_table_by_index(int fd, off_t table_pos, int index)
{
    off_t pos = db_point_to_table_info_in_table(fd, table_pos);
    
    pos += DB_POS_FIELDS_IN_TABLE +  index * DB_FIELD_INFO_SIZE;
    
    if (db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:db_point_to_field_info_in_table_by_index:point index field info = %d at %ld\n", index, pos));
    return pos;
}

/* 
    Function: db_get_field_info_in_table_by_index
    Params: fd,
            table_pos,
            index
    Description: 
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_field_t db_get_field_info_in_table_by_index(int fd, off_t table_pos, int index)
{
    db_field_t field;
    if(db_point_to_field_info_in_table_by_index(fd, table_pos, index) == -1)
    {
        return field;
    }
    ssize_t io_ret_val = db_read(fd, &field, DB_FIELD_INFO_SIZE);
    if(io_ret_val != DB_FIELD_INFO_SIZE)
    {
        DB_SET_ERROR(DB_READ_WRONG);
    }
    DB_TRACE(("DB:db_get_field_info_in_table_by_index: field id = %d\n", field.field_id));
    DB_TRACE(("DB:db_get_field_info_in_table_by_index: field name = %s\n", field.field_name));
    DB_TRACE(("DB:db_get_field_info_in_table_by_index: field index = %d\n", field.index));
    return field;
}


/* 
    Function: db_set_index_field_info_in_table
    Params: fd,
            table_pos,
            index
    Description: point to index field info in table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_index_field_info_in_table(int fd, off_t table_pos, int index, db_field_t field)
{
    if(db_point_to_field_info_in_table_by_index(fd, table_pos, index) == -1)
    {
        return DB_FAILURE;
    }
    ssize_t io_ret_val = db_write(fd, &field, DB_FIELD_INFO_SIZE);
    if(io_ret_val != DB_FIELD_INFO_SIZE)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }
    DB_TRACE(("DB:db_set_index_field_info_in_table: field id = %d\n", field.field_id));
    DB_TRACE(("DB:db_set_index_field_info_in_table: field name = %s\n", field.field_name));
    DB_TRACE(("DB:db_set_index_field_info_in_table: field index = %d\n", field.index));
    return DB_SUCCESS;
}

/* 
    Function: db_get_index_field_in_fields_bucket_by_field_name
    Params: fd,
            table,
            field_name
    Description: point to index field info in table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
int db_get_index_field_in_fields_bucket_by_field_name(int fd, db_table_info *table, U8bit *field_name)
{
    db_first_hash_ret_t hval = 0;
    int num_hash = 0;
    int index = 0;
    db_hash_function(field_name, &hval, DB_MAX_FIELDS_IN_TABLE, &num_hash, &index);
    int first_index = index;

    do
    {
        DB_TRACE(("DB:db_get_index_field_in_fields_bucket_by_field_name:field_name = %s\n", field_name));
        DB_TRACE(("DB:db_get_index_field_in_fields_bucket_by_field_name:table->field[%d].field_name = %s\n", index, table->fields[index].field_name));
        DB_TRACE(("DB:db_get_index_field_in_fields_bucket_by_field_name:table_pos = %ld\n", table->position_table));
        // DB_TRACE(("DB:db_get_index_field_in_fields_bucket_by_field_name:is used = %d\n", db_is_field_in_fields_bucket_used(fd, table->position_table, index)));
        if (db_strncmp(table->fields[index].field_name, field_name, db_length_str(field_name)) == 0 
            && db_is_field_in_fields_bucket_used(fd, table->position_table, index) == DB_TRUE)
        {
            return index;
        }
        db_hash_function(field_name, &hval, DB_MAX_FIELDS_IN_TABLE, &num_hash, &index);
    }while(first_index != index);

    return -1;
}

/* 
    Function: db_point_to_fields_bucket_by_field_name
    Params: fd,
            table,
            field_name
    Description: point to index field info in table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_point_to_fields_bucket_by_field_name(int fd, db_table_info *table, U8bit *field_name)
{
    off_t pos;
    
    int field_index = db_get_index_field_in_fields_bucket_by_field_name(fd, table, field_name);

    pos = db_point_to_fields_bucket_by_index(fd, table->position_table, field_index);
    if (pos == -1)
    {
        return -1;
    }
    DB_TRACE(("DB:db_point_to_fields_bucket_by_field_name:pos = %ld\n", pos));
    return pos;
}



/* 
    Function: db_get_index_table_from_table_name
    Params: db,
            table_name
    Description: get index of table from table name
    Return value: -1 if cannot found any table_name in database
                  index of table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
int db_get_index_table_from_table_name(DATABASE db, U8bit *table_name)
{
    int i;
    for(i = 0; i < db->num_table; i++)
    {
        if(db_strncmp(db->tables[i].table_name, table_name, db_length_str(table_name)) == 0)
        {
            DB_TRACE(("DB:db_get_index_table_from_table_name:index_table = %d\n", i));
            return i;
        }
    }
    // If it cannot found table_name in database
    DB_TRACE(("DB:db_get_index_table_from_table_name: cannot found table_name\n"));
    return -1;
}
/* 
    Function: db_point_to_fields_bucket_by_value
    Params: fd,
            fields_pos,
            value_index
    Description: search new memory for store value in fields bucket
    Return value: -1 if cannot found any table_name in database
                  index of table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
off_t db_point_to_fields_bucket_by_value_index(int fd, off_t fields_pos, db_index_t value_index)
{
    off_t pos;
    pos = fields_pos + value_index * DB_UNIT_SIZE_IN_FIELDS_BUCKET;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:db_point_to_fields_bucket_by_value_index:pos = %ld\n", pos));
    return pos;
}

/* 
    Function: db_get_value_in_fields_bucket
    Params: fd,
            fields_pos,
            value_index
    Description: search new memory for store value in fields bucket
    Return value: -1 if cannot found any table_name in database
                  index of table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
db_boolean_t db_get_value_in_fields_bucket(int fd, off_t field_pos, db_index_t value_index, db_value_field_t * value)
{
    off_t pos = db_point_to_fields_bucket_by_value_index(fd, field_pos, value_index);
    if (pos == -1)
    {
        return DB_FAILURE;
    }

    ssize_t io_ret_val = db_read(fd, value, DB_UNIT_SIZE_IN_FIELDS_BUCKET);
    
    if( io_ret_val == -1 )
    {
        DB_TRACE(("DB:db_get_value_in_fields_bucket: io_ret_val = %lu at %ld\n", io_ret_val, pos));
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.flag = %d at %ld\n", (int) value->flag, pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.row_id = %d at %ld\n", value->row_id, pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.size = %d at %ld\n", value->size, pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.val = %s at %ld\n", value->value, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_set_value_in_fields_bucket
    Params: fd,
            fields_pos,
            value_index
    Description: Set value in field bucket
    Return value: -1 if cannot found any table_name in database
                  index of table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
db_boolean_t db_set_value_in_fields_bucket(int fd, off_t field_pos, db_index_t value_index, db_value_field_t value)
{
    off_t pos = db_point_to_fields_bucket_by_value_index(fd, field_pos, value_index);
    if (pos == -1)
    {
        return DB_FAILURE;
    }

    ssize_t io_ret_val = db_write ( fd, &value, DB_UNIT_SIZE_IN_FIELDS_BUCKET );

    if (io_ret_val == -1)
    {
        DB_TRACE(("DB:db_set_value_in_fields_bucket: io_ret_val = %lu at %ld\n", io_ret_val, pos));
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_set_value_in_fields_bucket: set value.flag = %d at %ld\n", (int) value.flag, pos));
    DB_TRACE(("DB:db_set_value_in_fields_bucket: set value.row_id = %d at %ld\n", value.row_id, pos));
    DB_TRACE(("DB:db_set_value_in_fields_bucket: set value.size = %d at %ld\n", value.size, pos));
    DB_TRACE(("DB:db_set_value_in_fields_bucket: set value.val = %s at %ld\n", value.value, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_is_value_in_field_bucket_used
    Params: fd,
            fields_pos,
            value_index
    Description: search new memory for store value in fields bucket
    Return value: DB_FALSE if value is not used
                  DB_TRUE if value is used
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
db_boolean_t db_is_value_in_field_bucket_used(int fd, off_t field_pos, db_index_t value_index)
{
    // Get value pos
    DB_TRACE(("DB:db_is_value_in_field_bucket_used: field_pos = %ld, value_index = %d\n", field_pos, value_index));
    db_value_field_t value;
    if(db_get_value_in_fields_bucket(fd, field_pos, value_index, &value) == DB_FAILURE)
    {
        return DB_FAILURE;
    }
    
    if(value.flag == DB_FLAG_USED)
    {
        return DB_TRUE;
    }
    
    return DB_FALSE;
}

/* 
    Function: db_get_empty_index_field_in_fields_bucket_by_field_name
    Params: fd,
            table,
            field_name
    Description: point to index field info in table
    Return value: -1 if error
                  position of number table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
int db_get_empty_index_field_in_fields_bucket_by_field_name(int fd, off_t table_pos, U8bit *field_name)
{
    db_first_hash_ret_t hval = 0;
    int num_hash = 0;
    int index = 0;
    db_hash_function(field_name, &hval, DB_MAX_FIELDS_IN_TABLE, &num_hash, &index);
    int first_index = index;
    do
    {
        DB_TRACE(("DB:db_get_empty_index_field_in_fields_bucket_by_field_name:index = %d\n", index));
        if (db_is_field_in_fields_bucket_used(fd, table_pos, index) == DB_FALSE)
        {
            // Have an errro
            if(db_error_no != DB_NO_ERROR)
                break;
            // Return success
            return index;
        }
        db_hash_function(field_name, &hval, DB_MAX_FIELDS_IN_TABLE, &num_hash, &index);
    }while(first_index != index);
    
    return -1;
}


/* 
    Function: db_get_value_pos_from_rows_bucket_by_field_index
    Params: 
    Description: 
    Return value: -1 if error
                  position of value if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
off_t db_get_value_pos_from_rows_bucket_by_field_index(int fd, off_t table_pos, db_index_t row_index, db_index_t field_index)
{
    off_t pos;
    pos = db_point_to_rows_bucket_by_field_index(fd, table_pos, row_index, field_index);
    if(pos == -1)
    {
        return -1;
    }
    // Read position of field 
    ssize_t io_ret_val;
    off_t val_pos;
    io_ret_val = db_read(fd, &val_pos, DB_UNIT_SIZE_IN_ROWS_BUCKET);
    if(io_ret_val != DB_UNIT_SIZE_IN_ROWS_BUCKET)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return -1;
    }
    DB_TRACE(("DB:db_get_value_pos_from_rows_bucket_by_field_index: read val_pos = %ld at %ld\n", val_pos, pos));
    return val_pos;
}
/* 
    Function: db_set_value_pos_in_rows_bucket_by_field_index
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_value_pos_in_rows_bucket_by_field_index(int fd, off_t table_pos, db_index_t row_index, db_index_t field_index, off_t val_pos)
{
    off_t pos;
    pos = db_point_to_rows_bucket_by_field_index(fd, table_pos, row_index, field_index);
    if(pos == -1)
    {
        return DB_FAILURE;
    }
    // Write val_pos
    ssize_t io_ret_val;
    io_ret_val = db_write(fd, &val_pos, DB_UNIT_SIZE_IN_ROWS_BUCKET);
    if(io_ret_val != DB_UNIT_SIZE_IN_ROWS_BUCKET)
    {
        DB_TRACE(("DB:db_set_value_pos_in_rows_bucket_by_field_index: write wrong\n"));
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_set_value_pos_in_rows_bucket_by_field_index: write val_pos = %ld at %ld\n", val_pos, pos));
    return DB_SUCCESS;
}

/* 
    Function: point_to_num_row_in_table_info
    Params: 
    Description: 
    Return value: -1 if error
                  pos if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

static off_t point_to_num_row_in_table_info(int fd, off_t table_pos)
{
    off_t pos = table_pos + DB_POS_TABLE_INFO_IN_TABLE + DB_POS_NUM_ROW_IN_TABLE;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return -1;
    }
    DB_TRACE(("DB:point_to_num_row_in_table_info: num_row at %ld\n", pos));
    return pos;
}

/* 
    Function: db_get_num_row_in_table
    Params: 
    Description: 
    Return value: -1 if error
                  num_row if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
U32bit db_get_num_row_in_table(int fd, off_t table_pos)
{
    off_t pos = point_to_num_row_in_table_info(fd, table_pos);
    if(pos == -1)
    {
        return -1;
    }
    // Read num row
    int num_row;
    ssize_t io_ret_val = db_read(fd, &num_row, DB_U_32_BIT_SIZE);
    if(io_ret_val == -1)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return -1;
    }
    DB_TRACE(("DB:db_get_num_row_in_table: num_row = %u at %ld\n", num_row, pos));
    return num_row;

}

/* 
    Function: db_set_num_row_in_table
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_set_num_row_in_table(int fd, off_t table_pos, U32bit num_row)
{
    off_t pos = point_to_num_row_in_table_info(fd, table_pos);
    if (pos == -1)
    {
        return DB_FAILURE;
    }
    // Write num row
    ssize_t io_ret_val = db_write(fd, &num_row, DB_U_32_BIT_SIZE);
    if (io_ret_val == -1)
    {
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_set_num_row_in_table: write num_row = %u at %ld\n", num_row, pos));
    return DB_SUCCESS;
}

/* 
    Function: db_get_value_in_fields_bucket_by_value_pos
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_get_value_in_fields_bucket_by_value_pos(int fd, off_t val_pos, db_value_field_t * value)
{
    off_t pos = val_pos;
    if(db_seek(fd, pos, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_FAILURE;
    }
    // Get value
    ssize_t io_ret_val = db_read(fd, value, DB_MAX_SIZE_IN_VALUE);
    if(io_ret_val != DB_MAX_SIZE_IN_VALUE)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_get_value_in_fields_bucket_by_value_pos: read val at %ld\n", pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket_by_value_pos: flag = %d, size = %d, row_id = %d\n", (int)value->flag, value->size, value->row_id));
    DB_TRACE(("DB:db_get_value_in_fields_bucket_by_value_pos: value = %s\n", value->value));
    return DB_SUCCESS;
}
/* 
    Function: db_is_value_in_row_bucket_used
    Params: 
    Description: 
    Return value: DB_FALSE if error or not have value in row bucket
                  DB_TRUE if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_is_value_in_row_bucket_used(int fd, off_t table_pos, int row_index, int field_index)
{
    off_t pos = db_point_to_rows_bucket_by_field_index(fd, table_pos, row_index, field_index);
    if(pos == -1)
    {
        return DB_FALSE;
    }
    // Get position of value in row
    off_t val_pos = db_get_value_pos_from_rows_bucket_by_field_index(fd, table_pos, row_index, field_index);
    if(val_pos == -1)
    {
        return DB_FALSE;
    }
    DB_TRACE(("DB:db_is_value_in_row_bucket_used: value_pos = %ld\n", val_pos));
    return DB_TRUE;
}

/* 
    Function: db_is_value_in_field_bucket_used_with_field_index
    Params: fd,
            fields_pos,
            value_index
    Description: search new memory for store value in fields bucket
    Return value: DB_FALSE if value is not used
                  DB_TRUE if value is used
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
db_boolean_t db_is_value_in_field_bucket_used_with_field_index(int fd, off_t table_pos,int field_index, db_index_t value_index)
{
    off_t field_pos = db_point_to_fields_bucket_by_index(fd, table_pos, field_index);
    if(field_pos == -1)
    {
        return DB_FALSE;
    }
    // Get value pos
    DB_TRACE(("DB:db_is_value_in_field_bucket_used: field_pos = %ld, value_index = %d\n", field_pos, value_index));
    db_value_field_t value;
    if (db_get_value_in_fields_bucket(fd, field_pos, value_index, &value) == DB_FAILURE)
    {
        return DB_FAILURE;
    }

    if (value.flag == DB_FLAG_USED)
    {
        return DB_TRUE;
    }

    return DB_FALSE;
}

/* 
    Function: db_get_value_in_fields_bucket_with_field_index
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
db_boolean_t db_get_value_in_fields_bucket_with_field_index(int fd, off_t table_pos, db_index_t field_index, db_index_t value_index, db_value_field_t *value)
{
    off_t field_pos = db_point_to_fields_bucket_by_index(fd, table_pos, field_index);
    if(field_pos == -1)
    {
        return DB_FAILURE;
    }

    off_t pos = db_point_to_fields_bucket_by_value_index(fd, field_pos, value_index);
    if (pos == -1)
    {
        return DB_FAILURE;
    }

    ssize_t io_ret_val = db_read(fd, value, DB_UNIT_SIZE_IN_FIELDS_BUCKET);

    if (io_ret_val == -1)
    {
        DB_TRACE(("DB:db_get_value_in_fields_bucket: io_ret_val = %lu at %ld\n", io_ret_val, pos));
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.flag = %d at %ld\n", (int)value->flag, pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.row_id = %d at %ld\n", value->row_id, pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.size = %d at %ld\n", value->size, pos));
    DB_TRACE(("DB:db_get_value_in_fields_bucket: get value.val = %s at %ld\n", value->value, pos));
    return DB_SUCCESS;
}

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
    DB_TRACE(("DB:db_search_row_with_equal_condition: field_index = %d, value = %s\n", field_index, value));
    /* Init */
    *num_row = 0;

    db_first_hash_ret_t hval;
    int num_hash = 0;
    int index, first_index;
    db_hash_function(value, &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &first_index);
    index = first_index;

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
        DB_TRACE(("DB:db_search_row_with_equal_condition: row_id = %d\n", ret_value.row_id));
        row_ids[(*num_row)++] =  ret_value.row_id;
        db_hash_function(value, &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &index);

    }while(index != first_index);
    
    DB_TRACE(("DB:db_search_row_with_equal_condition: num_row = %d\n", *num_row));
    return DB_SUCCESS;
}

/* 
    Function: db_set_value_in_fields_bucket_by_field_index
    Params: fd,
            fields_pos,
            value_index
    Description: Set value in field bucket
    Return value: -1 if cannot found any table_name in database
                  index of table if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
db_boolean_t db_set_value_in_fields_bucket_by_field_index(int fd, off_t table_pos, db_index_t field_index, db_index_t value_index, db_value_field_t value)
{
    off_t field_pos = db_point_to_fields_bucket_by_index(fd, table_pos, field_index);
    if(field_pos == -1)
    {
        return DB_FAILURE;
    }

    off_t pos = db_point_to_fields_bucket_by_value_index(fd, field_pos, value_index);
    if (pos == -1)
    {
        return DB_FAILURE;
    }

    ssize_t io_ret_val = db_write ( fd, &value, DB_UNIT_SIZE_IN_FIELDS_BUCKET );

    if (io_ret_val == -1)
    {
        DB_TRACE(("DB:db_set_value_in_fields_bucket_by_field_index: io_ret_val = %lu at %ld\n", io_ret_val, pos));
        DB_SET_ERROR(DB_WRITE_WRONG);
        return DB_FAILURE;
    }

    DB_TRACE(("DB:db_set_value_in_fields_bucket_by_field_index: set value.flag = %d at %ld\n", (int) value.flag, pos));
    DB_TRACE(("DB:db_set_value_in_fields_bucket_by_field_index: set value.row_id = %d at %ld\n", value.row_id, pos));
    DB_TRACE(("DB:db_set_value_in_fields_bucket_by_field_index: set value.size = %d at %ld\n", value.size, pos));
    DB_TRACE(("DB:db_set_value_in_fields_bucket_by_field_index: set value.val = %s at %ld\n", value.value, pos));
    return DB_SUCCESS;
}
/* 
    Function: db_get_empty_value_in_field_of_fields_bucket
    Params: 
    Description: 
    Return value: -1 if error
                  index of value if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
int db_get_empty_value_in_field_of_fields_bucket(int fd, off_t table_pos, int field_index, U8bit * value)
{
    db_first_hash_ret_t hval;
    int num_hash = 0;
    int index, first_index;
    db_hash_function(value, &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &first_index);
    index = first_index;
    do
    {
        if(db_is_value_in_field_bucket_used_with_field_index(fd, table_pos, field_index, index) == DB_FALSE)
        {
            if(db_error_no != DB_NO_ERROR)
            {
                return -1;
            }
            break;
        }
        db_hash_function(value, &hval, DB_MAX_ROWS_IN_BUCKET, &num_hash, &index);
    }while(index != first_index);
    
    DB_TRACE(("DB:db_get_empty_value_in_field_of_fields_bucket: get empty index =  %d\n", index));
    return index;
}

/* 
    Function: db_remove_value_in_fields_bucket
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_remove_value_in_field_bucket(int fd, off_t table_pos, int field_index, int value_index)
{
    db_value_field_t reset_value;
    reset_value.flag = DB_FLAG_NOT_USED;
    reset_value.size = db_length_str(DB_STR_NULL);
    reset_value.row_id = -1;
    memcpy(reset_value.value, DB_STR_NULL, reset_value.size);

    if(db_set_value_in_fields_bucket_by_field_index(fd, table_pos, field_index, value_index, reset_value) == DB_FAILURE)
    {
        return DB_FAILURE;
    }


    /* Get all value in field of field_bucket and rehash */
    off_t field_pos = db_point_to_fields_bucket_by_index(fd, table_pos, field_index);
    if(field_pos == -1)
    {
        return DB_FAILURE;
    }
    // Get all values
    db_value_field_t values[DB_MAX_ROWS_IN_BUCKET];
    int value_count = 0;
    int i;
    for(i = 1; i <= DB_MAX_ROWS_IN_BUCKET; i++)
    {
        
        if(db_is_value_in_field_bucket_used(fd, field_pos, value_index) == DB_FALSE)
        {
            if(db_error_no != DB_NO_ERROR)
            {
                return DB_FAILURE;
            }
            // Nothing to do because no have any value
        }else
        {
            if(db_get_value_in_fields_bucket(fd, field_pos, value_index, &(values[value_count]) ) == DB_FAILURE)
            {
                return DB_FAILURE;
            }
            if(db_set_value_in_fields_bucket(fd, field_pos, value_index, reset_value) == DB_FAILURE)
            {
                return DB_FAILURE;
            }
            value_count++;
        }
    }
    // Rehash
    for(i = 0; i < value_count; i++)
    {
        
        int new_value_index = db_get_empty_value_in_field_of_fields_bucket(fd, table_pos, field_index, values[i].value);
        if(new_value_index == -1)
        {
            return DB_FAILURE;
        }
        if(db_set_value_in_fields_bucket(fd, field_pos, new_value_index, values[i]) == DB_FAILURE)
        {
            return DB_FAILURE;
        }

        DB_TRACE(("DB:db_remove_value_in_field_bucket: reshash value = %s in %d of field\n", values[i].value, new_value_index));
    }


    DB_TRACE(("DB:db_remove_value_in_field_bucket: remove field_index = %d, value_index = %d success\n", field_index, value_index));
    return DB_SUCCESS;
}

/* 
    Function: db_remove_value_pos_in_rows_bucket
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

db_boolean_t db_remove_value_pos_in_rows_bucket(int fd, off_t table_pos, db_index_t row_index, db_index_t field_index)
{
    off_t value_pos = -1;
    DB_TRACE(("DB:db_remove_value_pos_in_rows_bucket: remove row_index = %d , field_index = %d\n", row_index, field_index));
    if(db_set_value_pos_in_rows_bucket_by_field_index(fd, table_pos, row_index, field_index, value_pos) == DB_FAILURE)
    {
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}
/* 
    Function: db_remove_row_in_rows_bucket
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */

db_boolean_t db_remove_row_in_rows_bucket(int fd, off_t table_pos, db_index_t row_index)
{
    int i;
    
    db_flag_t flag = DB_FLAG_NOT_USED;
    if(db_set_flag_in_rows_bucket(fd, table_pos, row_index, flag) == DB_FAILURE)
    {
        return DB_FAILURE;
    }
    DB_TRACE(("DB:db_remove_row_in_rows_bucket: set flag = %d\n", (int)flag));

    for(i = 1; i <= DB_MAX_FIELDS_IN_TABLE; i++)
    {
        if(db_remove_value_pos_in_rows_bucket(fd, table_pos, row_index, i) == DB_FAILURE)
        {
            return DB_FAILURE;
        }
    }
    DB_TRACE(("DB:db_remove_row_in_rows_bucket: remove row_index = %d\n", row_index));
    return DB_SUCCESS;
}

/* 
    Function: db_get_value_index_from_rows_bucket
    Params: 
    Description: 
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
int db_get_value_index_from_rows_bucket(int fd, int table_pos, int row_index, int field_index)
{
    off_t value_pos = db_get_value_pos_from_rows_bucket_by_field_index(fd, table_pos, row_index, field_index);
    if(value_pos == -1)
    {
        return -1;
    }
    off_t field_pos = db_point_to_fields_bucket_by_index(fd, table_pos, field_index);
    int value_index = (value_pos - field_pos)/(DB_UNIT_SIZE_IN_FIELDS_BUCKET);
    
    DB_TRACE(("DB:db_get_value_index_from_rows_bucket: value_index = %d\n", value_index));
    return value_index;
}