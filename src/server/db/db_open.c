/* 
    File: db_open.c
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"
#include "db_def.h"
#include "db_alloc.h"
#include "db_error.h"
#include "db_core_funcs.h"
#include "db_global.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <error.h>
/* 
        Function: db_open_file
        Params: file,
                flag,
                mode
        Description: Open a database
        
 */
static inline int db_open_file(char *file,int flag, int mode)
{
    // DB_TRACE(("DB:db_open_file:file = %s,flag = %d,mode = %d\n", file, flag, mode));
    return open(file, flag, mode);
}

/* 
        Function: db_read_table
        Params: db,
                
        Description: Read table info from database
                     Like: name, num_field, fields
        
        Return value: DB_FAILURE if error
                      DB_SUCCESS if success
        
 */
static inline db_boolean_t db_read_table_info(DATABASE db)
{
    int i, j;
    off_t pos;
    ssize_t io_ret_val;
    for (i = 0; i < db->num_table; i++)
    {
        /* Read table id */
        pos = db->tables[i].position_table + DB_POS_ID_TABLE;
        if (db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_FAILURE;
        }

        io_ret_val = db_read(db->fd, &(db->tables[i].id_table), DB_INT_SIZE);
        DB_TRACE(("DB:db_read_table_info: table id = %d at %ld\n", db->tables[i].id_table, pos));
        if(io_ret_val != DB_INT_SIZE)
        {
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_FAILURE;
        }

        /* Read table name */
        pos = db->tables[i].position_table + DB_POS_NAME_TABLE;
        
        if (db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_FAILURE;
        }
        
        db->tables[i].table_name = (char *) db_alloc(DB_MAX_LENGTH_TABLE_NAME);
        io_ret_val = db_read(db->fd, db->tables[i].table_name, DB_MAX_LENGTH_TABLE_NAME);
        DB_TRACE(("DB:db_read_table_info: table name = %s at %ld\n", db->tables[i].table_name, pos));
        
        if (io_ret_val != DB_MAX_LENGTH_TABLE_NAME)
        {
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_FAILURE;
        }

        /* Read number field */
        pos = db->tables[i].position_table + DB_POS_NUM_FIELD_IN_TABLE;
        
        if (db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_FAILURE;
        }

        io_ret_val = db_read(db->fd, &(db->tables[i].num_fields), DB_U_8_BIT_SIZE);
        DB_TRACE(("DB:db_read_table_info: number fields = %d at %ld\n", db->tables[i].num_fields, pos));
        if (io_ret_val != DB_U_8_BIT_SIZE)
        {
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_FAILURE;
        }
        /* Alloc memory for fields */
        db->tables[i].fields = (db_field *)db_alloc(DB_MAX_FIELDS_IN_TABLE * DB_FIELD_INFO_SIZE);
        for(j = 0; j < DB_MAX_FIELDS_IN_TABLE; j++)
        {
            db->tables[i].fields[j].index = -1;
        }
        /* Read fields */
        for(j = 0; j < db->tables[i].num_fields; j++)
        {
            // Alloc memory for field
            db_field field;
            
            // Read field name
            field.field_name = (char *)db_alloc(DB_MAX_LENGTH_FIELD_NAME);
            io_ret_val = db_read(db->fd, field.field_name, DB_MAX_LENGTH_FIELD_NAME);
            DB_TRACE(("DB:db_read_table_info: field name = %s at %ld\n", field.field_name, db_seek(db->fd, 0, DB_CURRENT_FD) - DB_MAX_LENGTH_TABLE_NAME ));
            if (io_ret_val != DB_MAX_LENGTH_FIELD_NAME)
            {
                DB_SET_ERROR(DB_READ_WRONG);
                return DB_FAILURE;
            }
            // Read index of field
            io_ret_val = db_read(db->fd, &(field.index), DB_INDEX_T_SIZE);
            DB_TRACE(("DB:db_read_table_info: index in table = %d at %ld\n", field.index, db_seek(db->fd, 0, DB_CURRENT_FD) - DB_INDEX_T_SIZE));
            if (io_ret_val != DB_INDEX_T_SIZE)
            {
                DB_SET_ERROR(DB_READ_WRONG);
                return DB_FAILURE;
            }

            // Assign field
            db->tables[i].fields[field.index] = field;
        }

    }

    return DB_SUCCESS;
}
/* 
        Function: db_open
        Params: db_name,
                db_path,
                flag
        Description: Open a database
        
 */
