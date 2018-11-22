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
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
/* 
        Function: db_open_file
        Params: file,
                flag,
                mode
        Description: Open a database
        
 */
static inline int db_open_file(char *file,int flag, int mode)
{
    // DB_TRACE(("file = %s\nflag = %d\nmode = %d\n", file, flag, mode));
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
    sprintf(db_file, "%s%s", db_path, db_file);

    db_fd = db_open_file(db_file, open_flag, mode);
    if(db_fd == -1)
    {
        DB_TRACE(("open file error!\n"));
        return NULL;
    }
    // Alloc memory for data
    db = (DATABASE) db_alloc(DB_SIZE_DATABASE_TYPE);

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
        DB_TRACE(("DB:db_open: seek to name_db!\n"));

        // Check length of db_name
        if(db_strlen(db_name) > MAX_LENGTH_DB_NAME)
        {
            DB_SET_ERROR(DB_OUT_OF_BOUNDS);
            return NULL;
        }
        ret_val = db_write(db->fd, db_name, db_strlen(db_name));
        if(ret_val != db_strlen(db_name) || ret_val != MAX_LENGTH_DB_NAME)
        {
            DB_SET_ERROR(DB_WRITE_WRONG);
            return DB_NULL;
        }
        DB_TRACE(("DB:db_open: get name_db!\n"));
        // Write number table in database
        if(db_seek(db->fd, DB_POS_NAME_DATABASE, DB_BEGIN_FD) == -1)
        {
            DB_SET_ERROR(DB_SEEK_FD_FAIL);
            return DB_NULL;
        }
        // Because this is new db, number table is 0
        ret_val = db_write(db->fd, 0, DB_INT_SIZE);
    }

    // Load all data in db
    {   
        int ret_val, pos = 0;
        db->fd = db_fd;
        db_seek(db->fd, 0, DB_BEGIN_FD);
        // Read 32 byte name db
        db->database_name = (char *) db_alloc(MAX_LENGTH_DB_NAME);
        ret_val = db_read(db->fd, db->database_name, MAX_LENGTH_DB_NAME);
        DB_TRACE(("DB:db_open:db_read: database name: %s", db->database_name));

        if(ret_val < 0 || ret_val > MAX_LENGTH_DB_NAME)
        {
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_NULL;
        }else{
            db->database_name[ret_val] = '\0';
        }
        // Read all table in db
        pos += MAX_LENGTH_DB_NAME;
        db_seek(db->fd, pos, DB_BEGIN_FD);

        ret_val = db_read(db->fd, &(db->num_table), DB_INT_SIZE);
        DB_TRACE(("DB:db_open:db_read: number table = %d", ret_val));
        if(ret_val != DB_INT_SIZE)
        {
            DB_SET_ERROR(DB_READ_WRONG);
            return DB_NULL;
        }
        // Alloc table
        db->tables = (db_table_info *) db_alloc(db->num_table * DB_TABLE_INFO_SIZE);

        pos += ret_val;
        db_seek(db->fd, pos, DB_BEGIN_FD);
        // Read info in table
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

    return db;
}