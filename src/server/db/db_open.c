/* 
    File: db_def.h
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
        DB_TRACE(("flag param is wrong!"));
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
        if(db_strlen(db_name) > DB_MAX_LENGTH_DB_NAME)
        {
            DB_SET_ERROR(DB_OUT_OF_BOUNDS);
            return NULL;
        }

        DB_TRACE(("DB:db_open:write:db_name: %s\n", db_name));
        ret_val = db_write(db->fd, db_name, db_strlen(db_name));
        if(ret_val != db_strlen(db_name))
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }else{
            ret_val = db_write(db_fd, "\0", 1);
            if(ret_val != 1)
            {
                DB_SET_ERROR(DB_WRITE_WRONG);
                return DB_NULL;
            }
        }
        // Write number table in database
        if(db_seek(db->fd, DB_POS_NUMBER_TABLE, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        DB_TRACE(("DB:db_open::write:DB_POS_NUMBER_TABLE = %d\n", DB_POS_NUMBER_TABLE));
        // Because this is new db, number table is 0
        int num_table = 0;
        ret_val = db_write(db->fd, &num_table, DB_INT_SIZE);
        if(ret_val != DB_INT_SIZE)
        {
            perror("write!");
            DB_TRACE(("DB:db_open:write: ret_val = %d\n", ret_val));
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        // Write last_position
        off_t last_position = db_seek(db->fd, DB_POS_LAST_POSITION, DB_BEGIN_FD) + DB_OFF_T_SIZE;
        DB_TRACE(("DB:db_open:write: last_position = %ld\n", last_position));
        if(last_position - DB_OFF_T_SIZE == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        
        ret_val = db_write(db->fd, &last_position, DB_OFF_T_SIZE);
        if(ret_val != DB_OFF_T_SIZE)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
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
    db->tables = (db_table_info *) db_alloc(db->num_table * DB_TABLE_INFO_SIZE);
    
    // Read info in table
    {
        pos = DB_POS_FIRST_TABLE_POS;
        if(db_seek(db->fd, pos, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }

        int index;
        for(index = 0; index < db->num_table; index++)
        {
            ret_val = db_read(db->fd, &(db->tables[index].position_table), DB_OFF_T_SIZE);
            DB_TRACE(("DB:db_open:db_read: offsize = %ld", db->tables[index].position_table));
            if(ret_val != DB_OFF_T_SIZE)
            {
                DB_TRACE(("DB:db_open:db_read: wrong when get position in table %d, pos = %d", index, ret_val));
                DB_SET_ERROR(DB_READ_WRONG);
                return DB_NULL;
            }
            pos+=ret_val;
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