DATABASE db_open(char *db_name, char *db_path, int flag)
{
    // Declare variable
    DATABASE db;
    int mode = S_IRWXU | S_IRWXG | S_IRWXO;
    int open_flag;
    int db_fd;
    
    DB_RESET_ERROR();
    // open database
    switch(flag)
    {
        case DB_OPEN:
            open_flag = O_RDWR;
            break;
        case DB_CREATE:
            open_flag = O_RDWR | O_CREAT ;
            break;
        default:
            open_flag = -1;
    }

    // Check flag param
    if(open_flag == -1)
    {
        DB_TRACE(("DB:db_open: Flag param is wrong!"));
        return NULL;
    }
    // Get descriptor in database
    char db_file[DB_MAX_LENGTH_DATABASE_PATH];
    sprintf(db_file, "%s%s", db_path, db_name);

    db_fd = db_open_file(db_file, open_flag, mode);
    DB_TRACE(("DB:db_open: file descriptor: %d\n", db_fd));
    if(db_fd == -1)
    {
        DB_TRACE(("open file error!\n"));
        return NULL;
    }
    // Alloc memory for data
    db = (DATABASE) db_alloc(DB_SIZE_DATABASE_TYPE);
    // Assing file descriptor in database
    db->fd = db_fd;
    //Create database
    if(flag == DB_CREATE)
    {
        int ret_val;
        DB_TRACE(("DB:db_open: Create a new db!\n"));
        // Write name in database
        if(db_seek(db->fd, DB_POS_NAME_DATABASE, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        DB_TRACE(("DB:db_open:write:seek to name_db!\n"));

        // Check length of db_name
        if(db_strlen(db_name) + 1 > DB_MAX_LENGTH_DB_NAME)
        {
            DB_SET_ERROR(DB_OUT_OF_BOUNDS);
            return NULL;
        }

        int num_write = db_strlen(db_name) + 1;
        DB_TRACE(("DB:db_open:write:db_name: %s at %ld\n", db_name, db_seek(db->fd, 0, DB_CURRENT_FD) ));
        ret_val = db_write(db->fd, db_name, num_write);
        if(ret_val != num_write)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        // Write number table in database
        if(db_seek(db->fd, DB_POS_NUMBER_TABLE, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        
        // Because this is new db, number table is 0
        int num_table = 0;
        DB_TRACE(("DB:db_open::write:num_table = %d at %ld\n", num_table, db_seek(db->fd, 0, DB_CURRENT_FD) ));
        ret_val = db_write(db->fd, &num_table, DB_INT_SIZE);
        if(ret_val != DB_INT_SIZE)
        {
            perror("write!");
            DB_TRACE(("DB:db_open:write: ret_val = %d\n", ret_val));
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        // Write last_position
        off_t last_position = DB_POS_LAST_POSITION + DB_OFF_T_SIZE;
        if(db_set_last_position(db->fd, last_position) == DB_FAILURE)
        {
            return DB_NULL;
        }
        
    }

    /* Read from db, info about database */
    int ret_val, pos;
    // Load database name
    {
        pos = DB_POS_NAME_DATABASE;
        if(db_seek(db->fd, DB_POS_NAME_DATABASE, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        // Read 32 byte name db
        db->database_name = (char *) db_alloc(DB_MAX_LENGTH_DB_NAME);
        ret_val = db_read(db->fd, db->database_name, DB_MAX_LENGTH_DB_NAME);
        DB_TRACE(("DB:db_open:db_read: database name: %s\n", db->database_name));

        if(ret_val < 0 || ret_val > DB_MAX_LENGTH_DB_NAME)
        {
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_NULL;
        }else{
            db->database_name[ret_val] = '\0';
        }
    }
    
    
    {
        // Read num table in db
        pos = DB_POS_NUMBER_TABLE;
        if(db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }

        ret_val = db_read(db->fd, &(db->num_table), DB_INT_SIZE);
        DB_TRACE(("DB:db_open:db_read: number table = %d\n", db->num_table));
        if(ret_val != DB_INT_SIZE)
        {
            DB_TRACE(("DB:db_open:db_read: ret_val = %d\n", ret_val));
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_NULL;
        }
    }

    // Alloc table
    DB_TRACE(("DB:db_open:db_alloc: Alloc memory for all table\n"));
    db->tables = (db_table_info *) db_alloc(DB_MAX_TABLE_IN_DATABASE * DB_TABLE_INFO_SIZE);
    
    // Read info in table
    {
        int index;
        for(index = 0; index < db->num_table; index++)
        {
            db->tables[index].position_table = db_get_position_index_table(db->fd, index);
            if (db->tables[index].position_table == -1)
            {
                return DB_NULL;
            }

        }
    
        if(db_read_table_info(db) == DB_FAILURE)
        {
            return DB_NULL;
        }
    }


    // Read last_position
    if(db_seek(db->fd, DB_POS_LAST_POSITION, DB_BEGIN_FD) == -1)
    {
        DB_SET_ERROR(DB_SEEK_FD_FAIL);
        return DB_NULL;
    }

    ret_val = db_read(db->fd, &(db->last_position), DB_OFF_T_SIZE);
    if(ret_val != DB_OFF_T_SIZE)
    {
        DB_SET_ERROR(DB_READ_WRONG);
        return DB_NULL;
    }
    DB_TRACE(("DB:db_open:db_read: last_position= %ld\n", db->last_position));
    return db;
}